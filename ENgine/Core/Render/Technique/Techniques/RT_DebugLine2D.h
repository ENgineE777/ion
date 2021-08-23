
#pragma once

#include "Core/Render/Technique/RenderTechnique.h"

class RT_DebugLine2D: public RenderTechnique
{	
public:

	virtual void LoadVS();
	virtual void LoadPS();
	virtual void ApplyParams();	
};