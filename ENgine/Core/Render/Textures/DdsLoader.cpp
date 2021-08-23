//--------------------------------------------------------------------------------------
// biEngine technology
//
// CONTENT IS PROHIBITED FROM USAGE. BIART COMPANY CONFIDENTIAL.
//
// Copyright (C) Biart Company LLC. 2011. All rights reserved.
//--------------------------------------------------------------------------------------

#include "Core/Core.h"
#include "Core/Render/Textures/Texture.h"

#ifdef ANDROID

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif

// compressed texture types
#define FOURCC_DXT1				0x31545844
#define FOURCC_DXT3				0x33545844
#define FOURCC_DXT5				0x35545844

// surface description flags
#define 	DDSF_MAX_MIPMAPS	16
#define 	DDSF_MAX_TEXTURES	16		

#define		DDSF_CAPS           0x00000001
#define 	DDSF_HEIGHT         0x00000002
#define 	DDSF_WIDTH          0x00000004
#define 	DDSF_PITCH          0x00000008
#define 	DDSF_PIXELFORMAT    0x00001000
#define 	DDSF_MIPMAPCOUNT    0x00020000
#define 	DDSF_LINEARSIZE     0x00080000
#define 	DDSF_DEPTH			0x00800000

// pixel format flags
#define		DDSF_ALPHAPIXELS	0x00000001
#define		DDSF_ALPHA			0x00000002
#define		DDSF_INDEXED        0x00000020 
#define		DDSF_FOURCC			0x00000004
#define		DDSF_RGB			0x00000040
#define		DDSF_RGBA			0x00000041
#define		DDSF_BUMPDUDV		0x00080000
#define		DDSF_BUMPLUMINANCE	0x00020000

// dwCaps1 flags
#define DDSF_COMPLEX			0x00000008
#define DDSF_TEXTURE			0x00001000
#define DDSF_MIPMAP				0x00400000

// dwCaps2 flags
#define DDSF_CUBEMAP			0x00000200l
#define DDSF_CUBEMAP_POSITIVEX  0x00000400l
#define DDSF_CUBEMAP_NEGATIVEX  0x00000800l
#define DDSF_CUBEMAP_POSITIVEY  0x00001000l
#define DDSF_CUBEMAP_NEGATIVEY  0x00002000l
#define DDSF_CUBEMAP_POSITIVEZ  0x00004000l
#define DDSF_CUBEMAP_NEGATIVEZ  0x00008000l
#define DDSF_CUBEMAP_ALL_FACES  0x0000FC00l
#define DDSF_VOLUME				0x00200000l

struct DXTColBlock
{
	unsigned short col0;
    unsigned short col1;
    unsigned char row[4];
};

struct DXT3AlphaBlock
{
	unsigned short row[4];
};

struct DXT5AlphaBlock
{
	unsigned char alpha0;
    unsigned char alpha1;
    unsigned char row[6];
};

struct DDS_PIXELFORMAT
{
	uint dwSize;
    uint dwFlags;
    uint dwFourCC;
    uint dwRGBBitCount;
    uint dwRBitMask;
    uint dwGBitMask;
    uint dwBBitMask;
    uint dwABitMask;
};

struct DDS_HEADER
{
	uint dwSize;
	uint dwFlags;
	uint dwHeight;
	uint dwWidth;
	uint dwPitchOrLinearSize;
	uint dwDepth;
	uint dwMipMapCount;
	uint dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	uint dwCaps1;
	uint dwCaps2;
	uint dwReserved2[3];
};

typedef enum TextureType
{
	TextureNone,
    TextureFlat,    // 1D, 2D, and rectangle textures
    Texture3D,
    TextureCubemap
}TextureType;

typedef struct _DDS_IMAGE
{
	unsigned char *pixels[DDSF_MAX_MIPMAPS];		//the mip map images 
}_DDS_IMAGE;


typedef struct _DDS_TEXTURE
{
	unsigned char	*buffer;	//pointer to loaded dds file
	uint			format;		//compression used or pixel format
	uint			components; //number of channels 
	uint			width;		//width of base image in pixels
	uint			height;		//height of based image in pixels
	uint			mips;		//number of mip levels
	uint			surfaces;	//number of surfaces ( 1 = a texture 6 = a cube map)

	_DDS_IMAGE image[6];
}DDS_TEXTURE;

