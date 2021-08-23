
#include "RT_Font.h"
#include "Core/Core.h"
	
RTECHNIQUE(RT_Font, "Font")

void RT_Font::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("font_vs.bvs",_FL_);
}

void RT_Font::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("font_ps.bps",_FL_);	
}

void RT_Font::ApplyParams()
{	
	RenderTechnique::ApplyParams();

	en_core.Render()->State()->SetCulling(RenderService::cullNone);	
	en_core.Render()->State()->SetDepthTest(0);	
	en_core.Render()->State()->SetAlphaBlend(1);
	en_core.Render()->State()->SetBlendFunc(RenderService::argSrcAlpha,RenderService::argInvSrcAlpha);
}

void RT_Font::PostApplyParams()
{
	RenderTechnique::PostApplyParams();
	en_core.Render()->State()->SetDepthTest(1);	
	en_core.Render()->State()->SetAlphaBlend(0);
}