
#include "RTM_Atmo.h"
#include "Core/Core.h"
	
RTECHNIQUE(RTM_Atmo, "RTM_Atmo")

void RTM_Atmo::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("mdl_atmo_vs.bvs", _FL_);
}

void RTM_Atmo::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("mdl_atmo_ps.bps", _FL_);
}

void RTM_Atmo::ApplyParams()
{
	RenderTechnique::ApplyParams();
	en_core.Render()->State()->SetCulling(RenderService::cullNone);	
	en_core.Render()->State()->SetDepthWriting(0);
	en_core.Render()->State()->SetAlphaBlend(1);
	en_core.Render()->State()->SetBlendFunc(RenderService::argOne,RenderService::argOne);
}

void RTM_Atmo::PostApplyParams()
{
	RenderTechnique::PostApplyParams();
	en_core.Render()->State()->SetAlphaTestFunc(RenderService::cmpGreaterqual, 128);	
	en_core.Render()->State()->SetAlphaBlend(0);
	en_core.Render()->State()->SetDepthWriting(1);
}