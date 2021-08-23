
#include "RT_DebugLine.h"
#include "Core/Core.h"
	
RTECHNIQUE(RT_DebugLine, "DebugLine")

void RT_DebugLine::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("line_vs.bvs", _FL_);
}

void RT_DebugLine::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("line_ps.bps", _FL_);
}

void RT_DebugLine::ApplyParams()
{
	RenderTechnique::ApplyParams();

	en_core.Render()->State()->SetDepthFunc(RenderService::cmpLessEqual);
}