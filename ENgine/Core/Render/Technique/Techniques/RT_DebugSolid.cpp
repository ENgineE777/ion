
#include "RT_DebugSolid.h"
#include "Core/Core.h"
	
RTECHNIQUE(RT_DebugSolid, "DebugSolid")

void RT_DebugSolid::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("solid_vs.bvs", _FL_);
}

void RT_DebugSolid::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("solid_ps.bps", _FL_);
}

void RT_DebugSolid::ApplyParams()
{
	RenderTechnique::ApplyParams();

	en_core.Render()->State()->SetCulling(RenderService::cullNone);
	//en_core.Render()->SetAlphaBlend(1);
	//en_core.Render()->SetBlendFunc(RenderService::argSrcAlpha, RenderService::argInvSrcAlpha);
	//en_core.Render()->SetDepthFunc(RenderService::cmpAlways);
}

void RT_DebugSolid::PostApplyParams()
{
	RenderTechnique::PostApplyParams();

	//en_core.Render()->SetAlphaBlend(0);	
}