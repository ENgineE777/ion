#pragma once

#include "Core/Animation/IAnimationAffector.h"

class RotateMotorAffector: public IAnimationAffector
{	
	enum  eAngles
	{
		eYaw = 0,
		ePitch,
		eRoll,
	};
	struct boneMat
	{
		int			index;
		Matrix		rotation;
		boneMat()
		{
			index = -1;
		}
	};

	std::vector<boneMat> bones;

public:	
	RotateMotorAffector():IAnimationAffector(){}
	RotateMotorAffector(affecorDeclaratorBase*  Desc):IAnimationAffector(Desc)
	{}
	virtual void TransformBone(int index, Matrix* parentMat, Matrix* curMat);
	void RotateBone(int index, const Matrix& rotation);
	void UnregisterBone(int index);	
};