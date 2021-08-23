#pragma once

#include "Core/Model/Mesh.h"
#include "Core/Files/FileBuffer.h"
#include "Core/Render/RenderService.h"

class ModelInstance;
class TextureInstance;

class ModelRes
{
public:
	
	int	ref_count;

	char md_name[64];

	Mesh::MeshHolder mesh;

	VertexBuffer* vbuffer;
	IndexBuffer*  ibuffer;		
	
	byte* buffer;

	ModelRes();

	bool Load(const char* name);
	bool LoadImpl(const char* name);

#ifdef PC
	void Save(const char* name, Platform platform);
#endif

	ModelInstance* CreateReference();
	void Draw(Matrix& transform, RenderTechnique* rt);
	void Release();	
};
