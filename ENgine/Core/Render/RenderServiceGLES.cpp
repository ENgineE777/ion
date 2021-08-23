
#include "Core/Core.h"

#include "Geometry/VertexBufferGLES.h"
#include "Geometry/IndexBufferGLES.h"
#include "Shaders/VertexShaderGLES.h"
#include "Shaders/PixelShaderGLES.h"
#include "Textures/TextureGLES.h"
#include "Textures/VideoTextureGLES.h"
 
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

RenderServiceGLES::StateManagerGLES::StateManagerGLES()
{	
}

void RenderServiceGLES::StateManagerGLES::SetAlphaTest(dword enable)
{
	if (enable>0)
	{	
		//glEnable(GL_ALPHA_TEST);
	}
	else
	{
		//glDisable(GL_ALPHA_TEST);
	}
}

void RenderServiceGLES::StateManagerGLES::SetAlphaTestFunc(CompareFunc func, dword ref)
{
	//glAlphaFunc(GetCompareFunc(func), (float)((byte)ref)/255.0f);
}

void RenderServiceGLES::StateManagerGLES::SetAlphaBlend(dword enable)
{
	if (enable>0)
	{	
		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);	
	}	
}

void RenderServiceGLES::StateManagerGLES::SetBlendFunc(BlendArg src, BlendArg dest)
{
	glBlendFunc(GetBlendArg(src), GetBlendArg(dest));	
}

void RenderServiceGLES::StateManagerGLES::SetBlendOperation(BlendOp op)
{
	glBlendEquation(GetBlendOp(op));	
}
	
void RenderServiceGLES::StateManagerGLES::SetDepthTest(dword enable)
{
	if (enable>0)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}

void RenderServiceGLES::StateManagerGLES::SetDepthWriting(dword enable)
{
	if (enable>0)
	{	
		glDepthMask(1);
	}
	else
	{
		glDepthMask(0);
	}
}

void RenderServiceGLES::StateManagerGLES::SetDepthFunc(CompareFunc func)
{
	glDepthFunc(GetCompareFunc(func));	
}

void RenderServiceGLES::StateManagerGLES::SetCulling(CullMode mode)
{
	if (mode == cullNone)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);

		if (mode == cullCW)
		{
			glFrontFace( GL_CW );
		}
		else
		{
			glFrontFace( GL_CCW );
		}		 
	}	  	
}

void RenderServiceGLES::StateManagerGLES::SetStencilTest(dword enable)
{
	if (enable>0)
	{
		glEnable(GL_STENCIL_TEST);
	}
	else
	{
		glDisable(GL_STENCIL_TEST);
	}
}

void RenderServiceGLES::StateManagerGLES::SetStencilTestFunc(CompareFunc func, dword ref, dword mask)
{
	glStencilFunc(GetCompareFunc(func), ref, mask);
}

void RenderServiceGLES::StateManagerGLES::SetStencilWriteMask(dword mask)
{
	glStencilMask(mask);
}

void RenderServiceGLES::StateManagerGLES::SetStencilOperation(StencilOp failOp, StencilOp zFailOp, StencilOp zPass)
{
	glStencilOp(GetStencilOp(failOp), GetStencilOp(zFailOp), GetStencilOp(zPass));
}

void RenderServiceGLES::StateManagerGLES::SetColorWrite(bool red, bool green, bool blue, bool alpha)
{
	glColorMask(red, green, blue, alpha);
}

void RenderServiceGLES::StateManagerGLES::SetClipPlane(dword clipIndex, dword enable)
{
	if (enable>0)
	{	
		//glEnable(GL_CLIP_PLANE0 + clipIndex);	
	}
	else
	{
		//glDisable(GL_CLIP_PLANE0 + clipIndex);
	}
}

void RenderServiceGLES::StateManagerGLES::SetWireFrameMode(dword enable)
{
}
	
void RenderServiceGLES::StateManagerGLES::SetSamplerState(dword sampler, SamplerState state, dword value)
{	
}
	
void RenderServiceGLES::StateManagerGLES::SetupClipPlane(dword index, Vector4 plane)
{
	//glClipPlanef(GL_CLIP_PLANE0 + index, &plane.x);
}

