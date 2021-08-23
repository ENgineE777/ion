
#pragma once

#include "core/Render/Technique/RenderTechnique.h"

class RTM_DiffuseBlend: public RenderTechnique
{	
public:

	virtual void LoadVS();
	virtual void LoadPS();
	virtual void ApplyParams();	
	virtual void PostApplyParams();	
};