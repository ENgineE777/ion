
#include "TextureD3D11.h"
#include "Core/Core.h"

#include "d3d11.h"

TextureD3D11::TextureD3D11(int w, int h, int f, int l, ID3D11Texture2D* tex) : Texture(w, h, f, l, tex2d)
{
	texture = tex;
	type = tex2d;	

	RenderService::texture_count++;
	RenderService::texture_memory += width * height * 4;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	srvDesc.Format = (DXGI_FORMAT)f;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	sampler = null;
	sampler_need_recrete = true;

	RenderServiceD3D11::instance->pd3dDevice->CreateShaderResourceView( texture, &srvDesc, &srview );

	dummy_data = null;

	if (RenderServiceD3D11::instance->IsUseDummyRender())
	{
		dummy_data = (byte*)malloc(width * height * 4);		
	}	

	ready = true;
}

bool TextureD3D11::Load(const char* name, bool async)
{
	if (async)
	{
		TextureData* texture_data = RenderServiceD3D11::instance->AddTextureToQuene(this, name);	

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
			width = tex_data.width;
			height = tex_data.height;

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

void TextureD3D11::FlushData(TextureData* texture_data)
{		
	if (!texture_data->data)
	{
		ready = true;
		return;
	}	

	if (RenderServiceD3D11::instance->IsUseDummyRender()) return;	

	Texture::LockRect rect;

	Lock(0, 0, rect);
	
	for(int j=0; j < height;j++)
	{
		byte* rows = (byte*)&rect.ptr[j*rect.pitch];
				
		for(int i=0; i < width;i++)
		{									
			int index1 = i*4;
			int index2 = (i + j * width) * texture_data->bytesPerPixel;

			rows[index1 + 0] = texture_data->data[index2 + 0];			
			rows[index1 + 1] = texture_data->data[index2 + 1];			
			rows[index1 + 2] = texture_data->data[index2 + 2];			

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
}

void TextureD3D11::SetFilters(FilterType magmin, FilterType mipmap)
{
	Texture::SetFilters(magmin, mipmap);

	sampler_need_recrete = true;
}

void TextureD3D11::SetAdress(TextureAddress adress)
{
	Texture::SetAdress(adress);

	sampler_need_recrete = true;
}

void TextureD3D11::ApplyStates(int slot)
{
	if (sampler_need_recrete)
	{
		RELEASE(sampler)						

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory( &sampDesc, sizeof(sampDesc) );

		D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

		if (magminf == filterPoint && mipmapf == filterPoint)
		{
			filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
		else
		if (magminf == filterPoint && mipmapf == filterLinear)
		{
			filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;			
		}
		else
		if (magminf == filterLinear && mipmapf == filterPoint)
		{
			filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		}
		else
		if (magminf == filterLinear && mipmapf == filterLinear)
		{
			filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}		

		sampDesc.Filter = filter;
		sampDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)adressUVW;
		sampDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)adressUVW;
		sampDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)adressUVW;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		RenderServiceD3D11::instance->pd3dDevice->CreateSamplerState( &sampDesc, &sampler );

		sampler_need_recrete = false;
	}

	RenderServiceD3D11::instance->immediateContext->PSSetSamplers( slot, 1, &sampler );	
}

bool TextureD3D11::Lock(int face, int lod, Texture::LockRect& rect)
{
	if (RenderServiceD3D11::instance->IsUseDummyRender())
	{
		rect.ptr = dummy_data;
		rect.pitch = width * 4;
		rect.slicePitch = 0;

		return true;
	}

	D3D11_MAPPED_SUBRESOURCE res;	

	if (RenderServiceD3D11::instance->immediateContext->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &res) == S_OK)
	{
		rect.ptr = (byte*)res.pData;
		rect.pitch = res.RowPitch;
		rect.slicePitch = 0;
	}

	return false;
}

bool TextureD3D11::Unlock(int face, int lod)
{
	if (RenderServiceD3D11::instance->IsUseDummyRender()) return true;

	RenderServiceD3D11::instance->immediateContext->Unmap(texture, 0);

	return true;
}

void* TextureD3D11::GetData()
{
	return texture;
}

void* TextureD3D11::GetHackData()
{
	return srview;
}

bool TextureD3D11::IsReady()
{
	return ready;
}

void TextureD3D11::Release()
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
		
	RELEASE(sampler)
	RELEASE(texture)
	RELEASE(srview)
	
	delete this;
}