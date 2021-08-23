
#include "RTM_DiffusePxL.h"
#include "Core/Core.h"
	
RTECHNIQUE(RTM_DiffusePxL, "RTM_DiffusePxL")

void RTM_DiffusePxL::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("mdl_pxl_vs.bvs", _FL_);
}

void RTM_DiffusePxL::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("mdl_pxl_ps.bps", _FL_);
}

void RTM_DiffusePxL::ApplyParams()
{
	RenderTechnique::ApplyParams();

#ifdef PC
	en_core.Render()->State()->SetCulling(RenderService::cullCW);	
#else
	en_core.Render()->State()->SetCulling(RenderService::cullCCW);	
#endif
	
	en_core.Render()->State()->SetAlphaTest(1);
}

void RTM_DiffusePxL::PostApplyParams()
{
	RenderTechnique::PostApplyParams();
	en_core.Render()->State()->SetAlphaTestFunc(RenderService::cmpGreaterqual, 128);
	en_core.Render()->State()->SetAlphaTest(0);
}