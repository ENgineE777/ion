
#include "RT_DebugLine2D.h"
#include "Core/Core.h"
	
RTECHNIQUE(RT_DebugLine2D, "DebugLine2D")

void RT_DebugLine2D::LoadVS()
{
	vshader = en_core.Render()->CreateVertexShader("line2d_vs.bvs", _FL_);
}

void RT_DebugLine2D::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("line_ps.bps", _FL_);
}

void RT_DebugLine2D::ApplyParams()
{
	RenderTechnique::ApplyParams();

	en_core.Render()->State()->SetDepthFunc(RenderService::cmpLessEqual);
}