void swapEndian(void *val)
{
	unsigned int *ival = (unsigned int *)val;
    *ival = ((*ival >> 24) & 0x000000ff) |
            ((*ival >>  8) & 0x0000ff00) |
            ((*ival <<  8) & 0x00ff0000) |
            ((*ival << 24) & 0xff000000);
}

#if defined(PC) || defined(_XBOX)

#define PF_IS_DXT1(pf) \
  ((pf.dwFlags & DDSF_FOURCC) && \
   (pf.dwFourCC == '1TXD'))

#define PF_IS_DXT2(pf) \
  ((pf.dwFlags & DDSF_FOURCC) && \
   (pf.dwFourCC == '2TXD'))

#define PF_IS_DXT3(pf) \
  ((pf.dwFlags & DDSF_FOURCC) && \
   (pf.dwFourCC == '3TXD'))

#define PF_IS_DXT4(pf) \
  ((pf.dwFlags & DDSF_FOURCC) && \
   (pf.dwFourCC == '4TXD'))

#define PF_IS_DXT5(pf) \
  ((pf.dwFlags & DDSF_FOURCC) && \
   (pf.dwFourCC == '5TXD'))

#define PF_IS_BGRA8(pf) \
  ((pf.dwFlags & DDSF_RGB) && \
   (pf.dwFlags & DDSF_ALPHAPIXELS) && \
   (pf.dwRGBBitCount == 32) && \
   (pf.dwRBitMask == 0x00ff0000) && \
   (pf.dwGBitMask == 0x0000ff00) && \
   (pf.dwBBitMask == 0x000000ff) && \
   (pf.dwABitMask == 0xff000000))

#define PF_IS_BGR5A1(pf) \
  ((pf.dwFlags & DDSF_RGB) && \
   (pf.dwFlags & DDSF_ALPHAPIXELS) && \
   (pf.dwRGBBitCount == 16) && \
   (pf.dwRBitMask == 0x00007c00) && \
   (pf.dwGBitMask == 0x000003e0) && \
   (pf.dwBBitMask == 0x0000001f) && \
   (pf.dwABitMask == 0x00008000))

#define PF_IS_BGR565(pf) \
  ((pf.dwFlags & DDSF_RGB) && \
  !(pf.dwFlags & DDSF_ALPHAPIXELS) && \
   (pf.dwRGBBitCount == 16) && \
   (pf.dwRBitMask == 0x0000f800) && \
   (pf.dwGBitMask == 0x000007e0) && \
   (pf.dwBBitMask == 0x0000001f))

#define PF_IS_UV88(pf) \
   ((pf.dwFlags & DDSF_BUMPDUDV) && \
   (pf.dwRGBBitCount == 16) && \
   (pf.dwRBitMask == 0x000000ff) && \
   (pf.dwGBitMask == 0x0000ff00) && \
   (pf.dwBBitMask == 0x00000000))

#define PF_IS_UV1616(pf) \
   ((pf.dwFlags & DDSF_BUMPDUDV) && \
   (pf.dwRGBBitCount == 32) && \
   (pf.dwRBitMask == 0x0000ffff) && \
   (pf.dwGBitMask == 0xffff0000) && \
   (pf.dwBBitMask == 0x00000000))

#define PF_IS_A8(pf) \
  ((pf.dwFlags & DDSF_ALPHA) && \
   (pf.dwRGBBitCount == 8) && \
   (pf.dwABitMask == 0x000000ff))

#ifdef PC

static const D3DFORMAT D3DFMT_LIN_DXT1 = D3DFMT_DXT1;
static const D3DFORMAT D3DFMT_LIN_DXT2 = D3DFMT_DXT2;
static const D3DFORMAT D3DFMT_LIN_DXT3 = D3DFMT_DXT3;
static const D3DFORMAT D3DFMT_LIN_DXT4 = D3DFMT_DXT4;
static const D3DFORMAT D3DFMT_LIN_DXT5 = D3DFMT_DXT5;
static const D3DFORMAT D3DFMT_LIN_A8R8G8B8 = D3DFMT_A8R8G8B8;
static const D3DFORMAT D3DFMT_LIN_A1R5G5B5 = D3DFMT_A1R5G5B5;
static const D3DFORMAT D3DFMT_LIN_A8 = D3DFMT_A8;
static const D3DFORMAT D3DFMT_LIN_L8 = D3DFMT_L8;
static const D3DFORMAT D3DFMT_LIN_G16R16 = D3DFMT_G16R16;
static const D3DFORMAT D3DFMT_LIN_R5G6B5 = D3DFMT_R5G6B5;
static const D3DFORMAT D3DFMT_LIN_V16U16 = D3DFMT_V16U16;
static const D3DFORMAT D3DFMT_LIN_V8U8 = D3DFMT_V8U8;

