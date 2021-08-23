
#pragma once

#include "Core/Render/Technique/RenderTechnique.h"

class RT_NFont: public RenderTechnique
{	
public:

	virtual void LoadVS();
	virtual void LoadPS();
	virtual void ApplyParams();	
	virtual void PostApplyParams();	
};