void RenderServiceGLES::StateManagerGLES::SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset)
{
	if(enable)
	{
		glEnable ( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset(slopeZBias, depthOffset);
	}
	else
	{
		glDisable ( GL_POLYGON_OFFSET_FILL );
	}
}

void RenderServiceGLES::StateManagerGLES::SetScissor(dword enable)
{
	if (enable>0)
	{
		glEnable(GL_SCISSOR_TEST);
	}	
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}
}

void RenderServiceGLES::StateManagerGLES::SetScissorRect(const Rect& rect)
{
	glScissor(rect.left, en_core.Render()->Display()->GetHeight() - rect.bottom, rect.right - rect.left, rect.bottom - rect.top);		
}

void RenderServiceGLES::StateManagerGLES::GetScissorRect(Rect& rect)
{	
}

int RenderServiceGLES::StateManagerGLES::GetBlendArg(BlendArg arg)
{
	int arg_op = 0;

	if (arg == argZero)         arg_op = GL_ZERO;
	if (arg == argOne)          arg_op = GL_ONE;
	if (arg == argSrcColor)     arg_op = GL_SRC_COLOR;
	if (arg == argInvSrcColor)  arg_op = GL_ONE_MINUS_SRC_COLOR;
	if (arg == argSrcAlpha)     arg_op = GL_SRC_ALPHA;	
	if (arg == argInvSrcAlpha)  arg_op = GL_ONE_MINUS_SRC_ALPHA;	
	if (arg == argDestAlpha)    arg_op = GL_DST_ALPHA;
	if (arg == argInvDestAlpha) arg_op = GL_ONE_MINUS_DST_ALPHA;
	if (arg == argDestColor)    arg_op = GL_DST_COLOR;
	if (arg == argInvDestColor) arg_op = GL_ONE_MINUS_DST_COLOR;

	return arg_op;	
}

int RenderServiceGLES::StateManagerGLES::GetBlendOp(BlendOp op)
{
	int blend_op = 0;

	if (op == blendAdd) blend_op = GL_FUNC_ADD;
	if (op == blendSub) blend_op = GL_FUNC_SUBTRACT;
	//if (op == blendMin) blend_op = GL_MIN;
	//if (op == blendMax) blend_op = GL_MAX;
	if (op == blendRevSub) blend_op = GL_FUNC_REVERSE_SUBTRACT;	

	return blend_op;
}

int RenderServiceGLES::StateManagerGLES::GetCompareFunc(CompareFunc func)
{
	int cmp = 0;
	
	if (func == cmpNever) cmp = GL_NEVER;       
	if (func == cmpLess) cmp = GL_LESS;       
	if (func == cmpEqual) cmp = GL_EQUAL;       
	if (func == cmpLessEqual) cmp = GL_LEQUAL;
	if (func == cmpGreater) cmp = GL_GREATER;       
	if (func == cmpNotequal) cmp = GL_NOTEQUAL;       
	if (func == cmpGreaterqual) cmp = GL_GEQUAL;       
	if (func == cmpAlways) cmp = GL_ALWAYS;                   

	return cmp;
}

int RenderServiceGLES::StateManagerGLES::GetStencilOp(StencilOp op)
{
	int stencil_op = 0;

	if (op == stencilKeep) stencil_op = GL_KEEP;
	if (op == stencilZero) stencil_op = GL_ZERO;
	if (op == stencilReplace) stencil_op = GL_REPLACE;
	if (op == stencilIncSat) stencil_op = GL_INCR;
	if (op == stencilDecSat) stencil_op = GL_DECR;
	if (op == stencilInv) stencil_op = GL_INVERT;
	if (op == stencilInc) stencil_op = GL_INCR_WRAP;
	if (op == stencilDec) stencil_op = GL_DECR_WRAP;	 

	return stencil_op;
}

RenderServiceGLES::DisplayManagerGLES::DisplayManagerGLES() : DisplayManager()
{
}

void RenderServiceGLES::DisplayManagerGLES::ApplyVideoMode(VideoMode mode)
{	
	width = mode.width;
	height = mode.height;			
	aspectRatio = (float)width / (float)height;	

	DisplayManager::ApplyVideoMode(mode);
}		

void RenderServiceGLES::DisplayManagerGLES::SetViewport(const Viewport& viewport)
{
	lastViewport = viewport;
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
	CheckGlError("glViewport")

    glDepthRangef(viewport.minZ, viewport.maxZ);
	CheckGlError("glDepthRangef")
}

