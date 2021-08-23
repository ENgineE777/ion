
#include "RT_UI.h"
#include "Core/Core.h"
	
RTECHNIQUE(RT_UI, "UI")

void RT_UI::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("ui_vs.bvs",_FL_);
}

void RT_UI::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("ui_ps.bps",_FL_);	
}

void RT_UI::ApplyParams()
{	
	RenderTechnique::ApplyParams();

	en_core.Render()->State()->SetCulling(RenderService::cullNone);	
	en_core.Render()->State()->SetDepthTest(0);	
	en_core.Render()->State()->SetAlphaBlend(1);
	en_core.Render()->State()->SetBlendFunc(RenderService::argSrcAlpha,RenderService::argInvSrcAlpha);
}

void RT_UI::PostApplyParams()
{
	RenderTechnique::PostApplyParams();
	en_core.Render()->State()->SetDepthTest(1);	
	en_core.Render()->State()->SetAlphaBlend(0);
}