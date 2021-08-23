
#pragma once

#include "RenderService.h"

class RenderServiceD3D11 : public RenderService
{			
	friend class IndexBufferD3D11;
	friend class VertexBufferD3D11;
	friend class VertexShaderD3D11;
	friend class PixelShaderD3D11;	
	friend class TextureD3D11;
	friend class ShaderPlatform;
	friend class TextureBasePlatform;
	friend class ShaderInfoD3D11;	

	class ID3D11Device*	          pd3dDevice;
	class ID3D11DeviceContext*    immediateContext;
	class IDXGISwapChain*         swapChain;
	class ID3D11RenderTargetView* renderTargetView;
	class ID3D11Texture2D*        depthStencil;
	class ID3D11DepthStencilView* depthStencilView;
	class IDXGIFactory1* factory;

	static RenderServiceD3D11* instance;

	VertexBuffer* last_vbuffer;
	IndexBuffer* last_ibuffer;

	class ID3D11InputLayout* last_decl;	

	VertexShader* vshader;
	PixelShader*  pshader;		

	/*IDirect3DSurface9* pBackBuffer[5];
	IDirect3DSurface9* pZStencilSurface[5];	

	IDirect3D9*		  d3d;
	static RenderServiceD3D* instance;
	static IDirect3DDevice9* device;
	
	IDirect3DTexture9* used_textures[16];

	
	
	int numRtargets;
	RenderTargetD3D* rtargets[64];

	int numDepths;
	RenderTargetD3D* depths[64];

	bool need_post_reset;	

	IDirect3DSurface9* rt1Surface;
	RenderTarget* rt1;	
	
	bool vertex_seted;
	bool need_update_rt;		

	dword miltisample;	
*/

	class StateManagerD3D11 : public StateManager
	{
	public:

		dword clipInfo;		
		
		struct D3D11_BLEND_DESC* blend_desc;
		class ID3D11BlendState*  blend_state;
		bool			         blend_changed;

		struct D3D11_DEPTH_STENCIL_DESC* ds_desc;
		class ID3D11DepthStencilState*   ds_state;
		int                              ds_stencil_ref;
		bool			                 ds_changed;
		
		struct D3D11_RASTERIZER_DESC* raster_desc;
		class ID3D11RasterizerState*  raster_state;
		bool			              raster_changed;

		StateManagerD3D11();

		void Init();
		void ApplyChanges();

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

	StateManagerD3D11 state_manager;

	class DisplayManagerD3D11 : public DisplayManager
	{
	public:	
		
		DisplayManagerD3D11();		

		virtual void ApplyVideoMode(VideoMode mode);
				
		virtual void SetViewport(const Viewport& viewport);
		virtual void GetViewport(Viewport& viewport);			
	};

	DisplayManagerD3D11 display_manager;

public:		
	
	RenderServiceD3D11();	
		
	virtual bool Init(int wgt, int hgt);	
	void RecreateSwapchain(int wgt, int hgt);
			
	virtual bool Reset();

	virtual void Begin();
	virtual void End();				

	virtual void Clear(bool renderTarget, Color color, bool zbuffer, float zValue, bool stencil, dword stencilValue );

	virtual void DrawPrimitive(Primitive type, int startVertex, int primCount);
	virtual void DrawIndexedPrimitive(Primitive type, int startIndex, int primCount);	

	virtual Texture* CreateTexture(int width, int height, int lods, Texture::Format format, const char* file, int line);	

	virtual VertexBuffer* CreateVertexBuffer(dword size,int stride, const char* file, int line);
	virtual void SetVertexBuffer(int index, VertexBuffer* buffer);

	virtual IndexBuffer* CreateIndexBuffer(dword size, const char* file, int line);
	virtual void SetIndexBuffer(IndexBuffer* buffer);
	
	virtual VertexShader* CreateVertexShaderImpl(const char* name);
	virtual void SetVertexShader(VertexShader* shader);
	
	virtual PixelShader* CreatePixelShaderImpl(const char* name);
	virtual void SetPixelShader(PixelShader* shader);
	
	virtual void Release();

	void SetVertexDeclaration(class ID3D11InputLayout* decl);

	//void DeleteRenderTarget(RenderTargetD3D* rt);
	
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
	int  CalcPrimCount(Primitive type, int primCount);
};