void RenderServiceGLES::DisplayManagerGLES::GetViewport(Viewport& viewport)
{
	viewport.width = lastViewport.width;
    viewport.height = lastViewport.height;
    viewport.x = lastViewport.x;
    viewport.y = lastViewport.y;
    viewport.minZ = lastViewport.minZ;
    viewport.maxZ = lastViewport.maxZ;    
}

bool RenderServiceGLES::TextureDataGLES::GetInfo(byte* header, class IFile* fl)
{
	if (TextureData::GetInfo(header, fl)) return true;

	//PVTRC
	{
		PVRTexHeader* pvr_header = (PVRTexHeader*)header;
    
		has_alpha = pvr_header->bitmaskAlpha;
    
		bytesPerPixel = 0;

		switch (pvr_header->flags & 0xff)
		{
			case PVRTextureFlagTypePVRTC_2:
				bytesPerPixel = 2;
			break;
			case PVRTextureFlagTypePVRTC_4:
				bytesPerPixel = 4;
			break;
		}		

		if (bytesPerPixel != 0)
		{        
			if (pvr_header->width == pvr_header->height)
			{
				width = pvr_header->width;
				height = pvr_header->height;
				lods = 1;

				gl_format = tex_gles_pvrc;

				return true;
			}			
		}
	}

	{
		byte* ptr = &header[0];

		has_alpha = *((int*)ptr);
		ptr += 4;

		int sz = *((dword*)ptr) - 16;
		ptr += 4;
			
		//"PKM "
		ptr += 4;

		//version		
		ptr += 2;

		//mipmaps	
		ptr += 2;	

		word s_width = 0;
		word s_height = 0;

		s_width = *((word*)ptr);
		ptr += 2;

		s_height = *((word*)ptr);
		ptr += 2;

		SwapWord(s_width);
		SwapWord(s_height);

		width = s_width;
		height = s_height;
	
		if (has_alpha == 1)
		{
			bytesPerPixel = 4;
		}
		else
		{
			bytesPerPixel = 3;
		}	

		gl_format = tex_gles_etc1;

		return true;
	}

	core->TraceTo("Render", "GetInfo11");

	return false;
}

bool RenderServiceGLES::TextureDataGLES::LoadImpl()
{	
	if (TextureData::LoadImpl()) return true;
	
	if (gl_format == tex_gles_pvrc)
	{
		return LoadPVRC();
	}
	else	
    if (gl_format == tex_gles_etc1)
    {
        return LoadETC1();
    }

	return false;
}

bool RenderServiceGLES::TextureDataGLES::LoadPVRC()
{		
	PVRTexHeader* header = (PVRTexHeader*)file->GetBuffer();
    
	if (header->bitmaskAlpha)
	{
		has_alpha = 1;
	}
		
	bytesPerPixel = 2;

	switch (header->flags & 0xff)
	{
        case PVRTextureFlagTypePVRTC_2:
			bytesPerPixel = 2;
        break;
        case PVRTextureFlagTypePVRTC_4:
			bytesPerPixel = 4;
        break;
        default:            
			return false;
				
        break;
    }
        
	width = header->width;
	height = header->height;
	lods = 1;
        
    byte* ptr = (byte*)file->GetBuffer();
    ptr += header->headerLength;
    
	if (width != height)
	{
		return false;
	}
	
	int size = MathMax(32, width * height * bytesPerPixel / 8);
    
    data = (byte*)malloc(size);
	memcpy(data, ptr, size);
    
	return true;
}

bool RenderServiceGLES::TextureDataGLES::LoadETC1()
{
#ifdef ANDROID	
	data = (byte*)malloc(file->GetSize());
	memcpy(data, file->GetBuffer(), file->GetSize());
#endif
	
    return true;
}

RenderServiceGLES::RenderServiceGLES() : RenderService()
{
	bindVBuffer = false;

	last_vbuffer = null;	
	last_ibuffer = null;	
	lastVshader = 0;	
	//lastPshader = 0;		
	
	defaultFramebuffer = 0;
	colorRenderbuffer = 0;

	is_resourses_dead = false;
}

