
#include "RTM_Diffuse.h"
#include "Core/Core.h"
	
RTECHNIQUE(RTM_Diffuse, "RTM_Diffuse")
RTECHNIQUE(RTM_DiffuseAnim, "RTM_DiffuseAnim")

void RTM_Diffuse::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("mdl_diffuse_vs.bvs", _FL_);
}

void RTM_Diffuse::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("mdl_diffuse_ps.bps", _FL_);
}

void RTM_Diffuse::ApplyParams()
{
	RenderTechnique::ApplyParams();
	
#ifdef PC
	en_core.Render()->State()->SetCulling(RenderService::cullCW);	
#else
	en_core.Render()->State()->SetCulling(RenderService::cullCCW);	
#endif
	
	en_core.Render()->State()->SetAlphaTest(0);
}

void RTM_Diffuse::PostApplyParams()
{
	RenderTechnique::PostApplyParams();
	en_core.Render()->State()->SetAlphaTestFunc(RenderService::cmpGreaterqual, 128);
	en_core.Render()->State()->SetAlphaTest(0);
}

void RTM_DiffuseAnim::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("mdl_diffuse_anim_vs.bvs", _FL_);
}