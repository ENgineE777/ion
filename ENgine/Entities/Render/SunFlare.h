#pragma once
#include "Core/Core.h"

class SunFlare : public SceneEntity
{		
	RenderTechnique* tech;
	VertexBuffer* vbuffer;

    struct Vertex
	{
		Vector  Position;
		Vector2 Texcoords;
	};   

	TextureInstance* tex;

public:
		
	PROP_START

	FILENAME_PROP(SunFlare, tex_name, "", "Properties", "texture", "*.*")
	INT_PROP(SunFlare, size, 100, "Properties", "size")

	ENTITY_DEFC(SunFlare)
	virtual ~SunFlare();

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();

	void DrawSprite(Vector2& pos, Vector2& size);
	void Render(float dt, int level);
};
