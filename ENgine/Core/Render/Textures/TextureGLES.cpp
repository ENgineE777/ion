
#include "TextureGLES.h"
#include "Core/Core.h"

TextureGLES::TextureGLES(int w, int h, int f, int l) : Texture(w, h, f, l, tex2d)
{	
	texture = 0;	
	target = GL_TEXTURE_2D;
	
	tex_created = false;	
	
	CreateGLTexture();	
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, 0);
    
    ready = true;
}

void TextureGLES::CreateGLTexture()
{
	tex_created = true;    
    
	glGenTextures(1, &texture);	

	glActiveTexture(GL_TEXTURE0);	
	
    glBindTexture(target, texture);

	if (lods == 1)
	{
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 		
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		
	}
	else
	{
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 		
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		
	}
}

GLenum TextureGLES::GetTarget()
{
	return target;
}

bool TextureGLES::Load(const char* name, bool async)
{	
	if (async)
	{
		TextureData* texture_data = en_core.Render()->AddTextureToQuene(this, name);

		if (texture_data)
		{
			width = texture_data->width;
			height = texture_data->height;

			RenderService::texture_count++;
			RenderService::texture_memory += width * height * 4;
        
			ready = false;
		}

		return texture_data;
	}
	
	RenderService::TextureData texture_data;		

	String::Copy(texture_data.fl_name, 512, name);

	if (texture_data.GetInfo())
	{		
		if (texture_data.Load())
		{						
			RenderService::texture_count++;
			RenderService::texture_memory += width * height * 4;

			FlushData(&texture_data);			

			free(texture_data.data);			

			return true;
		}
	}		

	return false;	
}

void TextureGLES::FlushData(TextureData* texture_data)
{
	//RenderServiceGLES::TextureDataGLES* texture_data_gles = (RenderServiceGLES::TextureDataGLES*)texture_data;

    if (!texture_data->data)
    {
        ready = true;        
        return;
    }
    
	//if (texture_data_gles->gl_format == RenderServiceGLES::TextureDataGLES::tex_gles_unknown)
	{
        if (texture_data->bytesPerPixel == 4)
        {
            format = GL_RGBA;
        }
        else
        {
            format = GL_RGB;
        }
		
        glActiveTexture(GL_TEXTURE0);
        
        glBindTexture(target, texture);
        
        if (lods == 1)
        {
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		
        }
        
		//CreateGLTexture();
	
		//TextureAddress adress = adressUVW;
		//adressUVW = addressDword;

		SetAdress( addressClamp );

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture_data->data);

		SetFilters(magminf, mipmapf);
		SetAdress(adressUVW);
	}
	    
    ready = true;    
}

bool TextureGLES::UpdateLod(int face, int lod, int rect_x, int rect_y, int rect_w, int rect_h, byte* data, int size)
{	
	glBindTexture(target, texture);	

	//load a compressed texture
	//GL_COMPRESSED_RGB_S3TC_DXT1_EXT

	{
        GLenum formatEnum = GL_RGBA;
		
		if (formatEnum==GL_RGB || formatEnum==GL_LUMINANCE) glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		
		int w = width;
		int h = height;

		for (uint32_t i = 0; i <= lod; i++)
		{
			//uint32_t size = getImageSize(w,h,dds.components,dds.format);
			//printf("non compressed texture %d w %d h%d format %d size %d 0x%x\n",i,w,h,dds.format,size,dds.image[0].pixels[i]);
			if (i==lod)
			{
				//glTexImage2D(GL_TEXTURE_2D,i,4,w,h,0,formatEnum,GL_UNSIGNED_BYTE,data);

				glTexSubImage2D( GL_TEXTURE_2D, lod, rect_x, rect_y, rect_w, rect_h, format, GL_UNSIGNED_BYTE, data);                

				w/=2; h/=2;

				break;
			}
			
			if (w == 0) w = 1;
			if (h == 0) h = 1;
		} 
      
		if (formatEnum==GL_RGB || formatEnum==GL_LUMINANCE) glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	}

	//glBindTexture(target, 0);

	return true;
}

bool TextureGLES::Lock(int face, int lod, Texture::LockRect& rect)
{
	return false;
}

bool TextureGLES::Unlock(int face, int lod)
{
	return false;
}

void* TextureGLES::GetData()
{		
	return &texture;
}

void TextureGLES::SetFilters(Texture::FilterType magmin, Texture::FilterType mipmap)
{
	magminf = magmin;

	if (lods != 1)
	{
		mipmapf = mipmap;
	}

    glBindTexture( target, texture);

	if (lods > 1)
	{
		if(magminf == filterPoint && mipmapf == filterPoint)
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); 			
		}
		else
		if(magminf == filterLinear && mipmapf == filterPoint)
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 
			//glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST); 
		}
		else
		if(magminf == filterPoint && mipmapf == filterLinear)
		{
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			//glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		}
		else
		if(magminf == filterLinear && mipmapf == filterLinear)
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

void TextureGLES::SetAdress(Texture::TextureAddress adress)
{	
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
}

void TextureGLES::Reset()
{
    if (tex_created)
    {
        if (!en_core.Render()->ResoursesAreDead())
        {
            glDeleteTextures(1,&texture);
        }
        
        tex_created = false;
    }    
}

bool TextureGLES::IsReady()
{
    return ready;
}

void TextureGLES::Release()
{	
	if (width > 0)
	{
		RenderService::texture_count--;	
		RenderService::texture_memory -= width * height * 4;
	}

	en_core.Render()->MarkAsSkipped(this);	

	if (!en_core.Render()->ResoursesAreDead())
	{
		if (tex_created)
		{
			glDeleteTextures(1,&texture);			
		}		
	}

	delete this;
}