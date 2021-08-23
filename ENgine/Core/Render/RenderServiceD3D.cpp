
#include "RenderServiceD3D.h"
#include "Core/Core.h"

#include "Geometry/VertexBufferD3D.h"
#include "Geometry/IndexBufferD3D.h"
#include "Shaders/VertexShaderD3D.h"
#include "Shaders/PixelShaderD3D.h"
#include "Textures/TextureD3D.h"

#define SET_STATE(state, enable) \
if (RenderServiceD3D::instance->device)\
{dword state_statuc;\
RenderServiceD3D::instance->device->GetRenderState(state, &state_statuc);\
if (state_statuc != enable)\
{\
	RenderServiceD3D::instance->device->SetRenderState(state, enable);\
}}

RenderServiceD3D::StateManagerD3D::StateManagerD3D()
{
	clipInfo = 0;
}

void RenderServiceD3D::StateManagerD3D::SetAlphaTest(dword enable)
{
	SET_STATE(D3DRS_ALPHATESTENABLE, enable)
}

void RenderServiceD3D::StateManagerD3D::SetAlphaTestFunc(CompareFunc func, dword ref)
{
	SET_STATE(D3DRS_ALPHAFUNC, func)
	SET_STATE(D3DRS_ALPHAREF, ref)
}

void RenderServiceD3D::StateManagerD3D::SetAlphaBlend(dword enable)
{
	SET_STATE(D3DRS_ALPHABLENDENABLE, enable)
}

void RenderServiceD3D::StateManagerD3D::SetBlendFunc(BlendArg src, BlendArg dest)
{
	SET_STATE(D3DRS_SRCBLEND, src)
	SET_STATE(D3DRS_DESTBLEND, dest)	
}

void RenderServiceD3D::StateManagerD3D::SetBlendOperation(BlendOp op)
{
	SET_STATE(D3DRS_BLENDOP, op)	
}
	
void RenderServiceD3D::StateManagerD3D::SetDepthTest(dword enable)
{
	SET_STATE(D3DRS_ZENABLE, enable)
}

void RenderServiceD3D::StateManagerD3D::SetDepthWriting(dword enable)
{
	if (!RenderServiceD3D::instance->device) return;
	
	RenderServiceD3D::instance->device->SetRenderState(D3DRS_ZWRITEENABLE, enable);
}

void RenderServiceD3D::StateManagerD3D::SetDepthFunc(CompareFunc func)
{
	SET_STATE(D3DRS_ZFUNC, func)	
}

void RenderServiceD3D::StateManagerD3D::SetCulling(CullMode mode)
{
	SET_STATE(D3DRS_CULLMODE, mode)	
}

void RenderServiceD3D::StateManagerD3D::SetStencilTest(dword enable)
{
	SET_STATE(D3DRS_STENCILENABLE, enable)
}

void RenderServiceD3D::StateManagerD3D::SetStencilTestFunc(CompareFunc func, dword ref, dword mask)
{
	SET_STATE(D3DRS_STENCILFUNC, func)	
	SET_STATE(D3DRS_STENCILREF, ref)
	SET_STATE(D3DRS_STENCILMASK, mask)	
}

void RenderServiceD3D::StateManagerD3D::SetStencilWriteMask(dword mask)
{
	SET_STATE(D3DRS_STENCILWRITEMASK, mask)	
}

void RenderServiceD3D::StateManagerD3D::SetStencilOperation(StencilOp failOp, StencilOp zFailOp, StencilOp zPass)
{
	SET_STATE(D3DRS_STENCILFAIL, failOp)	
	SET_STATE(D3DRS_STENCILZFAIL, zFailOp)	
	SET_STATE(D3DRS_STENCILPASS, zPass)
}

