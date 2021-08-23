
#include "RT_NGUI_Mask.h"
#include "Core/Core.h"
	
RTECHNIQUE(RT_NGUI_Mask, "NGUIMASK")

void RT_NGUI_Mask::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("ngui_mask_vs.bvs",_FL_);
}

void RT_NGUI_Mask::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("ngui_mask_ps.bps",_FL_);	
}

void RT_NGUI_Mask::ApplyParams()
{	
	RenderTechnique::ApplyParams();

	en_core.Render()->State()->SetCulling(RenderService::cullNone);
	en_core.Render()->State()->SetDepthTest(0);	
	en_core.Render()->State()->SetAlphaBlend(1);

	en_core.Render()->State()->SetBlendFunc(RenderService::argSrcAlpha,RenderService::argInvSrcAlpha);
}

void RT_NGUI_Mask::PostApplyParams()
{
	RenderTechnique::PostApplyParams();
	en_core.Render()->State()->SetDepthTest(1);	
	en_core.Render()->State()->SetAlphaBlend(0);
}