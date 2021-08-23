
#include "SkyBox2D.h"
#include "Common/Utils.h"

ENTITY(SkyBox2D)
ENTITY_PARAMS(SkyBox2D,"SkyBox2D","Environment","SkyBox 2D")

void SkyBox2D::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( tex_name )		
}

void SkyBox2D::Init()
{	
	tex = null;
	
	PROP_REG( tex_name )
		
	tech = en_core.Render()->CreateRenderTechnique("UI");
	
	vbuffer = en_core.Render()->CreateVertexBuffer(sizeof(Vertex) * 4, sizeof(Vertex), _FL_);

	vbuffer->StartDesc();
	vbuffer->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");
	vbuffer->AddDescElem(VertexBuffer::texcoord,VertexBuffer::float2, "texCoord");			
	vbuffer->EndDesc();

	Vertex* Data = (Vertex*)vbuffer->Lock();			

	Data[0].Position.x = 0.0f;
	Data[1].Position.x = 0.0f;
	Data[2].Position.x = 1.0f;
	Data[3].Position.x = 1.0f;

	Data[0].Position.y = 0.0f;
	Data[1].Position.y = 1.0f;
	Data[2].Position.y = 0.0f;
	Data[3].Position.y = 1.0f;

	Data[0].Position.z = 0.0f;
	Data[1].Position.z = 0.0f;
	Data[2].Position.z = 0.0f;
	Data[3].Position.z = 0.0f;

	Data[0].Texcoords.x = 0.0f;
	Data[1].Texcoords.x = 0.0f;
	Data[2].Texcoords.x = 1.0f;
	Data[3].Texcoords.x = 1.0f;

#ifdef IOS
	Data[0].Texcoords.y = 1.0f;
	Data[1].Texcoords.y = 0.0f;
	Data[2].Texcoords.y = 1.0f;
	Data[3].Texcoords.y = 0.0f;
#else
	Data[0].Texcoords.y = 0.0f;
	Data[1].Texcoords.y = 1.0f;
	Data[2].Texcoords.y = 0.0f;
	Data[3].Texcoords.y = 1.0f;
	
#endif

	vbuffer->Unlock();

	en_core.Render()->Tasks()->AddTask("geometry", this, Scene(), (TaskHolder::Task)&SkyBox2D::Render, -10);
}

SkyBox2D::~SkyBox2D()
{
	RELEASE(tex)
	en_core.Render()->Tasks()->DelTask("geometry", this);	
}

void SkyBox2D::ReInit()
{		
	RELEASE(tex)	
	tex = en_core.Render()->LoadTexture(tex_name.c_str(), false, _FL_);
}

void SkyBox2D::DrawSprite(Vector2& uv)
{
	en_core.Render()->SetVertexBuffer( 0, vbuffer );
	en_core.Render()->SetRenderTechnique(tech);

	Vector4 params[4];
	
	tech->PS_SetTexture("diffuseMap", tex);

	params[0] = Vector4(0.0f - uv.x,0.0f - uv.y,(float)en_core.Render()->Display()->GetWidth()/ (float)en_core.Render()->Display()->GetHeight(),1.0f);

	params[1] = Vector4(0.0f, 0.0f, (float)en_core.Render()->Display()->GetWidth(), (float)en_core.Render()->Display()->GetHeight());
		
	params[2] = Vector4(en_core.Render()->Display()->GetWidth(), en_core.Render()->Display()->GetHeight(), 0, 1);

	tech->VS_SetVector4Array("desc", 3, &params[0].x);
		
	params[3] = Vector4(1.0, 1.0f, 1.0f, 1.0f);

	tech->PS_SetVector4("color", params[3]);

	en_core.Render()->DrawPrimitive( RenderService::triangleStrip, 0, 2 );			
}

void SkyBox2D::Render(float dt, int level)
{		
	Matrix view;
	en_core.Render()->Display()->GetView(view);
	view.Inverse();

	float alpha, betta;
	ExtractAlpaBetta(view.Vz(), alpha, betta);

	Vector2 uv = Vector2(alpha/90.0f,betta/90.0f);
	DrawSprite(uv); 
}