void RenderServiceD3D::StateManagerD3D::SetColorWrite(bool red, bool green, bool blue, bool alpha)
{
	if (!RenderServiceD3D::instance->device) return;

	dword clrWrite = 0;
	
	if (red)   clrWrite |= D3DCOLORWRITEENABLE_RED;
	if (green) clrWrite |= D3DCOLORWRITEENABLE_GREEN;
	if (blue)  clrWrite |= D3DCOLORWRITEENABLE_BLUE;
	if (alpha) clrWrite |= D3DCOLORWRITEENABLE_ALPHA;
	
	//SET_STATE(D3DRS_COLORWRITEENABLE, clrWrite)	
	RenderServiceD3D::instance->device->SetRenderState(D3DRS_COLORWRITEENABLE, clrWrite);
}

void RenderServiceD3D::StateManagerD3D::SetClipPlane(dword clipIndex, dword enable)
{
	if (!RenderServiceD3D::instance->device) return;

	if (enable > 0)
	{
		clipInfo |= (1 << clipIndex); 
	}
	else
	{
		clipInfo &= ~(1 << clipIndex);
	}

	RenderServiceD3D::instance->device->SetRenderState(D3DRS_CLIPPLANEENABLE, clipInfo);
}

void RenderServiceD3D::StateManagerD3D::SetWireFrameMode(dword enable)
{
	if (!RenderServiceD3D::instance->device) return;

	if(enable)
	{
		RenderServiceD3D::instance->device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else
	{
		RenderServiceD3D::instance->device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
}
	
void RenderServiceD3D::StateManagerD3D::SetSamplerState(dword sampler, SamplerState state, dword value)
{
	if (!RenderServiceD3D::instance->device) return;

	dword state_status;
	RenderServiceD3D::instance->device->GetSamplerState(sampler, (D3DSAMPLERSTATETYPE)state, &state_status);
	if (state_status != value)
	{
		RenderServiceD3D::instance->device->SetSamplerState(sampler, (D3DSAMPLERSTATETYPE)state, value);
	}
}
	
void RenderServiceD3D::StateManagerD3D::SetupClipPlane(dword index, Vector4 plane)
{
	if (!RenderServiceD3D::instance->device) return;

	RenderServiceD3D::instance->device->SetClipPlane(index, &plane.v4[0]); 
}

void RenderServiceD3D::StateManagerD3D::SetupSlopeZBias(bool enable, float slopeZBias, float depthOffset)
{
	if (!RenderServiceD3D::instance->device) return;

	if (enable)
	{
		float curDepthBias = depthOffset;
		float curBiasSlope = slopeZBias;
		RenderServiceD3D::instance->device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD *)&curDepthBias);
		RenderServiceD3D::instance->device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD *)&curBiasSlope);
	}
	else
	{
		float curDepthBias = 0;
		float curBiasSlope = 0;
		RenderServiceD3D::instance->device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD *)&curDepthBias);
		RenderServiceD3D::instance->device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD *)&curBiasSlope);
	}
}	

void RenderServiceD3D::StateManagerD3D::SetScissor(dword enable)
{
	SET_STATE(D3DRS_SCISSORTESTENABLE, enable)	
}

void RenderServiceD3D::StateManagerD3D::SetScissorRect(const Rect& rect)
{
	if (!RenderServiceD3D::instance->device) return;

	RECT rc;
	rc.left = rect.left;
	rc.top = rect.top;
	rc.right = rect.right;
	rc.bottom = rect.bottom;	

	RenderServiceD3D::instance->device->SetScissorRect(&rc);
}

void RenderServiceD3D::StateManagerD3D::GetScissorRect(Rect& rect)
{
	if (!RenderServiceD3D::instance->device) return;

	RECT rc;
	RenderServiceD3D::instance->device->GetScissorRect(&rc);

	rect.left = rc.left;
	rect.top = rc.top;
	rect.right = rc.right;
	rect.bottom = rc.bottom;
}

RenderServiceD3D::DisplayManagerD3D::DisplayManagerD3D() : DisplayManager()
{
}