#endif

struct SDDSChecker
{
	UINT divSize;
	UINT blockBytes;
	D3DFORMAT format;
	SDDSChecker()
	{
		divSize = 1;
		blockBytes = 4;
		format = D3DFMT_LIN_A8R8G8B8;
	}

	bool check(DDS_HEADER* Header)
	{
		if (PF_IS_DXT1(Header->ddspf)) 
		{
			divSize = 4;
			blockBytes = 8;
			format = (D3DFORMAT)D3DFMT_LIN_DXT1;
		}

		else 
		if (PF_IS_DXT2(Header->ddspf)) 
		{
			divSize = 4;
			blockBytes = 16;
			format = (D3DFORMAT)D3DFMT_LIN_DXT2;
		}

		else 
		if (PF_IS_DXT3(Header->ddspf)) 
		{
			divSize = 4;
			blockBytes = 16;
			format = (D3DFORMAT)D3DFMT_LIN_DXT3;
		}

		else 
		if (PF_IS_DXT4(Header->ddspf)) 
		{
			divSize = 4;
			blockBytes = 16;
			format = (D3DFORMAT)D3DFMT_LIN_DXT4;
		}

		else 
		if (PF_IS_DXT5(Header->ddspf))
		{
			divSize = 4;
			blockBytes = 16;
			format = (D3DFORMAT)D3DFMT_LIN_DXT5;
		}

		else 
		if (PF_IS_BGRA8(Header->ddspf))
		{
			divSize = 1;
			blockBytes = 4;
			format = D3DFMT_LIN_A8R8G8B8;
		}

		else 
		if (PF_IS_BGR5A1(Header->ddspf)) 
		{
			divSize = 1;
			blockBytes = 2;
			format = D3DFMT_LIN_A1R5G5B5;
		}

		else 
		if (PF_IS_BGR565(Header->ddspf)) 
		{
			divSize = 1;
			blockBytes = 2;
			format = D3DFMT_LIN_R5G6B5;
		}

		else
		if (PF_IS_UV88(Header->ddspf)) 
		{
			divSize = 1;
			blockBytes = 2;
			format = D3DFMT_LIN_V8U8;
		}

		else
		if (PF_IS_UV1616(Header->ddspf)) 
		{
			divSize = 1;
			blockBytes = 4;
			format = D3DFMT_LIN_V16U16;
		}

		else 
		if (PF_IS_A8(Header->ddspf))
		{
			divSize = 1;
			blockBytes = 1;
			format = D3DFMT_LIN_A8;
		}
		else
		{
			return false;
		}

		return true;
	}
};
#endif

#if defined(ANDROID)

uint getImageSize(uint w,  uint h, uint components, uint format)
{
	switch(format)
	{
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			return ((w+3)/4)* ((h+3)/4)* 8;   
	/*	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			return ((w+3)/4)*((h+3)/4)* 16;   
		case GL_BGRA:
		case GL_BGR:*/
		case GL_LUMINANCE:
			return w*h*components;         
		default:
			printf("ERROR: unable to determine image size\n");
			exit(1);
	}
}

bool ImageSpec(DDS_HEADER *ddsh, int *format ,uint *components)
{		
	if (ddsh->ddspf.dwFlags & DDSF_FOURCC)	
	{
		switch(ddsh->ddspf.dwFourCC)
		{
			case FOURCC_DXT1:				
				*format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				*components = 3;
            break;
			/*case FOURCC_DXT3:
				*format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				*components = 4;
            break;
			case FOURCC_DXT5:
				*format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				*components = 4;
            break;*/
			default:
				printf("ERROR: Uses a compressed texture of unsupported type\n");
				return false;
		}
    }
	else
	if (ddsh->ddspf.dwFlags == DDSF_RGBA && ddsh->ddspf.dwRGBBitCount == 32)
	{
	
		*format = GL_RGBA; 
		*components = 4;
    }
	else
	if (ddsh->ddspf.dwFlags == DDSF_RGB  && ddsh->ddspf.dwRGBBitCount == 32)
	{		
		*format = GL_RGBA; 
		*components = 4;
    }
	else
	if (ddsh->ddspf.dwFlags == DDSF_RGB  && ddsh->ddspf.dwRGBBitCount == 24)
    {	
	    *format = GL_RGB; 
	    *components = 3;
    }
	else
	if (ddsh->ddspf.dwRGBBitCount == 8)
	{		
		*format = GL_LUMINANCE; 
		*components = 1;
	}
	else 
    {
		core.TraceTo("Render", "ERROR: Uses a texture of unsupported type");
		return false;
    }

	return true;
}
#endif

