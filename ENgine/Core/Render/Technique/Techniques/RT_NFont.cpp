
#include "RT_NFont.h"
#include "Core/Core.h"
	
RTECHNIQUE(RT_NFont, "NFont")

void RT_NFont::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("nfont_vs.bvs",_FL_);
}

void RT_NFont::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("nfont_ps.bps",_FL_);	
}

void RT_NFont::ApplyParams()
{	
	RenderTechnique::ApplyParams();

	en_core.Render()->State()->SetCulling(RenderService::cullNone);	
	en_core.Render()->State()->SetDepthTest(0);	
	en_core.Render()->State()->SetAlphaBlend(1);
	en_core.Render()->State()->SetBlendFunc(RenderService::argSrcAlpha,RenderService::argInvSrcAlpha);
}

void RT_NFont::PostApplyParams()
{
	RenderTechnique::PostApplyParams();
	en_core.Render()->State()->SetDepthTest(1);	
	en_core.Render()->State()->SetAlphaBlend(0);
}