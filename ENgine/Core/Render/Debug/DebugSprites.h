
#pragma once

#include "Common/Color.h"

class DebugSprites : public TaskHoldersOwner
{
	RenderTechnique* tech;
	VertexBuffer* vbuffer;

    struct SpriteVertex
	{
		Vector  Position;
		Vector2 Texcoords;
	};   
    
	struct Sprite
	{
		Texture* texture;
		Vector2 pos;
		Vector2 size;
		Color color;
		Vector2 uv1;
		Vector2 uv2;
		float angle;
	};

	std::vector<Sprite> sprites; 
	
public:
   
	DebugSprites();
	virtual ~DebugSprites();

	bool Init();	
	void AddSprite(Texture* texture, const Vector2& pos, const Vector2& size,
				   ElemAlign corner, const Color& color, float angle,
				   const Vector2& uv1, const Vector2& uv2);

	void Draw(float dt, int level);

	void Release();
};

