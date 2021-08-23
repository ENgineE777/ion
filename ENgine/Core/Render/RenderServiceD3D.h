
#pragma once

#ifdef PC
#define NOMINMAX
#pragma push_macro("new")
#undef new
#include "d3d9.h"
#pragma pop_macro("new")
#endif

#include "RenderService.h"

class RenderServiceD3D : public RenderService
{			
	friend class VertexBufferD3D;
	friend class IndexBufferD3D;
	friend class ShaderPlatform;
	friend class VertexShaderD3D;
	friend class PixelShaderD3D;
	friend class TextureBasePlatform;	
	friend class TextureD3D;		
	friend class ShaderInfoD3D;		


	IDirect3DSurface9* pBackBuffer[5];
	IDirect3DSurface9* pZStencilSurface[5];

	VertexBuffer* last_vbuffer;
	IndexBuffer* last_ibuffer;

	IDirect3D9*		  d3d;
	static RenderServiceD3D* instance;
	IDirect3DDevice9* device;
	
	IDirect3DTexture9* used_textures[16];

	VertexShader* vshader;
	PixelShader*  pshader;	

	IDirect3DVertexDeclaration9* last_decl;	
		
	bool need_post_reset;		
	
	bool vertex_seted;
	bool need_update_rt;		

	dword miltisample;	

	class StateManagerD3D : public StateManager
	{
	public:

		dword clipInfo;		

		StateManagerD3D();
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
	};

	StateManagerD3D state_manager;

	class DisplayManagerD3D : public DisplayManager
	{
	public:	
		
		DisplayManagerD3D();		

		virtual void ApplyVideoMode(VideoMode mode);
				
		virtual void SetViewport(const Viewport& viewport);
		virtual void GetViewport(Viewport& viewport);			
	};

	DisplayManagerD3D display_manager;

public:		

	RenderServiceD3D();	
		
	virtual bool Init(int wgt, int hgt);	
			
	virtual bool Reset();

	virtual void Begin();
	virtual void End();			

	virtual void Clear(bool renderTarget, Color color, bool zbuffer, float zValue, bool stencil, dword stencilValue );

	virtual void DrawPrimitive(Primitive type, int startVertex, int primCount);
	virtual void DrawIndexedPrimitive(Primitive type, int startIndex, int primCount);

	void SetTexture(int index, Texture* texture);

	virtual Texture* CreateTexture(int width, int height, int lods, Texture::Format format, const char* file, int line);		

	virtual VertexBuffer* CreateVertexBuffer(dword size, int stride, const char* file, int line);
	virtual void SetVertexBuffer(int index, VertexBuffer* buffer);

	virtual IndexBuffer* CreateIndexBuffer(dword size, const char* file, int line);
	virtual void SetIndexBuffer(IndexBuffer* buffer);
	
	virtual VertexShader* CreateVertexShaderImpl(const char* name);
	virtual void SetVertexShader(VertexShader* shader);
	
	virtual PixelShader* CreatePixelShaderImpl(const char* name);
	virtual void SetPixelShader(PixelShader* shader);
	
	virtual void Release();

	void SetVertexDeclaration(IDirect3DVertexDeclaration9* decl);
	
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

	int  GetPrimitiveType(Primitive type);		
};
