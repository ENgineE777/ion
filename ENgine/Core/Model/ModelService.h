
#pragma once

#include "ModelInstance.h"
#include "ModelRes.h"

class ModelService
{		
public:	

	ModelService();
	bool Init();

	ModelInstance* LoadModel( const char* name);

	void RemoveRes(ModelRes* res);
	void Release();	
	void Clear();	

#ifdef PC
	void Save(const char* mdl, const char* path, Platform platform);
#endif

protected:

	HashMap<ModelRes> models;
	FileBuffer* buffer;
};

