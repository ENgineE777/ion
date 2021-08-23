
#pragma once

#include "RT_DebugLine.h"

class RT_DebugLineNoZ: public RT_DebugLine
{	
public:
	
	virtual void ApplyParams();
	virtual void PostApplyParams();
};