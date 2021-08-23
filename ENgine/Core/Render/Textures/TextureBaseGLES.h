
#pragma once

#include "TextureBase.h"


class TextureBasePlatform : public TextureBase
{	
protected:
	GLuint texture;
	GLenum target;

public:	

	TextureBasePlatform(const char* fl, int w, int h, int f, int l, GLenum t);

	virtual void SetFilters(FilterType magmin, FilterType mipmap);
	virtual void SetAdress(TextureAddress adress);

#ifdef ANDROID
	virtual bool HasSecData();
	virtual void* GetSecData();
#endif

	virtual GLuint GetInternalTexture();
	virtual GLenum GetInternalTarget();
};
