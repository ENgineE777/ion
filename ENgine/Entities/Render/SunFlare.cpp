
#include "SunFlare.h"
#include "Entities/NGUI/NGUIService.h"


ENTITY(SunFlare)
ENTITY_PARAMS(SunFlare,"SunFlare","Environment","Sun Flare")

void SunFlare::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( tex_name )
	PROP_REG( size )	
}

void SunFlare::Init()
{	
	tex = null;		
		
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

	en_core.Render()->Tasks()->AddTask("geometry", this, Scene(), (TaskHolder::Task)&SunFlare::Render, -5);
}

SunFlare::~SunFlare()
{
	RELEASE(tex)
	RELEASE(vbuffer)
	en_core.Render()->Tasks()->DelTask("geometry", this);	
}

void SunFlare::ReInit()
{		
	RELEASE(tex)	
	tex = en_core.Render()->LoadTexture(tex_name.c_str(), false, _FL_);
}

void SunFlare::DrawSprite(Vector2& pos, Vector2& size)
{
	en_core.Render()->SetVertexBuffer( 0, vbuffer );
	en_core.Render()->SetRenderTechnique(tech);

	Vector4 params[4];
	
	tech->PS_SetTexture("diffuseMap", tex);

	params[0] = Vector4(0.0f,0.0f,1.0f,1.0f);

	params[1] = Vector4(pos.x, pos.y, size.x, size.y);
		
	params[2] = Vector4(en_core.Render()->Display()->GetWidth(), en_core.Render()->Display()->GetHeight(), 0, 1);

	tech->VS_SetVector4Array("desc", 3, &params[0].x);
		
	params[3] = Vector4(1.0, 1.0f, 1.0f, 1.0f);

	tech->PS_SetVector4("color", params[3]);

	en_core.Render()->DrawPrimitive( RenderService::triangleStrip, 0, 2 );			
}

void SunFlare::Render(float dt, int level)
{		
	Matrix view;
	en_core.Render()->Display()->GetView(view);
	view.Inverse();

	Vector pos = view.Pos() + transform.Vz() * 100.0f;

	float scale = view.Vz() | transform.Vz();

	if (scale < 0.0f) return;

	scale = 0.8f * (scale - 0.8f) / 0.2f + 0.2f;	

	Vector ps = en_core.Render()->Display()->TransformToScreen(pos, 2);
			
	Vector2 sz((float)size * NGUIService::instance->h_mul_RefToScreen,
			   (float)size * NGUIService::instance->v_mul_RefToScreen);

	sz *= scale;

	Vector2 p = Vector2(ps.x,ps.y) - sz * 0.5f;
	
	DrawSprite(p, sz); 	
}