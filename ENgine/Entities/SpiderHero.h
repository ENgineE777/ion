#pragma once

#include "Core/Core.h"
#include "string.h"
#include "Entities/GUI/GUILabel.h"

#include "../../Libraries/Box/Box2D/Box2D.h"

class SpiderHero : public SceneEntity
{		
public:
	
	FLOAT_PROP(SpiderHero, x, 0, "x")
	FLOAT_PROP(SpiderHero, y, 0, "y")

	FLOAT_PROP(SpiderHero, height, 25, "height")	
	FLOAT_PROP(SpiderHero, width, 100, "width")

	FILENAME_PROP(SpiderHero, texName, "", "texture")

	ENTITY_DEFC(SpiderHero)

	Texture* tex;

	struct Collision
	{
		GUILabel* label;
		b2BodyDef def;
		b2PolygonShape shape;
		b2Body* body;
	};

	int num_collisions;
	Collision collisions[20];

	int num_hooks;
	GUILabel* hooks[10];

	b2Body* bodies[30];

	int num_active_hooks;

	struct Hook
	{
		int index;
		int num_segments;
		b2Body* segments[50];
		b2Joint* joints[50];
	};
	
	Hook active_hooks[5];

	bool blowed;

	b2World* world;
	float    workTime;

	b2Body* hero_body;

	virtual void Init();
	virtual void ReInit();

	virtual void Start();
	virtual void Finish();

	virtual void MethodsToScript();

	void AddCollision(std::string& name);
	void AddHook(std::string& name);
	void ConnectToHook();
	void GrabHook(std::string& name);

	virtual void SetGizmo(Gizmo* gz);
	virtual void DataToGizmo();
	virtual void DataFromGizmo();

	void Render(float dt, int level);
};
