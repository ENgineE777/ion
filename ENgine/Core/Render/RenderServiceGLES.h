
#pragma once

#include "RenderService.h"

class RenderServiceGLES : public RenderService
{		
	GLuint fbo;	

	bool bindVBuffer;
	VertexBuffer* last_vbuffer;
	IndexBuffer* last_ibuffer;
	
	VertexShader* lastVshader;	

	GLuint defaultFramebuffer;
	GLuint colorRenderbuffer;
	GLuint depthRenderBuffer;
	
	class StateManagerGLES : public StateManager
	{
	public:
		
		StateManagerGLES();
		virtual void SetAlphaTest(dword enable);
		virtual void SetAlphaTestFunc(CompareFunc func, dword ref);

		virtual void SetAlphaBlend(dword enable);
		virtual void SetBlendFunc(BlendArg src, BlendArg dest);
		virtual void SetBlendOperation(BlendOp op);
	
		virtual void SetDepthTest(dword enable);	
		virtual void SetDepthWriting(dword enable);	
		virtual void SetDepthFunc(CompareFunc func);

		virtual void SetCulling(CullMode mode);	

		virtual void SetStencilTest(dword enable);	
		virtual void SetStencilTestFunc(CompareFunc func, dword ref, dword mask);
		virtual void SetStencilWriteMask(dword mask);
		virtual void SetStencilOperation(StencilOp failOp, StencilOp zFailOp, StencilOp zPass);

		virtual void SetColorWrite(bool red, bool green, bool blue, bool alpha);	
		virtual void SetClipPlane(dword clipIndex, dword enable);	
	
		virtual void SetWireFrameMode(dword enable); 
	
		virtual void SetSamplerState(dword sampler, SamplerState state, dword value);		

		virtual void SetupClipPlane(dword index, Vector4 plane);

		virtual void SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset);		

		virtual void SetScissor(dword enable);
		virtual void SetScissorRect(const Rect& rect);
		virtual void GetScissorRect(Rect& rect);	

		int GetBlendArg(BlendArg arg);
		int GetBlendOp(BlendOp op);
		int GetCompareFunc(CompareFunc func);
		int GetStencilOp(StencilOp op);
	};

	StateManagerGLES state_manager;

	class DisplayManagerGLES : public DisplayManager
	{
	public:	
		
		Viewport lastViewport;

		DisplayManagerGLES();		

		virtual void ApplyVideoMode(VideoMode mode);
				
		virtual void SetViewport(const Viewport& viewport);
		virtual void GetViewport(Viewport& viewport);			
	};

	DisplayManagerGLES display_manager;

public:

	class TextureDataGLES : public TextureData
	{
	public:

		enum GLESFormat
		{
			tex_gles_pvrc = 0,
			tex_gles_etc1 = 1,
			tex_gles_unknown = 2
		};

		GLESFormat gl_format;
		int        has_alpha;

		TextureDataGLES() : TextureData()
		{
			gl_format = tex_gles_unknown;			
			has_alpha = 0;
		};
		
		virtual bool GetInfo(byte* header, class IFile* fl);		
		virtual bool LoadImpl();	

		bool LoadPVRC();
		bool LoadETC1();
	};

	bool is_resourses_dead;

	RenderServiceGLES();		

	virtual void Begin();
	virtual void End();

	virtual bool Init();
	virtual void Update();	
	
	virtual void Clear(bool renderTarget, Color color, bool zbuffer, float zValue, bool stencil, dword stencilValue );	
	
	virtual void DrawPrimitive(Primitive type, int startVertex, int primCount);
	virtual void DrawIndexedPrimitive(Primitive type, int startIndex, int primCount);

	virtual Texture* CreateTexture(int width, int height, int lods, Texture::Format format, const char* file, int line);
	virtual Texture* CreateCubmapTexture(int width, int height, int lods, int format, const char* file, int line);
	virtual Texture* CreateVolumeTexture(int width, int height, int depth, int lods, int format, const char* file, int line);
	
	virtual VertexBuffer* CreateVertexBuffer(dword size, int stride, const char* file, int line);
	virtual void SetVertexBuffer(int index, VertexBuffer* buffer);

	virtual IndexBuffer* CreateIndexBuffer(dword size, const char* file, int line);
	virtual void SetIndexBuffer(IndexBuffer* buffer);

	virtual VertexShader* CreateVertexShaderImpl(const char* name);
	virtual void SetVertexShader(VertexShader* shader);

	virtual PixelShader* CreatePixelShaderImpl(const char* name);
	virtual void SetPixelShader(PixelShader* shader);		

	virtual void Release();				 				

	virtual void ReCreateResources();
    
    virtual void ResetTextures();
    virtual void RestoreTextures();	
	
	bool ResoursesAreDead();

	virtual void MakeScreenshot(std::string& out_image);

	virtual DisplayManager* Display()
	{
		return &display_manager;
	};

	virtual StateManager* State()
	{
		return &state_manager;
	};

protected:

	int GetPrimitiveType(Primitive type); 
	int CalcPrimCount(Primitive type, int primCount); 	
};

#define CheckGlError(op) \
{ \
     for (GLint error = glGetError(); error; error = glGetError())\
	 {\		 
     }\
}

#define CheckGlErrorFN(op, filename) \
{ \
     for (GLint error = glGetError(); error; error = glGetError())\
	 {\		 
     }\
}
