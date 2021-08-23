
#pragma once
#include "Core/Core.h"

class SkyBox : public SceneEntity
{
public:		

	PROP_START

	FILENAME_PROP(SkyBox,side0,"","Properties", "FrontSide", "*.*")
	FILENAME_PROP(SkyBox,side1,"","Properties", "BackSide", "*.*")
	FILENAME_PROP(SkyBox,side2,"","Properties", "LeftSide", "*.*")
	FILENAME_PROP(SkyBox,side3,"","Properties", "RightSide", "*.*")
	FILENAME_PROP(SkyBox,side4,"","Properties", "UpSide", "*.*")
	FILENAME_PROP(SkyBox,side5,"","Properties", "DownSide", "*.*")
		
	RenderTechnique* tech;
	TextureInstance* sides[6];
	VertexBuffer* skyboxBuffer;	

	RenderTechnique* rtech;
	VertexBuffer* vbuffer;
	IndexBuffer*  ibuffer;
	
	float angle;
	
	ENTITY_DEFC(SkyBox)
	virtual ~SkyBox();
	
	float init_pow;
	float needed_pow;
	float chgspd_pow;
	float power;

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();

	virtual void Begin();
	virtual void End();

	virtual void Draw(float fDelta,int level);

#ifdef PC
	virtual void DataFromGizmo();
	virtual void DataToGizmo();	
#endif

	virtual void Command(const char* command,int NumParams,const char** params);
};