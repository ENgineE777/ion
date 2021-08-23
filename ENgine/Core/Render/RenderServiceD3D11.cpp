
#include "RenderServiceD3D11.h"
#include "Core/Core.h"

#include "Geometry/VertexBufferD3D11.h"
#include "Geometry/IndexBufferD3D11.h"
#include "Shaders/VertexShaderD3D11.h"
#include "Shaders/PixelShaderD3D11.h"
#include "Textures/TextureD3D11.h"

#include "d3d11.h"

RenderServiceD3D11* RenderServiceD3D11::instance = null;

RenderServiceD3D11::StateManagerD3D11::StateManagerD3D11()
{
	clipInfo = 0;

	blend_desc = new D3D11_BLEND_DESC();
	ZeroMemory( blend_desc, sizeof( D3D11_BLEND_DESC ) );
	blend_state = null;
	blend_changed = true;	

	ds_desc = new D3D11_DEPTH_STENCIL_DESC();
	ZeroMemory( ds_desc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	ds_state = null;
	ds_stencil_ref = 0;
	ds_changed = true;	

	raster_desc = new D3D11_RASTERIZER_DESC();
	ZeroMemory( raster_desc, sizeof( D3D11_RASTERIZER_DESC ) );
	raster_state = null;	
	raster_changed = true;
}

void RenderServiceD3D11::StateManagerD3D11::Init()
{		
	blend_desc->RenderTarget[0].BlendEnable = false;

	blend_desc->RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc->RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc->RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	
	ds_desc->DepthEnable = true;
	ds_desc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds_desc->DepthFunc = D3D11_COMPARISON_LESS;
	
	ds_desc->StencilEnable = false;
	ds_desc->StencilReadMask = 0xFF;
	ds_desc->StencilWriteMask = 0xFF;

	ds_desc->FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds_desc->FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	ds_desc->FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_desc->FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	ds_desc->BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds_desc->BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	ds_desc->BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_desc->BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;		
	
	raster_desc->AntialiasedLineEnable = false;
	raster_desc->CullMode = D3D11_CULL_NONE;
	raster_desc->DepthBias = 0;
	raster_desc->DepthBiasClamp = 0.0f;
	raster_desc->DepthClipEnable = false;

	raster_desc->FillMode = D3D11_FILL_SOLID;
	raster_desc->FrontCounterClockwise = false;
	raster_desc->MultisampleEnable = false;
	raster_desc->ScissorEnable = false;
	raster_desc->SlopeScaledDepthBias = 0.0f;		
}

void RenderServiceD3D11::StateManagerD3D11::ApplyChanges()
{
	if (blend_changed)
	{
		RELEASE(blend_state)
			RenderServiceD3D11::instance->pd3dDevice->CreateBlendState(blend_desc, &blend_state);
		RenderServiceD3D11::instance->immediateContext->OMSetBlendState(blend_state, 0, 0xffffffff);

		blend_changed = false;
	}

	if (ds_changed)
	{
		RELEASE(ds_state)
			RenderServiceD3D11::instance->pd3dDevice->CreateDepthStencilState(ds_desc, &ds_state);
		RenderServiceD3D11::instance->immediateContext->OMSetDepthStencilState(ds_state, ds_stencil_ref);

		ds_changed = false;
	}

	if (raster_changed)
	{
		RELEASE(raster_state)
		RenderServiceD3D11::instance->pd3dDevice->CreateRasterizerState(raster_desc, &raster_state);
		RenderServiceD3D11::instance->immediateContext->RSSetState(raster_state);	

		raster_changed = false;
	}
}

void RenderServiceD3D11::StateManagerD3D11::SetAlphaTest(dword enable)
{
	//SET_STATE(D3DRS_ALPHATESTENABLE, enable)
}

void RenderServiceD3D11::StateManagerD3D11::SetAlphaTestFunc(CompareFunc func, dword ref)
{
	//SET_STATE(D3DRS_ALPHAFUNC, func)
	//SET_STATE(D3DRS_ALPHAREF, ref)
}

void RenderServiceD3D11::StateManagerD3D11::SetAlphaBlend(dword enable)
{	
	blend_desc->RenderTarget[0].BlendEnable = enable;

	blend_changed = true;
}

void RenderServiceD3D11::StateManagerD3D11::SetBlendFunc(BlendArg src, BlendArg dest)
{
	blend_desc->RenderTarget[0].SrcBlend = (D3D11_BLEND)src;
	blend_desc->RenderTarget[0].DestBlend = (D3D11_BLEND)dest;

	blend_changed = true;	
}

void RenderServiceD3D11::StateManagerD3D11::SetBlendOperation(BlendOp op)
{
	blend_desc->RenderTarget[0].BlendOp = (D3D11_BLEND_OP)op;

	blend_changed = true;
}
	
void RenderServiceD3D11::StateManagerD3D11::SetDepthTest(dword enable)
{
	ds_desc->DepthEnable = enable;
	ds_changed = true;
}

void RenderServiceD3D11::StateManagerD3D11::SetDepthWriting(dword enable)
{
	if (enable)
	{
		ds_desc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	}
	else
	{
		ds_desc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}

	ds_changed = true;
}

void RenderServiceD3D11::StateManagerD3D11::SetDepthFunc(CompareFunc func)
{	
	ds_desc->DepthFunc = (D3D11_COMPARISON_FUNC)func;	
	ds_changed = true;
}

void RenderServiceD3D11::StateManagerD3D11::SetCulling(CullMode mode)
{
	raster_desc->CullMode = (D3D11_CULL_MODE)mode;
	raster_changed = true;		
}

void RenderServiceD3D11::StateManagerD3D11::SetStencilTest(dword enable)
{
	ds_desc->StencilEnable = enable;
	ds_changed = true;	
}

void RenderServiceD3D11::StateManagerD3D11::SetStencilTestFunc(CompareFunc func, dword ref, dword mask)
{				
	ds_desc->FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)func;
	ds_desc->BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)func;	

	ds_desc->StencilReadMask = mask;

	ds_stencil_ref = ref;

	ds_changed = true;
}

