

#include "Emiter.h"
#include "FxRes.h"
#include "Core/core.h"	



CEmiter::CEmiter(void)
:m_SpawnTimeCounter(0.f),
m_Desc(NULL),
m_vbuffer(NULL),
m_Texture(NULL),
m_LifeTime(0)
{
	
	tech = core->Render()->CreateRenderTechnique("DebugLine");	
}

CEmiter::~CEmiter(void)
{
	Release();
}

void CEmiter::SetDesc( const CFxRes::SEmiterDesc* _desc)
{
	m_Desc = _desc;
	OnChangeDesc();		
}


void CEmiter::OnChangeDesc()
{
	if (m_Texture)
	{
		m_Texture->Release();
		m_Texture = NULL;
	}
	m_Active = false;
	m_Particles.swap(std::vector<SParticle>());
	// create d3d buffer for unpacked verts
	if (m_vbuffer)
	{
		m_vbuffer->Release();
		m_vbuffer = NULL;
	}

	if (!m_Desc)
		return;

	char fullPath[512];
	strcpy(fullPath, core->Files()->GetAppDirectory());
	strcat( fullPath, m_Desc->TextureName );
	m_Texture = core->Render()->LoadTexture( fullPath, _FL_);
	m_Active = true;
	m_Particles.reserve(m_Desc->ParticlesCount);

	//core->Render()->Device()->CreateVertexBuffer( sizeof( vertex ) * 6 * m_Desc->ParticlesCount,
	//													 D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_vbuffer, NULL );

	if (m_Desc->ParticlesCount)
	{
		m_vbuffer = core->Render()->CreateVertexBuffer(sizeof( vertex ) * 6 * m_Desc->ParticlesCount,
			sizeof(vertex),false, _FL_);			
		m_vbuffer->StartDesc();
		m_vbuffer->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");		
		m_vbuffer->AddDescElem(VertexBuffer::color,VertexBuffer::tp_color, "color");
		m_vbuffer->AddDescElem(VertexBuffer::texcoord,VertexBuffer::float2, "texCoord");			
		m_vbuffer->EndDesc();
	}

}

void CEmiter::Release(void)
{
	m_Desc = NULL;
	OnChangeDesc();
}

