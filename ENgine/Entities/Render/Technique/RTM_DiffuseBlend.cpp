
#include "RTM_DiffuseBlend.h"
#include "Core/Core.h"
	
RTECHNIQUE(RTM_DiffuseBlend, "RTM_DiffuseBlend")

void RTM_DiffuseBlend::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("mdl_diffuse_vs.bvs", _FL_);
}

void RTM_DiffuseBlend::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("mdl_diffuse_ps.bps", _FL_);
}

void RTM_DiffuseBlend::ApplyParams()
{
	RenderTechnique::ApplyParams();

#ifdef PC
	en_core.Render()->State()->SetCulling(RenderService::cullCW);	
#else
	en_core.Render()->State()->SetCulling(RenderService::cullCCW);	
#endif
	
	en_core.Render()->State()->SetDepthWriting(0);
	en_core.Render()->State()->SetAlphaBlend(1);
	en_core.Render()->State()->SetBlendFunc(RenderService::argOne,RenderService::argOne);
}

void RTM_DiffuseBlend::PostApplyParams()
{
	RenderTechnique::PostApplyParams();	
	en_core.Render()->State()->SetAlphaBlend(0);
	en_core.Render()->State()->SetDepthWriting(1);
}