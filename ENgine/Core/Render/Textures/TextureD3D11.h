
#pragma once

#include "Texture.h"

class TextureD3D11 : public Texture
{			
	friend class PixelShaderD3D11; 

	class ID3D11SamplerState* sampler;	
	bool sampler_need_recrete;

	class ID3D11Texture2D* texture;
	class ID3D11ShaderResourceView* srview;
	byte* dummy_data;
	bool ready;

public:
	
	TextureD3D11(int w, int h, int f, int l, class ID3D11Texture2D* tex);
		
	virtual bool Load(const char* name, bool async);	
	virtual void FlushData(TextureData* texture_data);

	virtual void SetFilters(FilterType magmin, FilterType mipmap);
	virtual void SetAdress(TextureAddress adress);
	virtual void ApplyStates(int slot);	

	virtual bool Lock(int face, int lod, Texture::LockRect& rect);	
	virtual bool Unlock(int face, int lod);		
	virtual void* GetData();
	virtual void* GetHackData();
	virtual bool IsReady();
	virtual void Release();	
};
