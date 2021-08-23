
#include "Model.h"
#include "PointLight.h"
#include "DirLight.h"
#include "AmbientLight.h"

ENTITY(Model)
ENTITY_PARAMS(Model,"Model","Geometry", "Model")

void Model::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( model_name )
	PROP_REG( rtech )
	PROP_REG( scriptDraw )	
}

void Model::Init()
{	
	model = null;	
	scale = 1.0f;	
}

Model::~Model()
{
	RELEASE(model)
	en_core.Render()->Tasks()->DelTask("geometry", this);
}

void Model::ReInit()
{	
	en_core.Render()->Tasks()->DelTask("geometry", this);

	ModelInstance* old_model = model;

	model = en_core.Models()->LoadModel(model_name.c_str());

	if (old_model)
	{
		old_model->Release();
	}

	/*if (rtech == 0)
	{
		rt = en_core.Render()->CreateRenderTechnique("RTM_Diffuse");

		en_core.Render()->Tasks()->AddTask("geometry", this, Scene(), (TaskHolder::Task)&Model::Render, 0);
	}
	else
	if (rtech == 1)
	{
		rt = en_core.Render()->CreateRenderTechnique("RTM_DiffuseBlend");
		en_core.Render()->Tasks()->AddTask("geometry", this, Scene(), (TaskHolder::Task)&Model::Render, 10);
	}
	else
	if (rtech == 2)
	{
		rt = en_core.Render()->CreateRenderTechnique("RTM_DiffuseBlendAlpha");
		en_core.Render()->Tasks()->AddTask("geometry", this, Scene(), (TaskHolder::Task)&Model::Render, 10);
	}
	else
	if (rtech == 3)
	{
		rt = en_core.Render()->CreateRenderTechnique("RTM_DiffusePxL");
		en_core.Render()->Tasks()->AddTask("geometry", this, Scene(), (TaskHolder::Task)&Model::Render, 0);
	}	
	else
	if (rtech == 4)
	{
		rt = en_core.Render()->CreateRenderTechnique("RTM_DiffusePxCam");
		en_core.Render()->Tasks()->AddTask("geometry", this, Scene(), (TaskHolder::Task)&Model::Render, 10);
	}
	else
	if (rtech == 5)
	{
		rt = en_core.Render()->CreateRenderTechnique("RTM_Atmo");
		en_core.Render()->Tasks()->AddTask("geometry", this, Scene(), (TaskHolder::Task)&Model::Render, 20);
	}*/
}

void Model::Render(float dt, int level)
{		
	if (!IsVisible()) return;

	//if (rtech != 5) return;

	en_core.Render()->SetRenderTechnique(rt);

	if (model)
	{
		SceneInstance::EntityGroup* sg_al = Scene()->GetGroup("AmbientLight");

		if (sg_al && sg_al->entities.size()>0)
		{
			AmbientLight* al = (AmbientLight*)sg_al->entities[0];
			
			rt->PS_SetVector4("ambientColor", Vector4(al->color.r,al->color.g,al->color.b,al->color.a));
		}
		else
		{
			rt->PS_SetVector4("ambientColor", Vector4(0.2f));
		}

		SceneInstance::EntityGroup* sg_dl = Scene()->GetGroup("DirLight");

		if (sg_dl && sg_dl->entities.size()>0)
		{
			DirLight* dl = (DirLight*)sg_dl->entities[0];
				
			Matrix pt_mat;
			dl->GetTransform(pt_mat);

			rt->PS_SetVector4("lightDir", Vector4(pt_mat.Vz()));
			rt->PS_SetVector4("lightDirColor", Vector4(dl->color.r,dl->color.g,dl->color.b,dl->power));
		}
		else
		{
			rt->PS_SetVector4("lightDir", 0.0f);
			rt->PS_SetVector4("lightDirColor", 0.0f);
		}

		SceneInstance::EntityGroup* sg = Scene()->GetGroup("PointLight");

		for (int i=0;i<4;i++)
		{
			char l_pos[32];
			String::Printf(l_pos, 32, "lightPos%i", i+1);
				
			char l_color[32];
			String::Printf(l_color, 32, "lightColor%i", i+1);

			Vector4 pos = 0.0f;
			Vector4 color = 0.0f;
			
			if (sg)
			{
				if (sg->entities.size()>i)
				{
					PointLight* pl = (PointLight*)sg->entities[i];
				
					Matrix pt_mat;
					pl->GetTransform(pt_mat);
	
					pos = Vector4(pt_mat.Pos());
					color = Vector4(pl->color.r,pl->color.g,pl->color.b,pl->power);
				}
			}
			
			rt->VS_SetVector4(l_pos, pos);
			rt->PS_SetVector4(l_color, color);
		}

		Matrix scale_mat;
		scale_mat.BuildScale(scale);

		if (scriptDraw && Scene()->IsRunning())
		{
			for (int i=0;i<draw_calls.size();i++)
			{
				Matrix tr = scale_mat * draw_calls[i];

				model->SetTransform(tr);
				model->Draw(rt);
			}

			draw_calls.clear();
		}
		else
		{
			Matrix tr = scale_mat * transform;

			model->SetTransform(tr);
			model->Draw(rt);
		}		
	}
}

CALLWRAPPERARG3(Model, SetTransformRot, CALLWRAPPERGETADRESS(Vector,arg1), CALLWRAPPERGETADRESS(Vector,arg2), CALLWRAPPERGETADRESS(Vector,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG1(Model, SetTransformScale, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG1(Model, RotTransformY, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG3(Model, AddDrawCall, CALLWRAPPERGETADRESS(Vector,arg1), CALLWRAPPERGETADRESS(Vector,arg2), CALLWRAPPERGETADRESS(Vector,arg3), CALLWRAPPERNORET())	

void Model::MethodsToScript()
{
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void SetTransform(Vector&in pos, Vector&in pos_vz, Vector&in vy)", CALLWRAPPERNAMEAS(SetTransformRot));	
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void SetTransformScale(float y)", CALLWRAPPERNAMEAS(SetTransformScale));	
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void RotTransformY(float y)", CALLWRAPPERNAMEAS(RotTransformY));	
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void AddDrawCall(Vector&in pos, Vector&in pos_vz, Vector&in vy)", CALLWRAPPERNAMEAS(AddDrawCall));	
}

void Model::AddDrawCall(Vector& vx, Vector& vy, Vector& vz)
{
	vz.Normalize();

	Matrix tr;
	tr.BuildView(vx, vy, vz);
	tr.Inverse();

#if defined(IOS) || defined(ANDROID)
	tr.Vz() = -tr.Vz();
#endif

	draw_calls.push_back(tr);
}

void Model::SetTransformRot(Vector& vx, Vector& vy, Vector& vz)
{
	vz.Normalize();

	Matrix tr;
	tr.BuildView(vx, vy, vz);
	tr.Inverse();

#if defined(IOS) || defined(ANDROID)
	tr.Vz() = -tr.Vz();
#endif
	
	SetTransform(tr);	
}

void Model::SetTransformScale(float scl)
{
	scale = scl;
}

void Model::RotTransformY(float y)
{
	Matrix rot;
	
#if defined(IOS) || defined(ANDROID)
	rot.BuildRotateY(-y);
#else
	rot.BuildRotateY(y);
#endif
	
	transform = rot * transform;
}