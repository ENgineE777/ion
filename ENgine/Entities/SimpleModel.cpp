
#include "SimpleModel.h"

ENTITY_DECL(SimpleModel, "SimpleModel", 0)

void SimpleModel::Init()
{	
	model = null;
	anim = null;
	
	PROP_REG( model_name )
	PROP_REG( anim_name )	

	SetUpdate((TaskHolder::Task)&SimpleModel::Render, lvl_draw1);	
}

void SimpleModel::ReInit()
{
	if (model)
	{
		model->Release();
	}

	if (anim)
	{
		anim->Release();
	}

	model = core.Models()->LoadModel(model_name);

	if (model)
	{
		anim = core.Animation()->LoadAnimation(anim_name);
		model->SetAnimation(anim);
	}

	rot.Identity();
}

void SimpleModel::Render(float dt, int level)
{		
	if (model)
	{
		core.Render()->SetWorld(transform);
		model->Draw();		
	}
}