void CEmiter::Update(const float dt) 
{
	if (!m_Desc)
		return;

	m_LifeTime += dt;

	if ( m_Desc->Looped && m_LifeTime >= m_Desc->LifeTime)
		m_LifeTime = 0;

	if (  m_LifeTime >= m_Desc->LifeTime)
		return;
	m_SpawnTimeCounter += dt;
	if (m_SpawnTimeCounter >= m_Desc->SpawnTime)
	{
		int SpawnCount = (int)m_Desc->SpawnRate;
		if (SpawnCount+(unsigned int)m_Particles.size() > m_Desc->ParticlesCount)
			SpawnCount = m_Desc->ParticlesCount-(int)m_Particles.size();

		if (!m_Active) SpawnCount = 0;
		for (int i=0;  i<SpawnCount;  ++i)
		{
			SParticle Particle;
			Particle.Active		= true;
			Particle.Life		= rnd_range(m_Desc->LifeTimeMin, m_Desc->LifeTimeMax);
			Particle.LifeInit =	Particle.Life;
			Particle.Position.Rand(m_Transform.Pos()+m_Desc->PositionMin, m_Transform.Pos()+m_Desc->PositionMax);
			Particle.Velocity.Rand(m_Desc->VelocityMin, m_Desc->VelocityMax);
			Particle.Acceleration.Rand(m_Desc->AccelerationMin, m_Desc->AccelerationMax);
			Particle.sizeX = rnd_range( m_Desc->SizeMinX, m_Desc->SizeMaxX );
			Particle.sizeY = rnd_range( m_Desc->SizeMinY, m_Desc->SizeMaxY );
			m_Particles.push_back(Particle);
		}	

		m_SpawnTimeCounter = 0.0f;
	}	


	if (m_Particles.size() && m_vbuffer)
	{
		vertex* dst;
		dst = (vertex*)m_vbuffer->Lock(0);

		Matrix view;
		core->Render()->GetView(view);
		view.Inverse();
		view.Pos() = Vector(0.f, 0.f, 0.f);

		SParticle* pLast = &m_Particles.back();
		for(SParticle* p = &m_Particles.front(); p <= pLast; ++p)
		{
			p->Position += p->Velocity * dt;
			p->Velocity += p->Acceleration * dt;
			p->Life -= dt;
			float rate =  p->Life / p->LifeInit;
			float alfa = 1.f;
			if (rate < m_Desc->Fade)
				alfa = rate/m_Desc->Fade;
			
			p->alfa = alfa;

			if (p->Life <= 0.0f)
			{
				p->Active = false;
				*p = *pLast;
				m_Particles.pop_back();
				--pLast; 

			}

			//float sizeX = rangeRand( m_Desc->SizeMin, m_Desc->SizeMax );
			//float sizeY = rangeRand( m_Desc->SizeMin, m_Desc->SizeMax );
			//p->sizeX = sizeX;
			//p->sizeY = sizeY;
			p->sizeX += p->sizeX * m_Desc->scaleSizePerSec*dt;
			p->sizeY += p->sizeY * m_Desc->scaleSizePerSec*dt;
			dst->xyz = Vector(0, 0, 0);
				dst->xyz = view.MulVertex(dst->xyz);
				dst->xyz = dst->xyz + m_Transform.MulVertex(p->Position);				
				dst->color = m_Desc->Color[0];
				((byte*)&dst->color)[3] = alfa;
				dst->tu = 0;
				dst->tv = 1;

				++dst;
				dst->xyz = Vector(0, p->sizeY, 0);		
				dst->xyz = view.MulVertex(dst->xyz);
				dst->xyz = dst->xyz + m_Transform.MulVertex(p->Position);				
				dst->color = m_Desc->Color[1];
				((byte*)&dst->color)[3] = alfa;
				dst->tu = 0;
				dst->tv = 0;

				++dst;
				dst->xyz = Vector(p->sizeX, p->sizeY, 0);		
				dst->xyz = view.MulVertex(dst->xyz);
				dst->xyz = dst->xyz + m_Transform.MulVertex(p->Position);				
				dst->color = m_Desc->Color[2];
				((byte*)&dst->color)[3] = alfa;
				dst->tu = 1;
				dst->tv = 0;

				++dst;
				dst->xyz = Vector(0, 0, 0);		
				dst->xyz = view.MulVertex(dst->xyz);
				dst->xyz = dst->xyz + m_Transform.MulVertex(p->Position);				
				dst->color = m_Desc->Color[0];
				((byte*)&dst->color)[3] = alfa;
				dst->tu = 0;
				dst->tv = 1;

				++dst;
				dst->xyz = Vector(p->sizeX, p->sizeY, 0);		
				dst->xyz = view.MulVertex(dst->xyz);
				dst->xyz = dst->xyz + m_Transform.MulVertex(p->Position);				
				dst->color = m_Desc->Color[1];
				((byte*)&dst->color)[3] = alfa;
				dst->tu = 1;
				dst->tv = 0;

								
				++dst;
				dst->xyz = Vector(p->sizeX, 0, 0);		
				dst->xyz = view.MulVertex(dst->xyz);
				dst->xyz = dst->xyz + m_Transform.MulVertex(p->Position);				
				dst->color = m_Desc->Color[2];
				((byte*)&dst->color)[3] = alfa;
				dst->tu = 1;
				dst->tv = 1;

				++dst;
		}
		m_vbuffer->Unlock();
	}
	
}

void CEmiter::Draw()
{
	if (m_LifeTime >= m_Desc->LifeTime)
		return;

	if (m_Particles.size())
	{
		/*core->Render()->SetWorld(Matrix());
		core->Render()->Device()->SetStreamSource( 0, m_vbuffer, 0, sizeof( vertex ) );
		core->Render()->Device()->SetFVF( D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1 );
		core->Render()->BindTexture(m_Texture);
		core->Render()->Device()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_Particles.size() *2 );*/

		if ( m_Desc->type == eEmiterType::type3d)
		{
			core->Render()->SetRenderTechnique(tech);
			Matrix view_proj;
			core->Render()->GetViewProj(view_proj);		

			tech->VS_SetMatrix("modelViewProj", view_proj);	

			core->Render()->SetVertexBuffer(0,m_vbuffer);
			core->Render()->DrawPrimitive(RenderService::linesList, 0, m_Particles.size() *4);
			core->Render()->SetVertexBuffer(0,null);
		}	
		else
		{

			SParticle* pLast = &m_Particles.back();
			for(SParticle* p = &m_Particles.front(); p <= pLast; ++p)
			{
				Vector2 pos(  p->Position.x, p->Position.y);
				Vector2 size(p->sizeX, p->sizeY);
				pos.y *= core->Render()->GetHeight();
				pos.x *= core->Render()->GetHeight();
				size.y *= core->Render()->GetHeight();
				size.x *= core->Render()->GetHeight();
				core->Render()->DrawSprite(m_Texture,
					pos,
					size, 
					up_left,
					Color(1.0f, 1.0f, 1.0f, p->alfa), 
					0.f, Vector2(0, 0), Vector2(1, 1));
			}
		}


	}
	
}

void CEmiter::ReStart()
{
	m_LifeTime = 0;
}