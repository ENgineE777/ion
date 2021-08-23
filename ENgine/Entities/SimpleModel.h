#pragma once

#include "../Core/Core.h"

class SimpleModel : public SceneEntity
{		
public:
		
	FILENAME_PROP(SimpleModel, model_name, "", "model")
	FILENAME_PROP(SimpleModel, anim_name, "", "anim")	

	ModelInstance* model;
	IAnimationInstance* anim;

	IMatrix rot;

	virtual void Init();
	virtual void ReInit();
	void Render(float dt, int level);
};
