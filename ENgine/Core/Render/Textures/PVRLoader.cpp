
#include "Core/Core.h"

enum
{
	PVRTextureFlagTypePVRTC_2 = 24,	
	PVRTextureFlagTypePVRTC_4	
};

struct PVRTexHeader
{	
	uint32_t headerLength;	
	uint32_t height;	
	uint32_t width;	
	uint32_t numMipmaps;	
	uint32_t flags;	
	uint32_t dataLength;	
	uint32_t bpp;	
	uint32_t bitmaskRed;	
	uint32_t bitmaskGreen;	
	uint32_t bitmaskBlue;
	uint32_t bitmaskAlpha;	
	uint32_t pvrTag;	
	uint32_t numSurfs;	
};

enum PVRFormat
{
	TextureFormatPvrtcRgba2,
	TextureFormatPvrtcRgb2,
	TextureFormatPvrtcRgba4,
	TextureFormatPvrtcRgb4
};

class RVRLoader
{
public:
	    	
    void LoadPvrImage(const char* name)
    {
        buffer = core.Files()->LoadFile(name);
        
		if (!buffer) return;
		
        PVRTexHeader* header = (PVRTexHeader*) buffer->GetBuffer();
        bool hasAlpha = header->bitmaskAlpha;
		        
		switch (header->flags & 0xff)
		{
            case PVRTextureFlagTypePVRTC_2:    
                format = hasAlpha ? TextureFormatPvrtcRgba2 : TextureFormatPvrtcRgb2;
                break;
            case PVRTextureFlagTypePVRTC_4:
                format = hasAlpha ? TextureFormatPvrtcRgba4 : TextureFormatPvrtcRgb4;
                break;
            default:
                core.TraceTo("Render", "Unsupported PVR image.");
				buffer->Release();
				buffer = null;
				return;
				
                break;
        }
        
        size.x = header->width;
        size.y = header->height;
        mips = header->numMipmaps;
    }
	
    void* GetImageData()
    {
        if (!buffer) return null;
		
        PVRTexHeader* header = (PVRTexHeader*) buffer->GetBuffer();
        char* data = (char*) buffer->GetBuffer();
        unsigned int headerSize = header->headerLength;
        return data + headerSize;
    }

    void UnloadImage()
    {
        if (buffer) buffer->Release();
    }
	
    bool m_hasPvrHeader;
    PVRFormat format;
	Vector2 size;
	int mips;
	FileBuffer* buffer;
};			

Texture* LoadPvrTexture(const char* name, const char* file, int line)
{
	RVRLoader rs;
	
	rs.LoadPvrImage(name);
	
    unsigned char* data = (unsigned char*) rs.GetImageData();
    
	int width = rs.size.x;
    int height = rs.size.y;
    
    int bitsPerPixel;
    GLenum format;
    
	switch (rs.format)
	{
        case TextureFormatPvrtcRgba2:
            bitsPerPixel = 2;
            format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
            break;
        case TextureFormatPvrtcRgb2:
            bitsPerPixel = 2;
            format = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
            break;
        case TextureFormatPvrtcRgba4:
            bitsPerPixel = 4;
            format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
            break;
        case TextureFormatPvrtcRgb4:
            bitsPerPixel = 4;
            format = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
            break;
    }
    
	rs.mips = 1;
	
	if (width != height)
	{
        core.TraceTo("Render", "texture %s are not squared", name);
		rs.UnloadImage();
	
		return null;
	}

	Texture* texture = core.Render()->CreateTexture(width,height,rs.mips,format, file, line);	
	
    for (int level = 0; width > 0 && height > 0; ++level)
	{
        GLsizei size = MathMax(32, width * height * bitsPerPixel / 8);
		
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, 
                               height, 0, size, data);
        data += size;
		
		break;
		
        width >>= 1; height >>= 1;
    }
    
    rs.UnloadImage();
	
	return texture;
}	  