void RenderServiceGLES::Begin()
{
	RenderService::Begin();

	//glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
	
	//glClearColor(0.8f, 0.0f, 0.0f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	CheckGlError("glClearColor")
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CheckGlError("glClear")

    glViewport(0, 0, display_manager.width, display_manager.height);
    
	CheckGlError("glViewport")
	
	glEnable(GL_DEPTH_TEST);	
	CheckGlError("glEnable(GL_DEPTH_TEST")
	
	task_manager.Execute();
}

void RenderServiceGLES::End()
{
	RenderService::End();
}

bool RenderServiceGLES::Init()
{
    quene_class_size = sizeof(TextureDataGLES);
    quene_texture = (byte*)(new TextureDataGLES[512]);
    
#if defined(IOS)	
    
    /*allow_stream_atx = false;
    
    if (String::IsEqual(core->GetDeviceName(), "iPod1") ||
        String::IsEqual(core->GetDeviceName(), "iPod2") ||
        String::IsEqual(core->GetDeviceName(), "iPod3") ||
        String::IsEqual(core->GetDeviceName(), "iPod4") ||
        String::IsEqual(core->GetDeviceName(), "iPad1") ||
        String::IsEqual(core->GetDeviceName(), "iPhone1") ||
        String::IsEqual(core->GetDeviceName(), "iPhone2") ||
        String::IsEqual(core->GetDeviceName(), "iPhone3"))
    {
        allow_stream_atx = true;
    }*/
    
	glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, display_manager.width, display_manager.height);
	
	glGenRenderbuffers(1, &colorRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
			
	glGenFramebuffers(1, &defaultFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
#endif
    
	RenderService::Init(0, 0);

	return true;
}

void RenderServiceGLES::Update()
{	
}

void RenderServiceGLES::Clear(bool renderTarget, Color color, bool zbuffer, float zValue, bool stencil, dword stencilValue)
{
	dword flags = 0;

   if(renderTarget) flags |= GL_COLOR_BUFFER_BIT;
   if(zbuffer) flags |= GL_DEPTH_BUFFER_BIT;
   if(stencil) flags |= GL_STENCIL_BUFFER_BIT;

   glClearColor(color.r, color.g, color.b, color.a);
   glClearDepthf(zValue);
   glClearStencil(stencilValue);
   glClear(flags);
}

void RenderServiceGLES::DrawPrimitive(Primitive type, int startVertex, int primCount)
{
	draw_calls++;
	int count =  CalcPrimCount(type, primCount); 
	poly_count += primCount;

	if (bindVBuffer)
	{
		if (last_vbuffer && lastVshader)
		{
			((VertexBufferGLES*)last_vbuffer)->SetComponents((VertexShaderGLES*)lastVshader);
		}

		bindVBuffer = false;
	}

	cur_tech->ActivateParams();	

	glDrawArrays(GetPrimitiveType(type), startVertex, count);
	CheckGlError("glDrawArrays")

	cur_tech->DeactivateParams();
}

void RenderServiceGLES::DrawIndexedPrimitive(Primitive type, int startIndex, int primCount)
{
	draw_calls++;
	int count =  CalcPrimCount(type, primCount); 
	poly_count += primCount;
    
	//if (bindVBuffer)
	{
		if (last_vbuffer && lastVshader)
		{
			((VertexBufferGLES*)last_vbuffer)->SetComponents((VertexShaderGLES*)lastVshader);
		}

		bindVBuffer = false;
	}

	cur_tech->ActivateParams();	
		
	glDrawElements(GetPrimitiveType(type), count, GL_UNSIGNED_SHORT, (GLvoid*)(startIndex * sizeof(word)) );

	cur_tech->DeactivateParams();
}

Texture* RenderServiceGLES::CreateTexture(int width, int height, int lods, Texture::Format format, const char* file, int line)
{	
	int fmt = GL_RGBA;

	if (format == Texture::fmt_a8_r8)
	{
        fmt = GL_LUMINANCE_ALPHA;
	}

	return new TextureGLES(width, height, fmt, lods);
}

Texture* RenderServiceGLES::CreateCubmapTexture(int width, int height, int lods, int format, const char* file, int line)
{
	return null;
}

Texture* RenderServiceGLES::CreateVolumeTexture(int width, int height, int depth, int lods, int format, const char* file, int line)
{
	return null;
}

VertexBuffer* RenderServiceGLES::CreateVertexBuffer(dword size,int stride, const char* file, int line)
{
	return new VertexBufferGLES(size, stride);
}

void RenderServiceGLES::SetVertexBuffer(int index, VertexBuffer* buffer)
{	
	//if (last_vbuffer == buffer) return;

	last_vbuffer = buffer;
	bindVBuffer = true;

	if (!buffer) return;

#if defined(ANDROID)
	return;
#endif

#if !defined(ANDROID) && !defined(IOS) && !defined(OSUNIX)		

	VertexBufferIOS* bufferGL = (VertexBufferIOS*)buffer;
		
	//glEnableClientState(GL_VERTEX_ARRAY);	
	glBindBuffer(GL_ARRAY_BUFFER, bufferGL->vbo);	

#endif
}

IndexBuffer* RenderServiceGLES::CreateIndexBuffer(dword size, const char* file, int line)
{
	return new IndexBufferGLES(size);	
}

void RenderServiceGLES::SetIndexBuffer(IndexBuffer* buffer)
{
	if (last_ibuffer == buffer) return;
	last_ibuffer = buffer;

	if (buffer)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *((GLuint*)buffer->GetData()));
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0); 
	}
}

