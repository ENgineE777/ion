
#include "RT_NGUI_A.h"
#include "Core/Core.h"
	
RTECHNIQUE(RT_NGUI_A, "NGUIA")

void RT_NGUI_A::LoadPS()
{
	pshader = en_core.Render()->CreatePixelShader("ngui_ps_alpha.bps",_FL_);
}