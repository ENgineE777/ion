#pragma once
#include "Core/Core.h"

void CheckCallbackProp(PropertiesOwner* owner);

class Camera : public SceneEntity
{		
public:				
		
	PROP_START
	FLOAT_PROP(Camera, fov, 45.0f, "Properties", "FOV")
	BOOL_PROP(Camera, test_cam, false, "Properties", "TestView")

	ENTITY_DEFC(Camera)
	virtual ~Camera();

	//ModelInstance* cam_model;

	virtual void RegisterProperties();
	virtual void Init();
	void Update(float dt, int level);

	virtual void MethodsToScript();
	void    SetTarget(Vector& from, Vector& to);
};