void RenderServiceD3D11::StateManagerD3D11::SetStencilWriteMask(dword mask)
{
	ds_desc->StencilWriteMask = mask;
	ds_changed = true;
}

void RenderServiceD3D11::StateManagerD3D11::SetStencilOperation(StencilOp failOp, StencilOp zFailOp, StencilOp zPass)
{	
	ds_desc->FrontFace.StencilFailOp = (D3D11_STENCIL_OP)failOp;
	ds_desc->FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)zFailOp;
	ds_desc->FrontFace.StencilPassOp = (D3D11_STENCIL_OP)zPass;
	
	ds_desc->BackFace.StencilFailOp = (D3D11_STENCIL_OP)failOp;
	ds_desc->BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)zFailOp;
	ds_desc->BackFace.StencilPassOp = (D3D11_STENCIL_OP)zPass;	

	ds_changed = true;
}

void RenderServiceD3D11::StateManagerD3D11::SetColorWrite(bool red, bool green, bool blue, bool alpha)
{
	/*if (!RenderServiceD3D::device) return;

	dword clrWrite = 0;
	
	if (red)   clrWrite |= D3DCOLORWRITEENABLE_RED;
	if (green) clrWrite |= D3DCOLORWRITEENABLE_GREEN;
	if (blue)  clrWrite |= D3DCOLORWRITEENABLE_BLUE;
	if (alpha) clrWrite |= D3DCOLORWRITEENABLE_ALPHA;
	
	//SET_STATE(D3DRS_COLORWRITEENABLE, clrWrite)	
	RenderServiceD3D::device->SetRenderState(D3DRS_COLORWRITEENABLE, clrWrite);*/
}

