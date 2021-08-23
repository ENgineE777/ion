
#pragma once

#include "Common/Common.h"

class Texture
{	
public:

	class TextureData
	{
	public:

		char fl_name[512];
		int bytesPerPixel;
		int width, height, lods;
		byte* data;
		bool need_skip;

		TextureData()
		{
			fl_name[0] = 0;
			bytesPerPixel = 0;		
			width = height = 0;
			lods = 1;
			data = null;			
			need_skip = false;
		};

		virtual bool Load() = 0;
	};

	enum Format
	{
		fmt_a8r8g8b8 = 0,
		fmt_a8_r8 = 1,
		fmt_dwrod = 0x7fffffff
	};

#ifdef PC

	enum FilterType
	{		
		filterPoint = 1,
		filterLinear = 2,
		filterAnisotropic = 3,
		filterDword = 0x7fffffff
	};

	enum TextureAddress
	{
		addressWrap = 1,
		addressMirror = 2,
		addressClamp = 3,
		addressBorder = 4,
		addressMirrorOnce = 5,		
		addressDword = 0x7fffffff
	};	

#endif

#ifdef OPGL
	enum FilterType
	{		
		filterPoint = 1,
		filterLinear = 2,
		filterAnisotropic = 3,
		filterDword = 0x7fffffff
	};

	enum TextureAddress
	{
		addressWrap = 0,
		addressMirror = 1,
		addressClamp = 2,
		addressMirrorOnce = 3,
		addressBorderHalf = 4,
		addressMirrorBorderHalf = 5,
		addressBorder = 6,
		addressMirrorOnceBorder = 7,
		addressDword = 0x7fffffff
	};
#endif

	enum Type
	{
		tex2d      = 0,
		cubtex     = 1,		
		volumetex  = 2,
		forceDword = 0x7fffffff
	};

	struct LockRect
	{
		byte* ptr;
		int   pitch;
		int   slicePitch;
	};	

protected:

	int width;
	int height;	
	int format;
	int lods;

	FilterType magminf;
	FilterType mipmapf;
	TextureAddress adressUVW;
	Type type;	

public:			

	Texture(int w, int h, int f, int l, Type tp)
	{
		width = w;
		height = h;
		format = f;
		lods = l;
		type = tp;

		magminf = filterLinear;

		if (lods == 1)
		{
			mipmapf = filterPoint;
		}
		else
		{
			mipmapf = filterLinear;
		}
		adressUVW = addressWrap;
	};

	virtual ~Texture() {};

	virtual bool Load(const char* name, bool async) { return true; };
	virtual void FlushData(TextureData* texture_data) {};

	virtual int GetWidth() { return width; };
	virtual int GetHeight() { return height; };		
	virtual int GetFormat() { return format; };
	virtual int GetLods() { return lods; };

	virtual void SetFilters(FilterType magmin, FilterType mipmap)
	{
		magminf = magmin;
		mipmapf = mipmap;
	};

	virtual void SetAdress(TextureAddress adress)
	{
		adressUVW = adress;
	};

	virtual FilterType GetMagMin() { return magminf; };
	virtual FilterType GetMipMap() { return mipmapf; };
	virtual TextureAddress GetAdress() { return adressUVW; };

	virtual void* GetData() = 0;
	virtual void* GetHackData() { return null; };
	virtual bool IsReady() { return true; };

	virtual void ApplyStates(int slot) {};

	virtual bool Lock(int face, int lod, LockRect& rect) { return true; };	
	virtual bool Unlock(int face, int lod) { return true; };	
	virtual bool UpdateLod(int face, int lod, int rect_x, int rect_y, int rect_w, int rect_h, byte* data, int size) { return true; };
	virtual Type GetType() { return type; };	

	virtual void Release() {};
};