VertexShader* RenderServiceGLES::CreateVertexShaderImpl(const char* name)
{
	return new VertexShaderGLES(name);
}

void RenderServiceGLES::SetVertexShader(VertexShader* shader)
{	
	if (shader == lastVshader) return;

	/*CGprogram vshader = 0;

	if (shader)
	{
		vshader = (CGprogram)(shader->GetData());
	}*/
		
	//if (lastVshader != 0)
	//{
	//	cgGLUnbindProgram(lastVshader);	
	//}

	lastVshader = shader;
	bindVBuffer = true;		

	//if (vshader != 0) cgGLBindProgram(vshader);
}

PixelShader* RenderServiceGLES::CreatePixelShaderImpl(const char* name)
{
	return new PixelShaderGLES(name);
}

void RenderServiceGLES::SetPixelShader(PixelShader* shader)
{
	/*CGprogram pshader = 0;

	if (shader)
	{
		pshader = (CGprogram)(shader->GetData());
	}

	if (pshader == lastPshader) return;
	
	//if (lastPshader != 0)
	//{
	//	cgGLUnbindProgram(lastPshader);	
	//}

	lastPshader = pshader;	
	if (lastPshader != 0) cgGLBindProgram(lastPshader);*/
}

void RenderServiceGLES::Release()
{	
	//glDeleteFramebuffersOES(1,&fbo);
	glFinish();
			
	last_vbuffer = null;
	last_ibuffer = null;	
	lastVshader = null;		
	

#ifdef GLES2
	if (defaultFramebuffer)
    {
        glDeleteFramebuffers(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }
	
    if (colorRenderbuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }
#endif
	
	RenderService::Release();
}

int RenderServiceGLES::GetPrimitiveType(Primitive type)
{
	int tp = GL_TRIANGLES; 
	switch (type)
	{
		case pointsList:
		{
			tp = GL_POINTS;
		}
		break;
		case lineStrip:
		{
			tp = GL_LINE_STRIP;
		}
		break;
		case linesList:
		{
			tp = GL_LINES;
		}
		break;
		case triangleStrip:
		{
			tp = GL_TRIANGLE_STRIP;
		}
		break;		
		case trianglesList:
		{
			tp = GL_TRIANGLES;
		}
		break;		
	}

	return tp;
}

int RenderServiceGLES::CalcPrimCount(Primitive type, int primCount)
{
	int tp = primCount; 

	switch (type)
	{
		case pointsList:
		{
			tp = primCount;
		}
		break;
		case lineStrip:
		{
			tp = primCount + 1;
		}
		break;
		case linesList:
		{
			tp = primCount * 2;
		}
		break;
		case triangleStrip:
		{
			tp = primCount + 2;
		}
		break;		
		case trianglesList:
		{
			tp = primCount * 3;
		}
		break;		
	}

	return tp;	
}

void RenderServiceGLES::ReCreateResources()
{
	last_vbuffer = null;	
	last_ibuffer = null;	
	lastVshader = 0;	
	bindVBuffer = false;

//#ifdef ANDROID
	is_resourses_dead = true;
    en_core.Font()->Reset();
//#endif
    
    //is_resourses_dead = true;


	RenderService::ReCreateResources();

//#ifdef ANDROID
	is_resourses_dead = false;
    en_core.Font()->Restore();
//#endif
    
    is_resourses_dead = false;

}

void RenderServiceGLES::ResetTextures()
{
    for (int i=0; i < textures.Size(); i++)
	{
		textures.Get(i)->Reset();
	}
}

void RenderServiceGLES::RestoreTextures()
{
    for (int i=0; i < textures.Size(); i++)
	{
		textures.Get(i)->Restore();
	}
}

bool RenderServiceGLES::ResoursesAreDead()
{
	return is_resourses_dead;
}

void RenderServiceGLES::MakeScreenshot(std::string& out_image)
{
#ifdef OSUNIX
	char str[512];	
	String::Printf(str, 512,"%s/screenshots", en_core.EN_Files()->GetAppDirectory());

	CreateDirectories(str);

	en_core.EN_Files()->GetAppDirectory();
	
	if (out_image.size() == 0)
	{
		String::Printf(str, 512,"%s/screenshots/Screen_%i_%i_%i_%i_%i_%i.bmp", en_core.EN_Files()->GetAppDirectory(), GetTime(5),GetTime(4),GetTime(3),GetTime(0),GetTime(1),GetTime(2));
	}
	else
	{
		String::Printf(str, 512,"%s/screenshots/%s.bmp", en_core.EN_Files()->GetAppDirectory(), out_image.c_str());
	}
	
	int sz = 3 * Display()->GetWidth() * Display()->GetHeight();
	byte* pixels = (byte*)malloc( sz );
	
	if (!pixels)
	{
		core->TraceTo("Render", "No memmory for %i x %i screenshoot (%i bytea)", Display()->GetWidth(), Display()->GetHeight(), sz);
		return;
	}

	glReadPixels(0, 0, Display()->GetWidth(), Display()->GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, pixels);

	unsigned char file[14] =
	{
		'B','M', // magic
		0,0,0,0, // size in bytes
		0,0, // app data
		0,0, // app data
		40+14,0,0,0 // start of data offset
	};

	unsigned char info[40] =
	{
		40,0,0,0, // info hd size
		0,0,0,0, // width
		0,0,0,0, // heigth
		1,0, // number color planes
		24,0, // bits per pixel
		0,0,0,0, // compression is none
		0,0,0,0, // image bits size
		0x13,0x0B,0,0, // horz resoluition in pixel / m
		0x13,0x0B,0,0, // vert resolutions (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
		0,0,0,0, // #colors in pallete
		0,0,0,0, // #important colors
    };

	int w=Display()->GetWidth();
	int h=Display()->GetHeight();

	int padSize  = (4-w%4)%4;
	int sizeData = w*h*3 + h*padSize;
	int sizeAll  = sizeData + sizeof(file) + sizeof(info);

	file[ 2] = (unsigned char)( sizeAll    );
	file[ 3] = (unsigned char)( sizeAll>> 8);
	file[ 4] = (unsigned char)( sizeAll>>16);
	file[ 5] = (unsigned char)( sizeAll>>24);

	info[ 4] = (unsigned char)( w   );
	info[ 5] = (unsigned char)( w>> 8);
	info[ 6] = (unsigned char)( w>>16);
	info[ 7] = (unsigned char)( w>>24);

	info[ 8] = (unsigned char)( h    );
	info[ 9] = (unsigned char)( h>> 8);
	info[10] = (unsigned char)( h>>16);
	info[11] = (unsigned char)( h>>24);

	info[24] = (unsigned char)( sizeData    );
	info[25] = (unsigned char)( sizeData>> 8);
	info[26] = (unsigned char)( sizeData>>16);
	info[27] = (unsigned char)( sizeData>>24);

	IFile* fl = core->Files()->OpenFile(str, IFile::write);
	
	fl->Write(file, sizeof(file));
	
	fl->Write(info, sizeof(info));
	
		
	
	for ( int y=0; y<h; y++ )
	{
		for ( int x=0; x<w; x++ )
		{			
			byte tmp = pixels[(y * w + x) * 3];
			pixels[(y * w + x) * 3] = pixels[(y * w + x) * 3 + 2];
			pixels[(y * w + x) * 3 + 2] = tmp;				
		}		
	}

	fl->Write(pixels, sz);
	
	fl->Release();
	
	free(pixels);	
#endif
}