void RenderServiceD3D11::StateManagerD3D11::SetClipPlane(dword clipIndex, dword enable)
{
	/*if (!RenderServiceD3D::device) return;

	if (enable > 0)
	{
		clipInfo |= (1 << clipIndex); 
	}
	else
	{
		clipInfo &= ~(1 << clipIndex);
	}

	RenderServiceD3D::device->SetRenderState(D3DRS_CLIPPLANEENABLE, clipInfo);*/
}

void RenderServiceD3D11::StateManagerD3D11::SetWireFrameMode(dword enable)
{
	/*if (!RenderServiceD3D::device) return;

	if(enable)
	{
		RenderServiceD3D::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else
	{
		RenderServiceD3D::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}*/
}
	
void RenderServiceD3D11::StateManagerD3D11::SetSamplerState(dword sampler, SamplerState state, dword value)
{
	/*if (!RenderServiceD3D::device) return;

	dword state_status;
	RenderServiceD3D::device->GetSamplerState(sampler, (D3DSAMPLERSTATETYPE)state, &state_status);
	if (state_status != value)
	{
		RenderServiceD3D::device->SetSamplerState(sampler, (D3DSAMPLERSTATETYPE)state, value);
	}*/
}
	
void RenderServiceD3D11::StateManagerD3D11::SetupClipPlane(dword index, Vector4 plane)
{
	/*if (!RenderServiceD3D::device) return;

	RenderServiceD3D::device->SetClipPlane(index, &plane.v4[0]); */
}

void RenderServiceD3D11::StateManagerD3D11::SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset)
{	
	float curDepthBias = 0.0f;
	float curBiasSlope = 0.0f;

	if (enable)
	{
		curDepthBias = depthOffset;
		curBiasSlope = slopeZBias;
	}

	raster_desc->DepthBias = curDepthBias;
	raster_desc->SlopeScaledDepthBias = curBiasSlope;			

	raster_changed = true;	
}	

void RenderServiceD3D11::StateManagerD3D11::SetScissor(dword enable)
{
	raster_desc->ScissorEnable = enable;
	raster_changed = true;
}

void RenderServiceD3D11::StateManagerD3D11::SetScissorRect(const Rect& rect)
{
	D3D11_RECT rc;	
	rc.bottom = rect.bottom;
	rc.left = rect.left;
	rc.right = rect.right;
	rc.top = rect.top;

	RenderServiceD3D11::instance->immediateContext->RSSetScissorRects( 1, &rc );
}

void RenderServiceD3D11::StateManagerD3D11::GetScissorRect(Rect& rect)
{	
	D3D11_RECT rc;
	unsigned int num = 1;
	RenderServiceD3D11::instance->immediateContext->RSGetScissorRects( &num, &rc );

	rect.left = rc.left;
	rect.top = rc.top;
	rect.right = rc.right;
	rect.bottom = rc.bottom;
}

RenderServiceD3D11::DisplayManagerD3D11::DisplayManagerD3D11() : DisplayManager()
{
}

void RenderServiceD3D11::DisplayManagerD3D11::ApplyVideoMode(VideoMode mode)
{	
	width = mode.width;
	height = mode.height;
	core->TraceTo("Render", "RenderServiceD3D::ApplyVideoMode: %d x %d", width, height);
		
	aspectRatio = (float)width / (float)height;	

	instance->RecreateSwapchain(width, height);

	DisplayManager::ApplyVideoMode(mode);
}
				
void RenderServiceD3D11::DisplayManagerD3D11::SetViewport(const Viewport& viewport)
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)viewport.width;
	vp.Height = (float)viewport.height;
	vp.MinDepth = viewport.minZ;
	vp.MaxDepth = viewport.maxZ;
	vp.TopLeftX = viewport.x;
	vp.TopLeftY = viewport.y;
	
	RenderServiceD3D11::instance->immediateContext->RSSetViewports( 1, &vp );
}