Texture* LoadDDSTexture(const char *filename, const char* file, int line)
{			
	DDS_TEXTURE dds;			

	FileBuffer* file_buffer = core.Files()->LoadFile(filename);

	if (!file_buffer)
	{				
		return null;
	}
	
	byte *buffer = file_buffer->GetBuffer();		

	unsigned char *buff;
	buff = buffer;
	
#ifdef _XBOX
	if(strncmp((char*)buff," SDD",4)!=0)
#else
	if(strncmp((char*)buff,"DDS ",4)!=0)
#endif
	{
		core.TraceTo("Render","ERROR: %s is not a dds file",filename);
		return false;
	}
	buff+=4;
	

	DDS_HEADER *ddsh;

	//memcpy(&ddsh,buffer+4,sizeof(DDS_HEADER));
	ddsh=(DDS_HEADER*)buff;
	buff+=sizeof(DDS_HEADER);

#ifdef ANDROID
	/*uint* t = (uint*)ddsh;
	for(unsigned int ddsCounterHeader=0; ddsCounterHeader<sizeof(DDS_HEADER)/4; ddsCounterHeader++)
	{
		swapEndian(t+ddsCounterHeader);
	}*/
#endif

#if defined(PC) || defined(_XBOX)
	SDDSChecker checker;

	if (!checker.check(ddsh))
	{
		core.Trace("%s: unsupported texture format, %s",filename);		
		return null;
	}
#endif

	TextureType type = TextureFlat;
	if (ddsh->dwCaps2 & DDSF_CUBEMAP) type = TextureCubemap;

	// check if image is a volume texture
	if ((ddsh->dwCaps2 & DDSF_VOLUME) && (ddsh->dwDepth > 0))
	{
		type = Texture3D;
	}
	
#ifdef ANDROID
	int format;
	uint components;		
	ImageSpec(ddsh,&format,&components);
#endif

	uint width, height;
	width = ddsh->dwWidth;
	height = ddsh->dwHeight;		

	dds.buffer		= buffer;	
	
#if defined(PC) || defined(_XBOX)
	dds.format		= checker.format;	
#endif

#ifdef ANDROID
	dds.format		= format;	
	dds.components	= components;	
#endif

	dds.height		= height;
	dds.width		= width;

	if(ddsh->dwMipMapCount==0) ddsh->dwMipMapCount++;
	
	dds.mips = ddsh->dwMipMapCount;	

	dds.surfaces = 1;
	if (type == TextureCubemap) dds.surfaces = 6;
			
	if (dds.surfaces!=1) //its a cube map 
	{
#if defined (PC) || defined(_XBOX)
		uint faceMask = 0;
		if (ddsh->dwCaps2 & DDSF_CUBEMAP_POSITIVEX)	faceMask |= 0x1;
		if (ddsh->dwCaps2 & DDSF_CUBEMAP_NEGATIVEX) faceMask |= 0x2;
		if (ddsh->dwCaps2 & DDSF_CUBEMAP_POSITIVEY) faceMask |= 0x4;
		if (ddsh->dwCaps2 & DDSF_CUBEMAP_NEGATIVEY) faceMask |= 0x8;
		if (ddsh->dwCaps2 & DDSF_CUBEMAP_POSITIVEZ) faceMask |= 0x10;
		if (ddsh->dwCaps2 & DDSF_CUBEMAP_NEGATIVEZ) faceMask |= 0x20;

		uint x = ddsh->dwWidth;
		uint y = ddsh->dwWidth;			

		uint bestLod = g_GameProxy.RenderSettings.TextureQuality; 

		if (x<32 || y<32) bestLod = 0;

		bestLod = ((dds.mips - 1) < bestLod) ? ((dds.mips - 1)) : bestLod;

		dds.mips = dds.mips - bestLod;

		uint lodIndex;
		for (lodIndex = 0; lodIndex < bestLod; lodIndex++)
		{
			x = (x > 2) ? x / 2 : 1;
			y = (y > 2) ? y / 2 : 1;
		}

		Texture* texture = core.Render()->CreateCubmapTexture(x,x,dds.mips,dds.format, file, line);			

		bool need_release = core.Render()->AcquireDevice();

		if (!texture)
		{
			core.TraceTo("Render","Can't create texture for %s",filename);			
			file_buffer->Release();	
			return null;			
		}

		for (int faceIndex = 0; faceIndex < 6; faceIndex++)
		{
			if (!(faceMask & (1 << faceIndex))) continue;

			uint width = ddsh->dwWidth;
			uint height = ddsh->dwWidth;

			for (lodIndex = 0; lodIndex < bestLod; lodIndex++)
			{				
#ifdef ANDROID
				buff += getImageSize(width,height,components,format);
#else
				uint w = MathMax(checker.divSize, width) / checker.divSize;
				uint h = MathMax(checker.divSize, height) / checker.divSize;				
				buff += w * h * checker.blockBytes;
#endif				

				width = (width > 2) ? width / 2 : 1;
				height = (height > 2) ? height / 2 : 1;
			}
				
			for (lodIndex = 0; lodIndex < dds.mips; ++lodIndex)
			{				
#ifdef ANDROID
				uint size = getImageSize(width,height,components,format);
#else
				uint Width = MathMax(checker.divSize, width) / checker.divSize;
				uint Height = MathMax(checker.divSize, height) / checker.divSize;
				int size = Width * Height * checker.blockBytes;
#endif

				Texture::LockRect rect;
				if (texture->Lock(faceIndex, lodIndex,rect))
				{
					for (uint rowIndex = 0; rowIndex < Height; rowIndex++)
					{
						memcpy(&rect.ptr[rowIndex * rect.pitch],
							   &buff[rowIndex * Width * checker.blockBytes], 
							   Width * checker.blockBytes);
					}

					texture->Unlock(faceIndex, lodIndex);
				}				

				buff += size;

				width = (width > 2) ? width / 2 : 1;
				height = (height > 2) ? height / 2 : 1;
			}			
		}	

		if (need_release) core.Render()->ReleaseDevice();

		file_buffer->Release();	
		return texture;
#endif
		return null;
	}	

	if (type == Texture3D)
	{
#if defined (PC) || defined(_XBOX)
		uint x = ddsh->dwWidth;
		uint y = ddsh->dwHeight;
		uint z = ddsh->dwDepth;

		uint Offset = 0;

		uint bestLod = g_GameProxy.RenderSettings.TextureQuality; 
		bestLod = ((dds.mips - 1) < bestLod) ? ((dds.mips - 1)) : bestLod;

		dds.mips = dds.mips - bestLod;

		uint  LodIndex;
		
		for (LodIndex = 0; LodIndex < bestLod; ++LodIndex)
		{
			uint Width = MathMax(checker.divSize, x) / checker.divSize;
			uint Height = MathMax(checker.divSize, y) / checker.divSize;
			uint Depth = z;

			Offset += Width * checker.blockBytes * Height * Depth;

			x = (x > 2) ? x / 2 : 1;
			y = (y > 2) ? y / 2 : 1;
			z = (z > 2) ? z / 2 : 1;
		}

		Texture* texture = core.Render()->CreateVolumeTexture(MathMax(checker.divSize, x), 
														      MathMax(checker.divSize, y), 
															  z, 
															  dds.mips, dds.format, file, line);
										
		bool need_release = core.Render()->AcquireDevice();

		for (LodIndex = 0; LodIndex < dds.mips; LodIndex++)
		{
			uint Width = MathMax(checker.divSize, x) / checker.divSize;
			uint Height = MathMax(checker.divSize, y) / checker.divSize;
			uint Depth = z;

			Texture::LockRect rect;
			if (texture->Lock(0, LodIndex,rect))
			{
				char* pDstData = (char*)rect.ptr;
				byte* pSrcData = &buff[Offset];
						
				for (uint DepthIndex = 0; DepthIndex < Depth; DepthIndex++)
				{
					for (uint RowIndex = 0; RowIndex < Height; RowIndex++)
					{
						memcpy(&pDstData[DepthIndex * rect.slicePitch + RowIndex * rect.pitch],
							   &pSrcData[DepthIndex * Width * checker.blockBytes * Height + RowIndex * Width * checker.blockBytes], 
							   Width * checker.blockBytes);
					}
				}

				texture->Unlock(0, LodIndex);
			}
			
			Offset += Width * checker.blockBytes * Height * Depth;

			x = (x > 2) ? x / 2 : 1;
			y = (y > 2) ? y / 2 : 1;
			z = (z > 2) ? z / 2 : 1;
		}

		if (need_release) core.Render()->ReleaseDevice();

		file_buffer->Release();	
		return texture;
#endif
		return null;
	}

	uint w = dds.width;
	uint h = dds.height;	

	int skip = 0;//g_GameProxy.RenderSettings.TextureQuality;

	if (w<32 || h<32) skip = 0;

	if (skip>=dds.mips) skip = dds.mips - 1;	
	
	Texture* texture = null;
	
	for (uint i = 0; i < dds.mips; i++)
	{		
#if defined(PC) || defined(_XBOX)
		UINT Width = __max(checker.divSize, w) / checker.divSize;
		UINT Height = __max(checker.divSize, h) / checker.divSize;		
		uint size = Height * Width * checker.blockBytes;
#endif

#ifdef ANDROID
		uint size = getImageSize(w,h,components,format);
#endif
		
		if (skip<=i)
		{			
			if (!texture) texture = core.Render()->CreateTexture(w,h,dds.mips - i,dds.format, file, line);	

			if (!texture)
			{
				core.TraceTo("Render","Can't create texture for %s",filename);			
				file_buffer->Release();	
				return null;			
			}

#if defined(PC) || defined (_XBOX)

			bool need_release = core.Render()->AcquireDevice();
						
			Texture::LockRect rect;
			if (texture->Lock(0, i - skip,rect))
			{
				for (uint rowIndex = 0; rowIndex < Height; rowIndex++)
				{
					memcpy(&rect.ptr[rowIndex * rect.pitch],
						   &buff[rowIndex * Width * checker.blockBytes], 
						   Width * checker.blockBytes);
				}

				texture->Unlock(0, i - skip);
			}

			if (need_release) core.Render()->ReleaseDevice();
#endif

#ifdef ANDROID
			texture->UpdateLod(0,i - skip,buff, size);
#endif
		}

		buff+=size;

		w/=2; h/=2;
		if (w == 0) w = 1;
		if (h == 0) h = 1;
	}

	file_buffer->Release();	

	return texture;
}