void RenderServiceD3D::DisplayManagerD3D::ApplyVideoMode(VideoMode mode)
{	
	width = mode.width;
	height = mode.height;
	core->TraceTo("Render", "RenderServiceD3D::ApplyVideoMode: %d x %d", width, height);
		
	aspectRatio = (float)width / (float)height;	

	instance->Reset();

	DisplayManager::ApplyVideoMode(mode);
}
				
void RenderServiceD3D::DisplayManagerD3D::SetViewport(const Viewport& viewport)
{
	if (!RenderServiceD3D::instance->device) return;

	D3DVIEWPORT9 d3dviewport;
	d3dviewport.X = viewport.x;
	d3dviewport.Y = viewport.y;
	d3dviewport.Width = viewport.width;
	d3dviewport.Height = viewport.height;
	d3dviewport.MinZ = viewport.minZ;
	d3dviewport.MaxZ = viewport.maxZ;

	RenderServiceD3D::instance->device->SetViewport(&d3dviewport);
}

void RenderServiceD3D::DisplayManagerD3D::GetViewport(Viewport& viewport)
{
	if (!RenderServiceD3D::instance->device) return;

	D3DVIEWPORT9 d3dviewport;
	RenderServiceD3D::instance->device->GetViewport(&d3dviewport);

	viewport.x = d3dviewport.X;
	viewport.y = d3dviewport.Y;
	viewport.width = d3dviewport.Width;
	viewport.height = d3dviewport.Height;
	viewport.minZ = d3dviewport.MinZ;
	viewport.maxZ = d3dviewport.MaxZ;
}

RenderServiceD3D* RenderServiceD3D::instance = null;

RenderServiceD3D::RenderServiceD3D() : RenderService()
{	
	instance = this;

	last_vbuffer = null;
	last_ibuffer = null;	

	need_update_rt = false;

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

	need_reset = false;
	need_post_reset = false;

	miltisample = D3DMULTISAMPLE_NONE;
}

