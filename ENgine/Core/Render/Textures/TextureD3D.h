
#pragma once

#ifdef PC
#define NOMINMAX
#include "d3d9.h"
#endif

#include "Texture.h"

class TextureD3D : public Texture
{			
	IDirect3DTexture9* texture;
	byte* dummy_data;
	bool ready;

public:
	
	TextureD3D(int w, int h, int f, int l, IDirect3DTexture9* tex);
		
	virtual bool Load(const char* name, bool async);	
	virtual void FlushData(TextureData* texture_data);

	virtual void ApplyStates(int slot);

	virtual bool Lock(int face, int lod, Texture::LockRect& rect);	
	virtual bool Unlock(int face, int lod);	
	virtual bool UpdateLod(int face, int lod, int rect_x, int rect_y, int rect_w, int rect_h, byte* data, int size);
	virtual void* GetData();
	virtual bool IsReady();
	virtual void Release();	
};
