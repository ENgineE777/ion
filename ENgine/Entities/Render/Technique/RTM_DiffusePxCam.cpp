
#include "RTM_DiffusePxCam.h"
#include "Core/Core.h"
	
RTECHNIQUE(RTM_DiffusePxCam, "RTM_DiffusePxCam")

void RTM_DiffusePxCam::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("mdl_atmo_vs.bvs", _FL_);
}

void RTM_DiffusePxCam::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("mdl_pxl_cam_ps.bps", _FL_);
}

void RTM_DiffusePxCam::ApplyParams()
{
	RenderTechnique::ApplyParams();
	
#ifdef PC
	en_core.Render()->State()->SetCulling(RenderService::cullCW);	
#else
	en_core.Render()->State()->SetCulling(RenderService::cullCCW);	
#endif	
}

void RTM_DiffusePxCam::PostApplyParams()
{
	RenderTechnique::PostApplyParams();	
}