bool RenderServiceD3D::Init(int wgt, int hgt)
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

	D3DPRESENT_PARAMETERS	pparams;
	UINT					adapter;
	D3DDEVTYPE				devtype;
	HRESULT					hr;

	char errorMessage[256];
	
	d3d = Direct3DCreate9( D3D_SDK_VERSION );
	if ( !d3d )
	{
		core->TraceTo("Render", "[%s] Direct3DCreate9() failed", __FUNCTION__ );

		String::Printf(errorMessage,256, "Direct3DCreate9 failed");
		MessageBoxA(NULL, errorMessage , "ERROR", NULL);
		return 0;
	}
	
	D3DDISPLAYMODE displayMode;
	
	dword  numDisplayModes = d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
		
	D3DDISPLAYMODE enumDisplayMode;

	for (dword adapterMode = 0; adapterMode < numDisplayModes; adapterMode++)
	{
		if (FAILED(d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, adapterMode, &enumDisplayMode)))
		{
			core->TraceTo("Render", "Unable to enum adapter modes");				
			break;
		}

		if (!display_manager.numModes)
		{
			display_manager.modes[display_manager.numModes].width	= enumDisplayMode.Width;
			display_manager.modes[display_manager.numModes].height = enumDisplayMode.Height;
			display_manager.numModes++;
			continue;
		}

		if ((display_manager.modes[display_manager.numModes-1].width == enumDisplayMode.Width) &&
			(display_manager.modes[display_manager.numModes-1].height == enumDisplayMode.Height))
		{
			continue;
		}

		bool prevAbove = (display_manager.modes[display_manager.numModes-1].width < enumDisplayMode.Width) ||
						  ((display_manager.modes[display_manager.numModes-1].width == enumDisplayMode.Width) &&
						  (display_manager.modes[display_manager.numModes-1].height < enumDisplayMode.Height));

		if (prevAbove)
		{
			display_manager.modes[display_manager.numModes].width  = enumDisplayMode.Width;
			display_manager.modes[display_manager.numModes].height = enumDisplayMode.Height;
			display_manager.numModes++;			
			continue;
		}

		bool curAbove = prevAbove;

		int uniqueAdapterMode;
		
		for (uniqueAdapterMode = (int)display_manager.numModes - 2; (0 <= uniqueAdapterMode); uniqueAdapterMode--)
		{
			curAbove = (display_manager.modes[uniqueAdapterMode].width < enumDisplayMode.Width) ||
						((display_manager.modes[uniqueAdapterMode].width == enumDisplayMode.Width) &&
						(display_manager.modes[uniqueAdapterMode].height <= enumDisplayMode.Height));

			if (curAbove != prevAbove)
			{
				break;
			}

			prevAbove = curAbove;
		}

		if ((0 <= uniqueAdapterMode) &&
			(display_manager.modes[uniqueAdapterMode].width == enumDisplayMode.Width) &&
			(display_manager.modes[uniqueAdapterMode].height == enumDisplayMode.Height))
		{
			continue;
		}

		int shiftedAdapterMode;
		
		for (shiftedAdapterMode = (display_manager.numModes - 1); (shiftedAdapterMode > uniqueAdapterMode); shiftedAdapterMode--)
		{
			display_manager.modes[shiftedAdapterMode+1] = display_manager.modes[shiftedAdapterMode];
		}

		display_manager.modes[shiftedAdapterMode+1].width = enumDisplayMode.Width;
		display_manager.modes[shiftedAdapterMode+1].height = enumDisplayMode.Height;
		display_manager.numModes++;
	}

	// NVPerfHUD check
	adapter = D3DADAPTER_DEFAULT;
	devtype = D3DDEVTYPE_HAL;
	
	display_manager.cur_mode = -1;
	
	for (int i=0;i<display_manager.numModes-1;i++)
	{
		for (int j=i+1;j<display_manager.numModes;j++)
		{
			if (display_manager.modes[i].width > display_manager.modes[j].width ||
				(display_manager.modes[i].width == display_manager.modes[j].width &&
				 display_manager.modes[i].height > display_manager.modes[j].height))
			{
				VideoMode tmp = display_manager.modes[i];
				display_manager.modes[i] = display_manager.modes[j];
				display_manager.modes[j] = tmp;
			}
		}
	}
	
	for (int i=0;i<display_manager.numModes;i++)
	{
		if (display_manager.modes[i].width == display_manager.width &&
			display_manager.modes[i].height == display_manager.height)
		{
			display_manager.cur_mode = i;
			break;
		}
	}	

	for (int  i = 0; i < d3d->GetAdapterCount(); i++ )
	{
		D3DADAPTER_IDENTIFIER9	id;

		d3d->GetAdapterIdentifier( i, 0, &id );
		
		core->TraceTo("Render",id.Description);

		if ( strstr( id.Description, "PerfHUD" ) )
		{
			adapter = i;
			devtype = D3DDEVTYPE_REF;
			break;
		}
	}	
	
	memset( &pparams, 0, sizeof( pparams ) );

	pparams.hDeviceWindow = en_core.GetHWND();

	pparams.Windowed = true;

	if (pparams.Windowed)
	{
		pparams.BackBufferFormat = D3DFMT_X8R8G8B8;
	}
	else
	{
		pparams.BackBufferFormat = D3DFMT_X8R8G8B8;
	}

	pparams.BackBufferWidth = display_manager.width;
	pparams.BackBufferHeight = display_manager.height;	
	pparams.BackBufferCount = 1;
	pparams.BackBufferFormat = D3DFMT_X8R8G8B8;
	pparams.MultiSampleType = (D3DMULTISAMPLE_TYPE)miltisample;
	pparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pparams.EnableAutoDepthStencil = true;
	pparams.AutoDepthStencilFormat = D3DFMT_D24S8;
	pparams.Flags = 0;
	pparams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	
	pparams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;	
	
	hr = d3d->CreateDevice( adapter, devtype, pparams.hDeviceWindow,
						    D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &pparams, &device );
	
	if ( FAILED( hr ) )
	{
		miltisample = D3DMULTISAMPLE_2_SAMPLES;
		pparams.MultiSampleType = (D3DMULTISAMPLE_TYPE)miltisample;
		hr = d3d->CreateDevice( adapter, devtype, pparams.hDeviceWindow,
								D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &pparams, &device );

		if ( FAILED( hr ) )
		{
			miltisample = D3DMULTISAMPLE_NONE;
			pparams.MultiSampleType = (D3DMULTISAMPLE_TYPE)miltisample;
			hr = d3d->CreateDevice( adapter, devtype, pparams.hDeviceWindow,
									D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &pparams, &device );
		}

	}

	if ( FAILED (hr ))
	{
		miltisample = D3DMULTISAMPLE_NONE;
		pparams.MultiSampleType = (D3DMULTISAMPLE_TYPE)miltisample;		
		core->TraceTo("Render", "Trying create nonmultithreaded device. Render may be unstable");
		hr = d3d->CreateDevice( adapter, devtype, pparams.hDeviceWindow,
								D3DCREATE_HARDWARE_VERTEXPROCESSING , &pparams, &device );
		if(FAILED(hr))
		{			
			core->TraceTo("Render", "Create nonmultithreaded d3d device failed, hr = %u", hr );
		}
	}

	if ( FAILED( hr ) )
	{
		core->TraceTo("Render", "Create d3d device failed, hr = %u", hr );

		String::Printf(errorMessage,256, "CreateDevice failed. hr = 0x%x", hr);
		MessageBoxA(NULL, errorMessage , "ERROR", NULL);
		return false;
	}
	
	D3DDISPLAYMODE d3ddm;

	if(FAILED( d3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm )))
	{
		core->TraceTo("Render", "Error at GetAdapterDisplayMode");

		String::Printf(errorMessage,256, "GetAdapterDisplayMode failed");
		MessageBoxA(NULL, errorMessage , "ERROR", NULL);

		return false;
	}

	display_manager.aspectRatio = (float)display_manager.width / (float)display_manager.height;			

	cur_depth = 0;
	device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer[cur_depth]);	
	device->GetDepthStencilSurface(&pZStencilSurface[cur_depth]);
	
	core->TraceTo("Render", "Direct3D Device is successfully created (%i x %i). OK", display_manager.width, display_manager.height);

	RenderService::Init(wgt, hgt);		

	return true;
}

