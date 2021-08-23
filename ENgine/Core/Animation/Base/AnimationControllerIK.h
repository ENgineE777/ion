#pragma once

#include "Core/Animation/IAnimationController.h"
#include "Core/Animation/IAnimationAffector.h"

class AnimationProcessor;

class AnimationControllerIK : public IAnimationController
{
	float weight;	
	bool  active;	
				
	AnimationProcessor* processor;

	class BoneAffector: public IAnimationAffector
	{	
	public:

		struct boneMat
		{
			int index;			
			int parent_index;
			int next_parent_index;

			char  name[64];			
		};

		boneMat bone;		

		bool inited;
		
		Vector goal;
		Matrix  preRotation;

		float target_weight;
		float cur_weight;
		float weight_delta;
		float speed;
		Vector c_min, c_max;

		Vector bendDir;
		Vector up1, up2;
		Matrix skin1, skin2;
		float len_1,len_2;
		Vector loc_goal,q;		

		float cur_ang;
		float cur_rot;

		

		float ang;
		float rot;

		Matrix clearMat;

		BoneAffector();
		virtual void TransformBone(int index, Matrix* parentMat, Matrix* curMat);
		void  SetBone(const char* boneName);		
		void  SetWeight(float weight, float time);	
		float GetWeight();		
		void  SetSpeed(float spd);
		void  Reset();
		void  SetConstraints(const Vector& min,const Vector& max);
		void  UpdateAngle(float dt, float& cur_angle, float angle, float c_mn, float c_mx);
		virtual void Update(float dt);
	};
	
	BoneAffector affector;


public:

	AnimationControllerIK();

	virtual bool Init(IAnimationProcessor* anim_processor, ControllerType type,
					  const char* boneName,const char* boneOffsetName,
					  const Vector& min,const Vector& max, float speed);	

	virtual void  Activate(bool act);	
	virtual bool  IsActive();	
	virtual void  SetWeight(float weight, float time);	
	virtual float GetWeight();	
	virtual float GetCurWeight();
	virtual void  SetGoal(const Vector& goal);	
	virtual void  SetAdditionalParameter(const void* userData) {}
	virtual void  SetConstraints(const Vector& min,const Vector& max);
	virtual void  SetPreRotation(Matrix& transform);
	virtual void  SetBendDirection(const Vector& dir);
	virtual void  SetOffset(const Vector& offset);
	virtual void  SetSpeed(float speed);	

#ifdef ANIM_STANDALONE
	virtual void  GetEDMatrix(Matrix& mat) {};
#endif

	virtual void  Update(float dt);	
	virtual void  Release();	
};