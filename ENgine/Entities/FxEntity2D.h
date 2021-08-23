#pragma once

#include "Core/core.h"
#include "Entities/GUI/GUILabel.h"
#include "Core/Fx/Fx.h"

class FxEntity2D : public GUILabel
{	
	Fx* m_fx;

public:
	ENTITY_CONSTRUCT(FxEntity2D,GUILabel)
		ENTITY_SCRIPTDEC

	FILENAME_PROP(FxEntity2D, propResPath, "", "resPath", "*.fx");	

	virtual void Init();

	virtual void ReInit();

	virtual void Start();	

	virtual void Work(float dt, int level);

	virtual void RenderWidget();	

};