bool RenderServiceD3D::Reset()
{
	if (use_dummy_render) return true;

	en_core.Font()->Reset();	

	RELEASE(pBackBuffer[0]);
	RELEASE(pZStencilSurface[0]);

	for (int i=0;i<16;i++) used_textures[i] = 0;

	D3DPRESENT_PARAMETERS	pparams;

	memset( &pparams, 0, sizeof( pparams ) );

	pparams.hDeviceWindow = en_core.GetHWND();

	pparams.Windowed = true;

	pparams.BackBufferFormat = D3DFMT_X8R8G8B8;	
		
	if (pparams.Windowed)
	{
		RECT rect;
		GetClientRect(en_core.GetHWND(), &rect);

		display_manager.width = rect.right;
		display_manager.height = rect.bottom;			
	}
	
	pparams.BackBufferWidth = display_manager.width;
	pparams.BackBufferHeight = display_manager.height;		

	pparams.BackBufferCount = 1;
	pparams.BackBufferFormat = D3DFMT_X8R8G8B8;
	pparams.MultiSampleType = (D3DMULTISAMPLE_TYPE)miltisample;
	pparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pparams.EnableAutoDepthStencil = true;
	pparams.AutoDepthStencilFormat = D3DFMT_D24S8;
	pparams.Flags = 0;
	pparams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	pparams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	

	HRESULT hr = device->Reset(&pparams);

	if(FAILED(hr))
	{
		core->TraceTo("Render", "Error. Trying to reset device is failed");
		return false;
	}
	else
	{
		core->TraceTo("Render", "Device has resetted");
	}

	int rtWidth = 1;
	int rtHeight  = 1;
	int dsWidth  = 1;
	int dsHeight  = 1;	

	cur_depth = 0;
	device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer[cur_depth]);	
	device->GetDepthStencilSurface(&pZStencilSurface[cur_depth]);

	need_reset = false;
	need_post_reset = true;		

	return true;
}

