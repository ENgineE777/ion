#pragma once

#include "Core/Animation/IAnimationAffector.h"

#ifdef ANIM_AFFECT_PROP

class WaveAffector: public IAnimationAffector
{	
	AF_BONENAME_PROP( sStartBone, WaveAffector, "", "Start")	
	AF_BONENAME_PROP( sEndBone, WaveAffector, "", "End")	
	AF_FLOAT_PROP( fTideHeight, WaveAffector, 0.025f, "Height")	
	AF_FLOAT_PROP( fTideAmp, WaveAffector, 1.0f, "Amplitude")
	AF_FLOAT_PROP( fTideSpeed, WaveAffector, 1.0f, "Speed")

	struct Data
	{
		int index;
		float parent_lenght;

		Data(int indx)
		{
			index = indx;
			parent_lenght = -1.0f;
		}
	};

	IVector root;
	std::vector<Data> map;

	float offset;
	bool  inited;

public:	
	WaveAffector(affecorDeclaratorBase*  Desc):IAnimationAffector(Desc)
	{}	
	virtual void Init();
	virtual void ReInit();
	bool  Init(const char* start_bone, const char* end_bone);
	float Tider(float x);

	virtual void TransformBone(int index, IMatrix* parentMat, IMatrix* curMat);	
	virtual void Update(float dt);
};

#endif