bool LoadDDSTexture(Texture* texture, int& width, int& height, int& lods, int& format)
{			
	DDS_TEXTURE dds;			

	FileBuffer* file_buffer = core.Files()->LoadFile(texture->GetFileName());

	if (!file_buffer)
	{				
		return false;
	}
	
	byte *buffer = file_buffer->GetBuffer();		

	unsigned char *buff;
	buff = buffer;
	
	if(strncmp((char*)buff,"DDS ",4)!=0)
	{
		core.TraceTo("Render","ERROR: %s is not a dds file", texture->GetFileName());
		return false;
	}

	buff+=4;	

	DDS_HEADER *ddsh;
	
	ddsh=(DDS_HEADER*)buff;
	buff+=sizeof(DDS_HEADER);

	TextureType type = TextureFlat;
	if (ddsh->dwCaps2 & DDSF_CUBEMAP) type = TextureCubemap;

	// check if image is a volume texture
	if ((ddsh->dwCaps2 & DDSF_VOLUME) && (ddsh->dwDepth > 0))
	{
		type = Texture3D;
	}
		
	uint components;		
	ImageSpec(ddsh,&format,&components);
	
	width = ddsh->dwWidth;
	height = ddsh->dwHeight;		
	lods =  ddsh->dwMipMapCount;	

	dds.buffer		= buffer;		
	dds.format		= format;	
	dds.components	= components;
	dds.height		= height;
	dds.width		= width;	

	if(ddsh->dwMipMapCount==0) ddsh->dwMipMapCount++;
		
	dds.mips = ddsh->dwMipMapCount;		

	dds.surfaces = 1;
	if (type == TextureCubemap) dds.surfaces = 6;
				
	uint w = dds.width;
	uint h = dds.height;	

	int skip = 0;

	if (w<32 || h<32) skip = 0;

	if (skip>=dds.mips) skip = dds.mips - 1;	
	
	for (uint i = 0; i < dds.mips; i++)
	{		
		uint size = getImageSize(w,h,components,format);
		
		if (skip<=i)
		{						
			texture->UpdateLod(0,i - skip,buff, size);
		}

		buff+=size;

		w/=2; h/=2;
		if (w == 0) w = 1;
		if (h == 0) h = 1;
	}

	file_buffer->Release();	

	return true;
}