
#pragma once

#include "Core/Render/Technique/RenderTechnique.h"

class RT_NGUI: public RenderTechnique
{	
public:

	virtual void LoadVS();
	virtual void LoadPS();
	virtual void ApplyParams();	
	virtual void PostApplyParams();	
};