#pragma once
#include "Core/Core.h"

class Model : public SceneEntity
{	
	ModelInstance* model;
	RenderTechnique* rt;
	float scale;

	std::vector<Matrix> draw_calls;

public:
			
	PROP_START
	FILENAME_PROP(Model, model_name, "", "Properties", "model", "*.*")

	ENUM_PROP(Model, rtech, 0, "Properties", "RenderTech")
		ENUM_ITEM("simple", 0)
		ENUM_ITEM("simple_blend", 1)
		ENUM_ITEM("simple_blend_alpha", 2)
		ENUM_ITEM("dynamic", 3)
		ENUM_ITEM("dynamic_cam", 4)
		ENUM_ITEM("atmos", 5)
	ENUM_END(Model, rtech)	

	BOOL_PROP(Model, scriptDraw, false, "Properties", "ScriptedDraw")

	ENTITY_DEFC(Model)
	virtual ~Model();

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();
	void Render(float dt, int level);

	virtual void MethodsToScript();
	void    SetTransformRot(Vector& vx, Vector& vy, Vector& vz);	
	void    SetTransformScale(float scale);
	void    RotTransformY(float y);
	void    AddDrawCall(Vector& vx, Vector& vy, Vector& vz);
};
