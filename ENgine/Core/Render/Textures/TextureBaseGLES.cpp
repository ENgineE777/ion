
#include "TextureBaseGLES.h"
#include "Core/Core.h"

static void checkGlError3(const char* op)
{
	for (GLint error = glGetError(); error; error = glGetError())
	{
		core->TraceTo("Render", "%s: after %s() glError (0x%x)\n", __FILE__, op, error);
	}
}

const char* getConstantName(GLenum target);

TextureBasePlatform::TextureBasePlatform(const char* fl, int w, int h, int f, int l, GLenum t):TextureBase(fl, w, h, f, l)
{
	target = t;
}

void TextureBasePlatform::SetFilters(FilterType magmin, FilterType mipmap)
{	
	//if (magminf == magmin && mipmapf == mipmap) return;

	magminf = magmin;

	if (lods != 1)
	{
		mipmapf = mipmap;
	}

	glBindTexture( target, texture);	
		
	if (mipmapf != filterNone)
	{
		if(magminf == filterPoint && mipmapf == filterPoint)
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); 
			//glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST); 
		}
		else if(magminf == filterLinear && mipmapf == filterPoint)
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 
			//glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST); 
		}
		else if(magminf == filterPoint && mipmapf == filterLinear)
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			//glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		}
		else if(magminf == filterLinear && mipmapf == filterLinear)
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			//glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); 
			//glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		}
	}
	else
	{		
		if(magminf == filterLinear)
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}
	glBindTexture (target, 0);
}

void TextureBasePlatform::SetAdress(TextureAddress adress)
{	
	//if (texture == 0)	return;	
	//if (adressUVW == adress)	return;

	adressUVW = adress;	

	glBindTexture( target, texture);	

	if(adressUVW == addressWrap)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);		
	}
	/*else
	if(adressUVW == addressBorder)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}*/
	else if(adressUVW == addressClamp)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);		
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
	}
	else if(adressUVW == addressMirror)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);		
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);		
	}
	else
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);		
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);		
	}

	//glBindTexture (target, 0);
}

GLuint TextureBasePlatform::GetInternalTexture() { return texture; }
GLenum TextureBasePlatform::GetInternalTarget() { return target; }

#ifdef ANDROID

bool TextureBasePlatform::HasSecData()
{
	return false;
}

void* TextureBasePlatform::GetSecData()
{
	return 0;
}

#endif