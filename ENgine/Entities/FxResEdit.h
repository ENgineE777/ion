#pragma once

#include "Core/core.h"

class FxResEdit : public SceneEntity
{
	Fx* m_fx;
	char resPathEdited[512];

public:
	
	FILENAME_PROP(FxResEdit, propResPath, "", "resPath", "*.fx");	
	INT_PROP(FxResEdit, numEmiters, 1, "numEmiters")
	INT_PROP(FxResEdit, curEditedEmiter, 1, "curEditedEmiter")

	FILENAME_PROP(FxResEdit, TextureName, "", "TextureName", "*.dds");
	BOOL_PROP(FxResEdit, Looped, true, "Looped")
	FLOAT_PROP(FxResEdit, LifeTime, 1.f, "LifeTime")
	INT_PROP(FxResEdit, ParticlesCount, 20, "ParticlesCount")
	INT_PROP(FxResEdit, SpawnRate, 2, "SpawnRate")
	FLOAT_PROP(FxResEdit, SpawnTime, 0.1f, "SpawnTime")
	FLOAT_PROP(FxResEdit, SizeMaxX, 0.1f, "SizeMaxX")
	FLOAT_PROP(FxResEdit, SizeMinX, 0.05f, "SizeMinX")
	FLOAT_PROP(FxResEdit, SizeMaxY, 0.1f, "SizeMaxY")
	FLOAT_PROP(FxResEdit, SizeMinY, 0.05f, "SizeMinY")
	FLOAT_PROP(FxResEdit, LifeTimeMin, 0.05f, "LifeTimeMin")
	FLOAT_PROP(FxResEdit, LifeTimeMax, 1.f, "LifeTimeMax")
	STRING_PROP(FxResEdit, PositionMin, "0.0,0.0,0.0", "PositionMin")
	STRING_PROP(FxResEdit, PositionMax, "0.0,0.0,0.0", "PositionMax")
	STRING_PROP(FxResEdit, VelocityMin, "0.0,0.0,0.0", "VelocityMin")
	STRING_PROP(FxResEdit, VelocityMax, "0.0,0.0,0.0", "VelocityMax")
	STRING_PROP(FxResEdit, AccelerationMin, "0.0,0.0,0.0", "AccelerationMin")
	STRING_PROP(FxResEdit, AccelerationMax, "0.0,0.0,0.0", "AccelerationMax")
	FLOAT_PROP(FxResEdit, Fade, 1.f, "Fade")
	COLOR_PROP(FxResEdit, Color, 0xffffffff, "Color")
	FLOAT_PROP(FxResEdit, scaleSizePerSec, 0.1, "scaleSizePerSec")
	ENTITY_DEFC(FxResEdit)

	virtual void Init();

	virtual void ReInit();

	virtual void Start();	

	virtual void Work(float dt, int level);
	virtual void Draw(float dt, int level);
};
