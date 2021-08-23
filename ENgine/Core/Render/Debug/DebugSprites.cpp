#include "Core/Core.h"

DebugSprites::DebugSprites()
{   
   vbuffer = null;   
}

DebugSprites::~DebugSprites()
{    
}

bool DebugSprites::Init()
{    
	en_core.Render()->Tasks()->AddTask("debug_sprite", this, this, (TaskHolder::Task)&DebugSprites::Draw, 0);

	tech = en_core.Render()->CreateRenderTechnique("UI");
	
	vbuffer = en_core.Render()->CreateVertexBuffer(sizeof(SpriteVertex) * 4,sizeof(SpriteVertex), _FL_);

	vbuffer->StartDesc();
	vbuffer->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");
	vbuffer->AddDescElem(VertexBuffer::texcoord,VertexBuffer::float2, "texCoord");			
	vbuffer->EndDesc();
	
	SpriteVertex* Data = (SpriteVertex*)vbuffer->Lock();			

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

	Data[0].Texcoords.y = 0.0f;
	Data[1].Texcoords.y = 1.0f;
	Data[2].Texcoords.y = 0.0f;
	Data[3].Texcoords.y = 1.0f;

	vbuffer->Unlock();
					
    return true;
}

void DebugSprites::AddSprite(Texture* texture, const Vector2& pos, const Vector2& size,
							 ElemAlign corner, const Color& color, float angle,
							 const Vector2& uv1, const Vector2& uv2)
{
//	if (!texture) return;

	if (sprites.size()>1000) return;

	sprites.push_back(Sprite());
	Sprite* spr = &sprites[sprites.size()-1];

	spr->texture = texture;
	
	spr->pos = pos;
	spr->size = size;

	spr->color = color;
	
	spr->uv1 = uv1;
	spr->uv2 = uv2;
	
	spr->angle = angle;
	
	if (corner == 1 || corner == 2)
	{		
		spr->pos.x = en_core.Render()->Display()->GetWidth() - spr->pos.x - spr->size.x;
	}

	if (corner == 2 || corner == 3)
	{
		spr->pos.y = en_core.Render()->Display()->GetHeight() - spr->pos.y - spr->size.y;
	}
}

void DebugSprites::Draw(float dt, int level)
{	
	if (sprites.size() == 0) return;

	en_core.Render()->SetVertexBuffer( 0, vbuffer );
	en_core.Render()->SetRenderTechnique(tech);

	Vector4 params[4];

	for (int i=0;i<sprites.size();i++)
	{		
		tech->PS_SetTexture("diffuseMap", sprites[i].texture);

		params[0] = Vector4(sprites[i].uv1.x,sprites[i].uv1.y,
						    sprites[i].uv2.x,sprites[i].uv2.y);

		params[1] = Vector4(sprites[i].pos.x,sprites[i].pos.y,
						    sprites[i].size.x,sprites[i].size.y);
		
		params[2] = Vector4(en_core.Render()->Display()->GetWidth(),
							en_core.Render()->Display()->GetHeight(),
							sin(RADIAN * sprites[i].angle),cos(RADIAN * sprites[i].angle));

		tech->VS_SetVector4Array("desc", 3, &params[0].x);
		
		params[3] = Vector4(sprites[i].color.r, sprites[i].color.g,
							sprites[i].color.b, sprites[i].color.a);
				
		tech->PS_SetVector4("color", params[3]);

		en_core.Render()->DrawPrimitive( RenderService::triangleStrip, 0, 2 );
	}

	sprites.clear();
}

void DebugSprites::Release()
{
	if (vbuffer)
	{
		vbuffer->Release();
		vbuffer = null;
	}
}