
#pragma once

#include "core/Render/Technique/RenderTechnique.h"

class RTM_Diffuse: public RenderTechnique
{	
public:

	virtual void LoadVS();
	virtual void LoadPS();
	virtual void ApplyParams();	
	virtual void PostApplyParams();	
};

class RTM_DiffuseAnim: public RTM_Diffuse
{
public:

	virtual void LoadVS();
};