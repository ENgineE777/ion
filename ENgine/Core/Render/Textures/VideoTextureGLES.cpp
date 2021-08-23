
#include "VideoTextureGLES.h"
#include "Core/Core.h"

#if defined(ANDROID) || defined(OSUNIX)
VideoTextureGLES::VideoTextureGLES(const char* path): 
	Texture(path, 0, 0, 0, 1, GL_TEXTURE_EXTERNAL_OES)
{	
	tex_created = false;
	CreateGLTexture(GL_TEXTURE_EXTERNAL_OES);
}
#endif

#ifdef IOS
VideoTextureGLES::VideoTextureGLES(const char* path):
Texture(path, 0, 0, 0, 1, GL_TEXTURE_2D)
{
	tex_created = false;
	CreateGLTexture(GL_TEXTURE_2D);
}
#endif

void VideoTextureGLES::CreateGLTexture(GLenum t)
{	
	glGenTextures(1, &texture);	

	glActiveTexture(GL_TEXTURE0);	
	
	glBindTexture(t, texture);	

	if (lods == 1)
	{
		glTexParameteri(t, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		checkGlError2("glTexParameteri");

		glTexParameteri(t, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		checkGlError2("glTexParameteri");
	}
	else
	{
		glTexParameteri(t, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		checkGlError2("glGenTextures");

		glTexParameteri(t, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		checkGlError2("glTexParameteri");
	}

	tex_created = true;
}

void* VideoTextureGLES::GetData()
{		
	return &texture;
}


void VideoTextureGLES::Reset()
{
	glDeleteTextures(1,&texture);
	tex_created = false;
}

void VideoTextureGLES::Release()
{	
	if (!en_core.Render()->ResoursesAreDead())
	{
		if (tex_created)
		{
			glDeleteTextures(1,&texture);			
		}
	}

	delete this;
}