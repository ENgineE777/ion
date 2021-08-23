
#include "AnimationControllerIK.h"
#include "AnimationProcessor.h"
#include "AnimationInstance.h"
#include "AnimationService.h"

//-------- SOLVE TWO LINK INVERSE KINEMATICS -------------

// Given a two link joint from [0,0,0] to end effector position P,
// let link lengths be a and b, and let norm |P| = c.  Clearly a+b >= c.
//
// Problem: find a "knee" position Q such that |Q| = a and |P-Q| = b.
//
// In the case of a point on the x axis R = [c,0,0], there is a
// closed form solution S = [d,e,0], where |S| = a and |R-S| = b:
//
//    d2+e2 = a2                  -- because |S| = a
//    (c-d)2+e2 = b2              -- because |R-S| = b
//
//    c2-2cd+d2+e2 = b2           -- combine the two equations
//    c2-2cd = b2 - a2
//    c-2d = (b2-a2)/c
//    d - c/2 = (a2-b2)/c / 2
//
//    d = (c + (a2-b2/c) / 2      -- to solve for d and e.
//    e = sqrt(a2-d2)

double findD(double a, double b, double c)
{
	float mn = a;
	if (mn > (c + (a*a-b*b)/c) / 2)
	{
		mn = (c + (a*a-b*b)/c) / 2;
	}

	if (mn>0) return mn;
	
	return 0;
}

double findE(double a, double d)
{
	return sqrt(a*a-d*d);
} 

double norm(Vector v)
{
	return sqrt(v | v);
}

Matrix mWd;
Matrix mInv;

void defineM(Vector p, Vector d)
{
	mWd.Identity();

	mWd.Vx() = p;
	mWd.Vx().Normalize();
			
	double dDOTx = d | mWd.Vx();

	mWd.Vy() = d - dDOTx * mWd.Vx();	
	mWd.Vy().Normalize();

	mWd.Vz() = mWd.Vx() ^ mWd.Vy();	

	mInv = mWd;
	mInv.Inverse();
}	

bool Solve(double A, double B, Vector p, Vector d, Vector& q)
{
	Vector r;
	defineM(p,d);
	r = p * mInv;
	double d2 = findD(A,B,norm(r));
	double e = findE(A,d2);	
	q = Vector(d2,e,0) * mWd;

	return (d2 > 0 && d2 < A);
}	

AnimationControllerIK::BoneAffector::BoneAffector()
{
	target_weight = 0.0f;
	weight_delta = 0.0f;
	cur_weight = 0.0f;
	speed = 1.0f;
	goal = 0.0f;
	clearMat.Identity();

	inited = false;
}

void AnimationControllerIK::BoneAffector::TransformBone(int index, Matrix* parentMat, Matrix* curMat)
{	
	if (!inited)
	{
		/*inited = true;
		bone.index = Owner()->GetBoneIndex(bone.name);
		bone.parent_index = Owner()->GetBoneParentIndex(bone.index);		
		bone.next_parent_index = Owner()->GetBoneParentIndex(bone.parent_index);		

		const Matrix* mat1 = Owner()->GetBoneMatrix(bone.index);
		const Matrix* mat2 = Owner()->GetBoneMatrix(bone.parent_index);
		const Matrix* mat3 = Owner()->GetBoneMatrix(bone.next_parent_index);

		len_1 = (mat2->Pos() - mat1->Pos()).Length();
		len_2 = (mat2->Pos() - mat3->Pos()).Length();
				
		Matrix tmp;
		
		up1 = mat3->Vz();
		tmp.BuildView(mat3->Pos(),mat2->Pos(),up1);
		skin1 = (*mat3) * tmp;

		up2 = mat2->Vz();
		tmp.BuildView(mat2->Pos(),mat1->Pos(),up2);
		skin2 = (*mat2) * tmp;

		Reset();*/
	}

	if (cur_weight<0.001f) return;

	if (bone.next_parent_index == index)
	{		
		Matrix curGl = (*curMat) * (*parentMat);

		Vector local_bendDir = curMat->MulNormal(bendDir);
		

		loc_goal = goal;

		Matrix inv_gl_pose = ((AnimationInstance*)Owner())->processor->inv_globalMatrix;
		Matrix gl_pose = ((AnimationInstance*)Owner())->processor->globalMatrix;

		loc_goal = loc_goal * inv_gl_pose - parentMat->Pos();
		float len = loc_goal.Normalize();


		local_bendDir = local_bendDir ^ loc_goal;

		if (len>(len_1 + len_2)) len = len_1 + len_2;
		loc_goal *= len;		

		Vector d = loc_goal * 0.5f + local_bendDir * (len_1 + len_2);
	
		Solve(len_1, len_2, loc_goal, d, q);

		
		Vector pos1 = parentMat->Pos() * gl_pose;
		Vector pos2 = (parentMat->Pos() + loc_goal) * gl_pose;
		Vector pos3 = (parentMat->Pos() + q) * gl_pose;
		Vector pos4 = (parentMat->Pos() + d) * gl_pose;

		Matrix mt;
		mt.BuildView(0.0f,q,up1);
		mt.Inverse();

		mt.Pos() = curGl.Pos();

		Matrix invParent = *parentMat;
		invParent.Inverse();

		(*curMat) = skin1 * mt * invParent;
	}

	if (bone.parent_index == index)
	{
		Matrix curGl = (*curMat) * (*parentMat);

		Matrix mt;
		mt.BuildView(q,loc_goal,up2);
		mt.Inverse();

		mt.Pos() = curGl.Pos();

		Matrix invParent = *parentMat;
		invParent.Inverse();

		(*curMat) = skin2 * mt * invParent;
	}

	if (bone.index == index)
	{
		(*curMat) = preRotation * (*curMat);
	}
}

