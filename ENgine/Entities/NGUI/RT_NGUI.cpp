
#include "RT_NGUI.h"
#include "Core/Core.h"
	
RTECHNIQUE(RT_NGUI, "NGUI")

void RT_NGUI::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("ngui_vs.bvs",_FL_);
}

void RT_NGUI::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("ngui_ps.bps",_FL_);	
}

void RT_NGUI::ApplyParams()
{	
	RenderTechnique::ApplyParams();

	en_core.Render()->State()->SetCulling(RenderService::cullNone);	
	en_core.Render()->State()->SetDepthTest(0);	
	en_core.Render()->State()->SetAlphaBlend(1);

	en_core.Render()->State()->SetBlendFunc(RenderService::argSrcAlpha,RenderService::argInvSrcAlpha);
}

void RT_NGUI::PostApplyParams()
{
	RenderTechnique::PostApplyParams();
	en_core.Render()->State()->SetDepthTest(1);	
	en_core.Render()->State()->SetAlphaBlend(0);
}