void RenderServiceD3D11::DisplayManagerD3D11::GetViewport(Viewport& viewport)
{	
	D3D11_VIEWPORT d3dviewport;
	unsigned int num = 1;
	RenderServiceD3D11::instance->immediateContext->RSGetViewports( &num, &d3dviewport );	

	viewport.x = d3dviewport.TopLeftX;
	viewport.y = d3dviewport.TopLeftY;
	viewport.width = d3dviewport.Width;
	viewport.height = d3dviewport.Height;
	viewport.minZ = d3dviewport.MinDepth;
	viewport.maxZ = d3dviewport.MaxDepth;
}

RenderServiceD3D11::RenderServiceD3D11() : RenderService()
{	
	instance = this;

	pd3dDevice = null;
	immediateContext = null;
	swapChain = null;
	renderTargetView = null;
	depthStencil = null;
	depthStencilView = null;
	factory = null;

	last_vbuffer = null;
	last_ibuffer = null;	

	last_decl = null;	

	/*need_update_rt = false;

	for (int i=0;i<16;i++) used_textures[i] = 0;

	for (int i=0;i<5;i++)
	{
		pBackBuffer[i] = null;
		pZStencilSurface[i] = null;
	}

	vshader = null;
	pshader = null;	

	vertex_seted = false;

	last_decl = null;

#ifdef PC
	numRtargets = 0;	
	numDepths = 0;
	need_reset = false;
	need_post_reset = false;

	rt1 = 0;
	rt1Surface = 0;

#endif	

#ifdef PC 
	miltisample = D3DMULTISAMPLE_NONE;
#endif*/
}

bool RenderServiceD3D11::Init(int wgt, int hgt)
{	
	quene_class_size = sizeof(TextureData);
    quene_texture = (byte*)(new TextureData[512]);


	if (use_dummy_render)
	{
		RenderService::Init(wgt, hgt);	
		return true;
	}

	if (!en_core.GetHWND()) return true;	

	display_manager.width  = wgt;
	display_manager.height = hgt;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );
	
	D3D_DRIVER_TYPE   driverType;
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice( NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION,  &pd3dDevice, &featureLevel, &immediateContext );
		
		if( SUCCEEDED( hr ) )
		{
			break;		
		}
	}
	
	CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&factory));

	RecreateSwapchain(wgt, hgt);	

	RenderService::Init(wgt, hgt);

	state_manager.Init();

	return true;
}

void RenderServiceD3D11::RecreateSwapchain(int wgt, int hgt)
{
	RELEASE(depthStencilView)
	RELEASE(depthStencil)
	RELEASE(renderTargetView)	
	RELEASE(swapChain)		

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = wgt;
	sd.BufferDesc.Height = hgt;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = en_core.GetHWND();
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	factory->CreateSwapChain(pd3dDevice, &sd, &swapChain);


	//if( FAILED( hr ) )
	//return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	HRESULT hr = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );

	//if( FAILED( hr ) )
	//return hr;

	hr = pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &renderTargetView );
	pBackBuffer->Release();
	
	if( FAILED( hr ) )
	{
		return;
	}

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory( &descDepth, sizeof(descDepth) );
	descDepth.Width = wgt;
	descDepth.Height = hgt;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = pd3dDevice->CreateTexture2D( &descDepth, NULL, &depthStencil );
	
	if( FAILED( hr ) )
	{
		return;
	}		

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory( &descDSV, sizeof(descDSV) );
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = pd3dDevice->CreateDepthStencilView( depthStencil, &descDSV, &depthStencilView );

	//if( FAILED( hr ) )
	//return hr;	

	immediateContext->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)wgt;
	vp.Height = (FLOAT)hgt;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	immediateContext->RSSetViewports( 1, &vp );
}

bool RenderServiceD3D11::Reset()
{
	if (use_dummy_render) return true;	

	return true;
}

