#include "Core/Core.h"

ModelService::ModelService() : models(_FL_)
{	
}

bool ModelService::Init()
{
	return true;
}

ModelInstance* ModelService::LoadModel(const char* name)
{
	if (!name[0]) return null;

	char nm[256];
	String::Copy(nm, 256, name);
	String::LowerCase(nm);

	ModelRes* res = models.Get(nm);

	if (!res)
	{
		res = new ModelRes();		

		if (!res->Load(nm))
		{
			delete res;
			return null;
		}		
		
		models.Add(nm,res);
	}	
			
	return res->CreateReference();	
}

void ModelService::RemoveRes(ModelRes* res)
{
	models.Delete(res, false);
}

#ifdef PC
void ModelService::Save(const char* mdl, const char* path, Platform platform)
{
	char nm[256];
	String::Copy(nm, 256, mdl);
	String::LowerCase(nm);

	ModelRes* res = models.Get(nm);

	if (res)
	{
		res->Save(path, platform);		
	}
}
#endif

void ModelService::Release()
{

}