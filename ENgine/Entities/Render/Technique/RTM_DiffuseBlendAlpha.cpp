
#include "RTM_DiffuseBlendAlpha.h"
#include "Core/Core.h"
	
RTECHNIQUE(RTM_DiffuseBlendAlpha, "RTM_DiffuseBlendAlpha")

void RTM_DiffuseBlendAlpha::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("mdl_diffuse_vs.bvs", _FL_);
}

void RTM_DiffuseBlendAlpha::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("mdl_diffuse_ps.bps", _FL_);
}

void RTM_DiffuseBlendAlpha::ApplyParams()
{
	RenderTechnique::ApplyParams();

#ifdef PC
	en_core.Render()->State()->SetCulling(RenderService::cullCW);	
#else
	en_core.Render()->State()->SetCulling(RenderService::cullCCW);	
#endif
	en_core.Render()->State()->SetAlphaTest(1);
	en_core.Render()->State()->SetDepthWriting(0);
	en_core.Render()->State()->SetAlphaBlend(1);
	en_core.Render()->State()->SetBlendFunc(RenderService::argSrcAlpha,RenderService::argInvSrcAlpha);
}

void RTM_DiffuseBlendAlpha::PostApplyParams()
{
	RenderTechnique::PostApplyParams();
	en_core.Render()->State()->SetAlphaTestFunc(RenderService::cmpGreaterqual, 128);
	en_core.Render()->State()->SetAlphaTest(0);
	en_core.Render()->State()->SetAlphaBlend(0);
	en_core.Render()->State()->SetDepthWriting(1);
}