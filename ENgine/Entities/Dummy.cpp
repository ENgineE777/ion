
#include "Dummy.h"

ENTITY(Dummy)
ENTITY_PARAMS(Dummy,"Dummy","Geometry", "Dummy")

void Dummy::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( attach_name )
}

void Dummy::Init()
{
	attach = null;		

	SetUpdate((TaskHolder::Task)&Dummy::Work, lvl_work1);
}

bool Dummy::Start()
{
    attach = Scene()->Find(attach_name.c_str());
	
	if (attach)
	{
		Matrix tr;		
		attach->GetTransform(tr);
		tr.Inverse();

		relative_matrix = transform * tr;
	}
	else
	{
        relative_matrix.Identity();
	}

	return true;
}

void Dummy::Work(float dt, int level)
{
	if (Scene()->IsRunning())
	{
		if (attach)
		{
			Matrix tr;		
			attach->GetTransform(tr);

			transform = relative_matrix * tr;
		}
	}	
}