void RenderServiceD3D11::Begin()
{
	if (use_dummy_render) return;

	RenderService::Begin();

#ifdef EDITOR
	Clear(true, COLOR_GRAY, true, 1.0f, false, 0);	
#else
	Clear(true, COLOR_BLACK, true, 1.0f, false, 0);	
#endif

	if (!null_render && !use_dummy_render)
	{
		task_manager.Execute();
	}
}

void RenderServiceD3D11::End()
{	
	if (use_dummy_render) return;

	swapChain->Present( 0, 0 );

	RenderService::End();
}

Texture* RenderServiceD3D11::CreateTexture(int width, int height, int lods, Texture::Format format, const char* file, int line)
{	
	ID3D11Texture2D* texture = NULL;
		
	DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (format == Texture::fmt_a8_r8)
	{
		fmt = DXGI_FORMAT_R8G8_UNORM;
	}

	if (!use_dummy_render)
	{	
		D3D11_TEXTURE2D_DESC desc;

		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = fmt;		
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		RenderServiceD3D11::instance->pd3dDevice->CreateTexture2D(&desc, null, &texture);		
	}

	if (texture || use_dummy_render)
	{
		return new TextureD3D11(width, height, fmt, lods, texture);		
	}
	else
	{
		core->TraceTo("Render", "Can't create texture. Width: %d, Height: %d, Format: %d", width, height, format);
	}

	return null;
}

void RenderServiceD3D11::Clear(bool renderTarget, Color color, bool zbuffer, float zValue, bool stencil, dword stencilValue )
{
	if (use_dummy_render) return;
	
	if (renderTarget)
	{
		immediateContext->ClearRenderTargetView( renderTargetView, (float*)&color.r );
	}

	if (zbuffer)
	{
		immediateContext->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH, zValue, stencilValue );
	}
}

void RenderServiceD3D11::DrawPrimitive(Primitive type, int startVertex, int primCount)
{
	if (use_dummy_render) return;

	if (!vshader || !pshader)
	{
		return;
	}

	state_manager.ApplyChanges();	
	cur_tech->UpdateShaderConstants();
	
	draw_calls++;
	poly_count += primCount;
				
	immediateContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)GetPrimitiveType(type));
	immediateContext->Draw(CalcPrimCount(type, primCount), startVertex);
}

void RenderServiceD3D11::DrawIndexedPrimitive(Primitive type, int startIndex, int primCount)
{
	if (use_dummy_render) return;

	if (!vshader || !pshader)
	{
		return;
	}

	state_manager.ApplyChanges();
	cur_tech->UpdateShaderConstants();	

	draw_calls++;
	poly_count += primCount;    
		
	immediateContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)GetPrimitiveType(type));	
	immediateContext->DrawIndexed(CalcPrimCount(type, primCount), startIndex, 0 );
}

VertexBuffer* RenderServiceD3D11::CreateVertexBuffer(dword size,int stride, const char* file, int line)
{	
	VertexBufferD3D11* buff = new VertexBufferD3D11(size, stride);	

	vbuffers.push_back(buff);

	return buff;
}

void RenderServiceD3D11::SetVertexBuffer(int index, VertexBuffer* buffer)
{
	if (use_dummy_render) return;

	if (last_vbuffer == buffer) return;

	ID3D11Buffer* vb = null;
	unsigned int stride = 0;

	if (buffer)
	{
		vb = (ID3D11Buffer*)buffer->GetData();	
		stride = buffer->GetStride();
	}
	
	unsigned int offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &vb, &stride, &offset);	

	last_vbuffer = buffer;
}

IndexBuffer* RenderServiceD3D11::CreateIndexBuffer(dword size, const char* file, int line)
{
	IndexBufferD3D11* buff;

	buff = new IndexBufferD3D11(size);	

	ibuffers.push_back(buff);

	return buff;	
}

