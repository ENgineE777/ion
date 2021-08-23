#pragma once

#include "Texture.h"

#ifdef IOS
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#endif

#if defined(ANDROID) || defined(OSUNIX)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#ifdef OSX
#import	<OpenGL/gl.h>
#import	<OpenGL/glu.h>
#endif

class TextureGLES : public Texture
{				
	GLuint texture;
	GLenum target;

	bool tex_created;    
    bool ready;

public:	
	
	TextureGLES(int w, int h, int f, int l);	
		
	virtual bool Load(const char* name, bool async);
	virtual void FlushData(TextureData* texture_data);

	void CreateGLTexture();
	GLenum GetTarget();

	virtual bool Lock(int face, int lod, Texture::LockRect& rect);		
	virtual bool Unlock(int face, int lod);	
	virtual bool UpdateLod(int face, int lod, int rect_x, int rect_y, int rect_w, int rect_h, byte* data, int size);
	virtual void* GetData();
	virtual void SetFilters(Texture::FilterType magmin, Texture::FilterType mipmap);
	virtual void SetAdress(Texture::TextureAddress adress);
    virtual void Reset();
    virtual bool IsReady();

	virtual void Release();
};
