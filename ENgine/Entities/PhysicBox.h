#pragma once

#include "Core/core.h"

class PhysicBox : public SceneEntity
{		
public:				

	FLOAT_PROP(PhysicBox, weight, 1.0f, "weight")
	FLOAT_PROP(PhysicBox, width, 1.0f, "width")
	FLOAT_PROP(PhysicBox, height, 1.0f, "height")
	FLOAT_PROP(PhysicBox, length, 1.0f, "length")
	COLOR_PROP(PhysicBox, color, 0xffffffff, "color")

	ENTITY_DEFC(PhysicBox)

	~PhysicBox();

	IPhysObject* pbox;

	virtual void Init();	
	virtual void ReInit();

	virtual void Start();
	virtual void Finish();

	void ApplyImpulse(Vector& imp);
	void Render(float dt, int level);
};