void RenderServiceD3D11::SetIndexBuffer(IndexBuffer* buffer)
{
	if (use_dummy_render) return;
	
	if (last_ibuffer == buffer) return;

	ID3D11Buffer* ib = null;
	if (buffer)
	{
		ib = (ID3D11Buffer*)buffer->GetData();	
	}

	immediateContext->IASetIndexBuffer( ib, DXGI_FORMAT_R16_UINT, 0 );
	
	last_ibuffer = buffer;
}

VertexShader* RenderServiceD3D11::CreateVertexShaderImpl(const char* name)
{
	return new VertexShaderD3D11(name);
}

void RenderServiceD3D11::SetVertexShader(VertexShader* shader)
{
	if (use_dummy_render) return;
	
	if (vshader == shader) return;

	ID3D11VertexShader* shd = NULL;
	if (shader) shd = (ID3D11VertexShader*)shader->GetData();

	immediateContext->VSSetShader( shd, NULL, 0 );	

	vshader = shader;

	if (vshader) vshader->SetDirty();
}

PixelShader* RenderServiceD3D11::CreatePixelShaderImpl(const char* name)
{
	return new PixelShaderD3D11(name);
}

void RenderServiceD3D11::SetPixelShader(PixelShader* shader)
{
	if (use_dummy_render) return;
	
	if (pshader == shader) return;

	ID3D11PixelShader* shd = NULL;
	if (shader) shd = (ID3D11PixelShader*)shader->GetData();

	immediateContext->PSSetShader(shd, 0, NULL);

	pshader = shader;
	if(pshader)
	{
		pshader->SetDirty();
	}
}

void RenderServiceD3D11::Release()
{	
	RenderService::Release();	
}

void RenderServiceD3D11::SetVertexDeclaration(ID3D11InputLayout* decl)
{
	if (use_dummy_render) return;

	if (last_decl != decl) immediateContext->IASetInputLayout(decl);
	last_decl = decl;
}

int RenderServiceD3D11::GetPrimitiveType(Primitive type)
{	
	int tp = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; 
	switch (type)
	{
		case pointsList:
		{
			tp = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
		}
		break;
		case lineStrip:
		{
			tp = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
		}
		break;
		case linesList:
		{
			tp = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
		}
		break;
		case triangleStrip:
		{
			tp = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		}
		break;		
		case trianglesList:
		{
			tp = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
		break;		
	}

	return tp;
}

int RenderServiceD3D11::CalcPrimCount(Primitive type, int primCount)
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

/*
void RenderServiceD3D11::DeleteRenderTarget(RenderTargetD3D* rt)
{	
#ifdef PC
	for (int i=0;i<numRtargets;i++)
	{
		if (rtargets[i] == rt)
		{
			rtargets[i] = rtargets[numRtargets-1];
			numRtargets--;
			return;
		}		
	}
	
	for (int i=0;i<numDepths;i++)
	{
		if (depths[i] == rt)
		{
			depths[i] = depths[numDepths-1];
			numDepths--;
			return;
		}		
	}	
#endif
}
*/

void RenderServiceD3D11::MakeScreenshot(std::string& out_image)
{
	if (use_dummy_render) return;

	//en_core.RenderFrame();

	/*char str[512];	
	String::Printf(str, 512,"%s/Screenshots", en_core.EN_Files()->GetAppDirectory());

	CreateDirectory(str, NULL);

	en_core.EN_Files()->GetAppDirectory();
	
	if (out_image.size() == 0)
	{
		String::Printf(str, 512,"%s/Screenshots/Screen_%i_%i_%i_%i_%i_%i.png", en_core.EN_Files()->GetAppDirectory(), GetTime(5),GetTime(4),GetTime(3),GetTime(0),GetTime(1),GetTime(2));
	}
	else
	{
		String::Printf(str, 512,"%s/Screenshots/%s.png", en_core.EN_Files()->GetAppDirectory(), out_image.c_str());
	}

	IDirect3DSurface9* pBack;
	device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBack);

	D3DXSaveSurfaceToFile( str, D3DXIFF_PNG , pBack, NULL, NULL);		

	pBack->Release();

	out_image = str;*/
}