void AnimationControllerIK::BoneAffector::SetBone(const char* boneName)
{
	inited = false;	
	String::Copy(bone.name,63,boneName);	
}

void AnimationControllerIK::BoneAffector::SetWeight(float weight, float time)
{
	if (time > 0.01f)
	{
		if (fabs(weight - target_weight) > 0.01f )
		{			
			target_weight = weight;
			weight_delta = (target_weight-cur_weight) / time;				
		}
	}
	else
	{
		target_weight = weight;
		cur_weight = weight;
	}	
}

float AnimationControllerIK::BoneAffector::GetWeight()
{
	return cur_weight;
}

void AnimationControllerIK::BoneAffector::SetSpeed(float spd)
{
	speed = spd;
}

void  AnimationControllerIK::BoneAffector::Reset()
{
	cur_ang = 0.0f;
	cur_rot = 0.0f;

	ang = 0.0f;
	rot = 0.0f;
}

void AnimationControllerIK::BoneAffector::SetConstraints(const Vector& min,const Vector& max)
{
	c_min = min;
	c_max = max;
}

void AnimationControllerIK::BoneAffector::UpdateAngle(float dt, float& cur_angle, float angle, float c_mn, float c_mx)
{
	if ((c_mx - c_mn)> 2*3.14159265 - 0.01f)
	{
		float sg = 1.0f;
		float limit = angle;

		if (fabsf(cur_angle - angle) > 3.14159265)
		{
			if (angle>0.0f)
			{
				limit -= 2*PI;
			}
			else
			{
				limit += 2*PI;
			}
		}

		if (cur_angle>limit) sg = -1.0f;

		cur_angle += speed * dt * sg;

		if (sg>0.0f)
		{
			if (cur_angle>limit) cur_angle=limit;
		}
		else
		{
			if (cur_angle<limit) cur_angle=limit;
		}

		if (cur_angle> PI) cur_angle -= 2*PI;
		if (cur_angle<-PI) cur_angle += 2*PI;
	}
	else
	{
		if (cur_angle < angle)
		{
			cur_angle += speed * dt;

			if (cur_angle > angle )
			{				
				cur_angle = angle;			
			}
		}
		else
		{
			cur_angle -= speed * dt;

			if (cur_angle < angle)
			{				
				cur_angle = angle;			
			}
		}	
	}
}

void AnimationControllerIK::BoneAffector::Update(float dt)
{	
	if (!inited) return;

	if (target_weight >= 0.0f)
	{
		cur_weight += weight_delta * dt;

		if (weight_delta > 0.0f)
		{				
			if (cur_weight > target_weight)
			{
				cur_weight = target_weight;
				target_weight = -1.0f;
			}
		}
		else
		{
			if (cur_weight < target_weight)
			{
				cur_weight = target_weight;
				target_weight = -1.0f;							
			}
		}
	}	
	
	UpdateAngle(dt, cur_ang, ang, c_min.x, c_max.x);
	UpdateAngle(dt, cur_rot, rot, c_min.y, c_max.y);
}

AnimationControllerIK::AnimationControllerIK()
{	
	active = true;
	weight = 0.0f;			
	processor = NULL;
}

bool AnimationControllerIK::Init(IAnimationProcessor* anim_processor, ControllerType type,
							   const char* boneName,const char* boneOffsetName,
							   const Vector& min,const Vector& max, float speed)
{
	processor = (AnimationProcessor*)anim_processor;

	processor->anim->AddAffector(&affector, 0);
	affector.SetBone(boneName);
	affector.SetConstraints( min, max);	
	affector.SetSpeed(speed);
	affector.SetWeight(weight, 0.01f);		

	return true;	
}

void AnimationControllerIK::Activate(bool act)
{	
	if (act)
	{		
		if (affector.GetWeight()<0.01f && weight>0.01f)
		{
			affector.Reset();
		}

		affector.SetWeight(weight,0.35f);
	}
	else
	{
		affector.SetWeight(0.00f,0.05f);		
	}

	active = act;
}

bool AnimationControllerIK::IsActive()
{
	return active;
}

void AnimationControllerIK::SetWeight(float wght, float time)
{	
	weight = wght;
	if (!active) return;

	if (affector.GetWeight()<0.01f && wght>0.01f)
	{
		affector.Reset();
	}
	
	affector.SetWeight(weight,time);	
}

float AnimationControllerIK::GetWeight()
{	
	// maybe hasn't need in this
	//if (!active) return -1.0f;

	return weight;
}		

float AnimationControllerIK::GetCurWeight()
{
	// maybe hasn't need in this
	//if (!active) return -1.0f;

	return affector.GetWeight();	
}

void AnimationControllerIK::SetGoal(const Vector& goal)
{	
	//if (!active) return;

	affector.goal = goal;	
}

void AnimationControllerIK::SetConstraints(const Vector& min,const Vector& max)
{			
	affector.SetConstraints( min, max);	
}

void AnimationControllerIK::SetPreRotation(Matrix& transform)
{
	affector.preRotation = transform;
}

void AnimationControllerIK::SetBendDirection(const Vector& dir)
{	
	affector.bendDir = dir;
}

void AnimationControllerIK::SetOffset(const Vector& offset)
{

}

void AnimationControllerIK::SetSpeed(float speed)
{
	affector.SetSpeed(speed);	
}

void AnimationControllerIK::Update(float dt)
{	
}

void AnimationControllerIK::Release()
{

}