void RenderServiceD3D::Begin()
{
	if (use_dummy_render) return;

	HRESULT hr = device->TestCooperativeLevel();
	
	if (D3DERR_DEVICELOST == hr)
	{		
		need_reset = true;
		return;
	}

	if (hr == D3DERR_DEVICENOTRESET)
	{				
		if(Reset() == false)
		{
			return;
		}
	}

	RenderService::Begin();

	device->BeginScene();

	last_ibuffer = null;
	last_vbuffer = null;	
	
	last_decl = null;

	need_update_rt = false;

	for (int i=0;i<16;i++) used_textures[i] = 0;	

	vshader = null;
	pshader = null;	

	vertex_seted = false;

#ifdef EDITOR
	Clear(true, COLOR_GRAY, true, 1.0f, false, 0);	
#else
	Clear(true, COLOR_BLACK, true, 1.0f, false, 0);	
#endif

	if (!null_render && !use_dummy_render)
	{
		task_manager.Execute();
	}

	device->EndScene();

	if (need_post_reset)
	{
		en_core.Font()->Restore();		
		
		need_post_reset = false;
	}		
}

void RenderServiceD3D::End()
{
	if (use_dummy_render) return;

	if (need_reset) return;

	device->Present(null,null,null,null);

	RenderService::End();
}

void RenderServiceD3D::SetTexture(int index, Texture* texture)
{	
	if (use_dummy_render) return;

	IDirect3DTexture9* tex = NULL;
	if (texture) tex = (IDirect3DTexture9*)texture->GetData();

	if (used_textures[index] != tex)
	{
		device->SetTexture(index, tex);
		used_textures[index] = tex;
	}
}

Texture* RenderServiceD3D::CreateTexture(int width, int height, int lods, Texture::Format format, const char* file, int line)
{	
	IDirect3DTexture9* texture = NULL;

	if (!use_dummy_render)
	{	
		D3DFORMAT fmt = D3DFMT_A8R8G8B8;

		if (format == Texture::fmt_a8_r8)
		{
			fmt = D3DFMT_A8L8;
		}

		HRESULT hr = device->CreateTexture(width, height, lods, 0, fmt, D3DPOOL_MANAGED, &texture, NULL);	
	}

	if (texture || use_dummy_render)
	{
		return new TextureD3D(width, height, format, lods, texture);		
	}
	else
	{
		core->TraceTo("Render", "Can't create texture. Width: %d, Height: %d, Format: %d", width, height, format);
	}

	return null;
}

void RenderServiceD3D::Clear(bool renderTarget, Color color, bool zbuffer, float zValue, bool stencil, dword stencilValue )
{
	if (use_dummy_render) return;

	dword flags = 0;

	if(renderTarget)
		flags |= D3DCLEAR_TARGET;
	if(zbuffer)
		flags |= D3DCLEAR_ZBUFFER;
	if(stencil)
		flags |= D3DCLEAR_STENCIL;

	device->Clear(0,0, flags, color.Get(), zValue, stencilValue );
}

void RenderServiceD3D::DrawPrimitive(Primitive type, int startVertex, int primCount)
{
	if (use_dummy_render) return;
	
	cur_tech->UpdateShaderConstants();

	draw_calls++;
	poly_count += primCount;
		
	HRESULT hr = device->DrawPrimitive((D3DPRIMITIVETYPE)GetPrimitiveType(type), startVertex, primCount);	
}

void RenderServiceD3D::DrawIndexedPrimitive(Primitive type, int startIndex, int primCount)
{
	if (use_dummy_render) return;
	
	cur_tech->UpdateShaderConstants();

	if (primCount<1) return;

	draw_calls++;
	poly_count += primCount;    
		
	HRESULT hr = device->DrawIndexedPrimitive((D3DPRIMITIVETYPE)GetPrimitiveType(type), 0,0,last_vbuffer->GetSize()/last_vbuffer->GetStride(), startIndex, primCount);
}

