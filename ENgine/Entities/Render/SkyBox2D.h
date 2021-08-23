#pragma once
#include "Core/Core.h"

class SkyBox2D : public SceneEntity
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

	FILENAME_PROP(SkyBox2D, tex_name, "", "Properties", "texture", "*.*")

	ENTITY_DEFC(SkyBox2D)
	virtual ~SkyBox2D();

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();

	void DrawSprite(Vector2& uv);
	void Render(float dt, int level);
};
