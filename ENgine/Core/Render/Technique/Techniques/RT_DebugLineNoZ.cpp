
#include "RT_DebugLineNoZ.h"
#include "Core/Core.h"
	
RTECHNIQUE(RT_DebugLineNoZ, "DebugLineNoZ")

void RT_DebugLineNoZ::ApplyParams()
{
	RT_DebugLine::ApplyParams();

	en_core.Render()->State()->SetDepthTest(0);		
	en_core.Render()->State()->SetDepthWriting(0);
	en_core.Render()->State()->SetDepthFunc(RenderService::cmpAlways);
}

void RT_DebugLineNoZ::PostApplyParams()
{
	RT_DebugLine::PostApplyParams();

	en_core.Render()->State()->SetDepthTest(1);	
	en_core.Render()->State()->SetDepthWriting(1);
	en_core.Render()->State()->SetDepthFunc(RenderService::cmpLessEqual);
}