VertexBuffer* RenderServiceD3D::CreateVertexBuffer(dword size,int stride, const char* file, int line)
{	
	VertexBufferD3D* buff = new VertexBufferD3D(size, stride);	

	vbuffers.push_back(buff);

	return buff;
}

void RenderServiceD3D::SetVertexBuffer(int index, VertexBuffer* buffer)
{
	if (use_dummy_render) return;

	if (last_vbuffer == buffer) return;

	IDirect3DVertexBuffer9* vb = null;
	int stride = 0;
	if (buffer)
	{
		vb = (IDirect3DVertexBuffer9*)buffer->GetData();	
		stride = buffer->GetStride();
	}

	device->SetStreamSource(index, vb, 0, stride);	
	vertex_seted = true;

	last_vbuffer = buffer;
}

IndexBuffer* RenderServiceD3D::CreateIndexBuffer(dword size, const char* file, int line)
{
	IndexBufferD3D* buff;

	buff = new IndexBufferD3D(size);	

	ibuffers.push_back(buff);

	return buff;	
}

void RenderServiceD3D::SetIndexBuffer(IndexBuffer* buffer)
{
	if (use_dummy_render) return;

	if (last_ibuffer == buffer) return;

	IDirect3DIndexBuffer9* ib = null;
	if (buffer) ib = (IDirect3DIndexBuffer9*)buffer->GetData();	

	device->SetIndices(ib);
	last_ibuffer = buffer;
}

VertexShader* RenderServiceD3D::CreateVertexShaderImpl(const char* name)
{
	return new VertexShaderD3D(name);
}

void RenderServiceD3D::SetVertexShader(VertexShader* shader)
{
	if (use_dummy_render) return;

	if (vshader == shader) return;

	IDirect3DVertexShader9* shd = NULL;
	if (shader) shd = (IDirect3DVertexShader9*)shader->GetData();

	device->SetVertexShader(shd);
	
	//device->SetVertexShader(null);
	//vertex_seted = true;

	vshader = shader;

	if (vshader) vshader->SetDirty();
}

PixelShader* RenderServiceD3D::CreatePixelShaderImpl(const char* name)
{
	return new PixelShaderD3D(name);
}

void RenderServiceD3D::SetPixelShader(PixelShader* shader)
{
	if (use_dummy_render) return;
	if (pshader == shader) return;

	IDirect3DPixelShader9* shd = NULL;
	if (shader) shd = (IDirect3DPixelShader9*)shader->GetData();

	device->SetPixelShader(shd);


	pshader = shader;
	if(pshader)
		pshader->SetDirty();
}

void RenderServiceD3D::Release()
{	
	RenderService::Release();	
}

void RenderServiceD3D::SetVertexDeclaration(IDirect3DVertexDeclaration9* decl)
{
	if (use_dummy_render) return;
	if (last_decl != decl) device->SetVertexDeclaration(decl);
	last_decl = decl;
}

int RenderServiceD3D::GetPrimitiveType(Primitive type)
{
	int tp = D3DPT_TRIANGLELIST; 
	switch (type)
	{
		case pointsList:
		{
			tp = D3DPT_POINTLIST;
		}
		break;
		case lineStrip:
		{
			tp = D3DPT_LINESTRIP;
		}
		break;
		case linesList:
		{
			tp = D3DPT_LINELIST;
		}
		break;
		case triangleStrip:
		{
			tp = D3DPT_TRIANGLESTRIP;
		}
		break;		
		case trianglesList:
		{
			tp = D3DPT_TRIANGLELIST;
		}
		break;		
	}

	return tp;
}

void RenderServiceD3D::MakeScreenshot(std::string& out_image)
{
	if (use_dummy_render) return;

	//en_core.RenderFrame();

	char str[512];	
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

	out_image = str;
}
