#pragma once

#include "Texture.h"

class VideoTextureGLES : public Texture
{		
	bool tex_created;
public:	

	VideoTextureGLES(const char* path);
	void    CreateGLTexture();
	void*	GetData();
    virtual void Reset();
	virtual void Release();

	//empty
    virtual bool Lock(int face, int lod, Texture::LockRect& rect) { return false; };
    virtual bool Unlock(int face, int lod) { return false; };
    virtual bool UpdateLod(int face, int lod, int rect_x, int rect_y, int rect_w, int rect_h, byte* data, int size) { return false;};
	virtual void FlushData() {};
};
