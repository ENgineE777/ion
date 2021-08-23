
#include "TextureD3D.h"
#include "Core/Core.h"

TextureD3D::TextureD3D(int w, int h, int f, int l, IDirect3DTexture9* tex) : Texture(w, h, f, l, tex2d)
{
	texture = tex;	

	RenderService::texture_count++;
	RenderService::texture_memory += width * height * 4;

	dummy_data = null;
	if (RenderServiceD3D::instance->IsUseDummyRender())
	{
		dummy_data = (byte*)malloc(width * height * 4);		
	}	

	ready = true;
}

bool TextureD3D::Load(const char* name, bool async)
{	
	if (async)
	{
		TextureData* texture_data = RenderServiceD3D::instance->AddTextureToQuene(this, name);	

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

	RenderService::TextureData tex_data;	
	tex_data.texture = this;
	String::Copy(tex_data.fl_name, 512, name);

	if (tex_data.GetInfo())
	{
		if (tex_data.Load())
		{
			RenderService::texture_count++;
			RenderService::texture_memory += width * height * 4;

			FlushData(&tex_data);
			free(tex_data.data);

			return true;
		}
	}

	ready = true;	

	return false;
}

void TextureD3D::FlushData(TextureData* texture_data)
{		
	if (!texture_data->data)
	{
		ready = true;
		texture_data = null;

		return;
	}	

	if (RenderServiceD3D::instance->IsUseDummyRender()) return;	

	Texture::LockRect rect;

	Lock(0, 0, rect);
	
	for(int j=0; j < height;j++)
	{
		byte* rows = (byte*)&rect.ptr[j*rect.pitch];
				
		for(int i=0; i < width;i++)
		{									
			int index1 = i*4;
			int index2 = (i + j * width) * texture_data->bytesPerPixel;

			rows[index1 + 2] = texture_data->data[index2 + 0];			
			rows[index1 + 1] = texture_data->data[index2 + 1];			
			rows[index1 + 0] = texture_data->data[index2 + 2];			

			if (texture_data->bytesPerPixel == 4)
			{
				rows[index1 + 3] = texture_data->data[index2 + 3];			
			}
			else
			{
				rows[index1 + 3] = 255;
			}
		}
	}

	Unlock(0, 0);

	ready = true;

	texture_data = null;
}

void TextureD3D::ApplyStates(int slot)
{
	en_core.Render()->State()->SetSamplerState( slot, RenderService::SamplerMagFilter, magminf);
	en_core.Render()->State()->SetSamplerState( slot, RenderService::SamplerMinFilter, magminf);
	en_core.Render()->State()->SetSamplerState( slot, RenderService::SamplerMipFilter, mipmapf);

	en_core.Render()->State()->SetSamplerState( slot, RenderService::SamplerAddressU, adressUVW);
	en_core.Render()->State()->SetSamplerState( slot, RenderService::SamplerAddressV, adressUVW);	

	RenderServiceD3D::instance->SetTexture(slot, this);
}

bool TextureD3D::Lock(int face, int lod, Texture::LockRect& rect)
{
	if (RenderServiceD3D::instance->IsUseDummyRender())
	{
		rect.ptr = dummy_data;
		rect.pitch = width * 4;
		rect.slicePitch = 0;

		return true;
	}

	D3DLOCKED_RECT d3d_rect;

	if (texture->LockRect(lod, &d3d_rect, NULL, 0) == S_OK)
	{
		rect.ptr = (byte*)d3d_rect.pBits;
		rect.pitch = d3d_rect.Pitch;
		rect.slicePitch = 0;

		return true;		
	}

	return false;
}

bool TextureD3D::Unlock(int face, int lod)
{
	if (RenderServiceD3D::instance->IsUseDummyRender()) return true;

	return (texture->UnlockRect(lod) == S_OK);
}

bool TextureD3D::UpdateLod(int face, int lod, int rect_x, int rect_y, int rect_w, int rect_h, byte* data, int size)
{	
	return false;
}

void* TextureD3D::GetData()
{
	return texture;
}

bool TextureD3D::IsReady()
{
	return ready;
}

void TextureD3D::Release()
{
	if (width > 0)
	{
		RenderService::texture_count--;	
		RenderService::texture_memory -= width * height * 4;
	}

	en_core.Render()->MarkAsSkipped(this);	

	Texture::Release();

	if (dummy_data)
	{
		free(dummy_data);
	}
		
	RELEASE(texture)	
	
	delete this;
}