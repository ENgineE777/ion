
#include "AnimationControllerLookAt.h"
#include "AnimationProcessor.h"
#include "AnimationInstance.h"
#include "AnimationService.h"
#include "Core/Core.h"

AnimationControllerLookAt::BoneAffector::BoneAffector()
{
	target_weight = 0.0f;
	weight_delta = 0.0f;
	cur_weight = 0.0f;
	speed = 1.0f;
	goal = 0.0f;
	clearMat.Identity();

	offset = 0.0f;
	inited = false;
	eyePos = 0.f;
	lookDir = 0.f;
	useEyeVec = false;

	rot = 0.f;
	ang = 0.f;
	cur_rot = 0.f;
	cur_ang = 0.f;
}

void AnimationControllerLookAt::BoneAffector::TransformBone(int index, Matrix* parentMat, Matrix* curMat)
{
	if (!inited)
	{		
		bone.index = Owner()->GetBoneIndex(bone.name);		

		if (bone.index != -1) 
		{
			bone.offset_index = Owner()->GetBoneIndex(bone.boneOffset);
			bone.parent_index = Owner()->GetBoneParentIndex(bone.index);

			Reset();
		}

		inited = true;
	}

	if (bone.index == -1) return;
	if (cur_weight<0.001f) return;

	if (bone.index == index)
	{		
		Matrix curGl;

		if (parentMat)
		{
			curGl = (*curMat) * (*parentMat);
		}
		else
		{
			curGl = (*curMat);
		}

		Matrix gl_pose = ((AnimationInstance*)Owner())->processor->globalMatrix;
		Vector tmp = gl_pose.Pos();

		((AnimationInstance*)Owner())->processor->UpdateStack(bone.index);

		Matrix stackMat = ((AnimationInstance*)Owner())->processor->StackMat();
		Matrix stackMat_inv = stackMat;
		stackMat_inv.Inverse();

		Matrix gl_pose_bone = curGl * gl_pose;
		Vector dir = gl_pose_bone.Pos() - goal;

	  if ( !useEyeVec )
	  {
		float x_offset = 0.0f;
		float y_offset = 0.0f;
		
		if (bone.offset_index != -1)		
		{			
			Matrix offset_bone;
			((AnimationInstance*)Owner())->processor->CalcGlobalMat(bone.offset_index,offset_bone);

			Vector pos = offset;
			pos = pos * offset_bone;

			x_offset = pos.x - curGl.Pos().x;
			y_offset = pos.y - curGl.Pos().y;			
		}


		Vector dir2 = dir;
		//dir2.y = 0.0f;
		//dir2.x = 0;
		float len = dir2.Normalize();		

		clearMat = gl_pose;
		clearMat.Pos() = 0.0f;
		clearMat = clearMat * stackMat;
		clearMat.Pos() = gl_pose_bone.Pos();
		
		rot = asinf(dir2.y/1.0f) - asinf((gl_pose._32)/1.0f);		
		
		Vector dr = dir;
		//dr.x = 0.0f;
		float dst = dr.Normalize();		


		if (bone.offset_index != -1)
		{
			rot += -atanf(fabs(y_offset)/dst);
		}

		rot = rot - ((AnimationInstance*)Owner())->processor->StackRot();
		
		if (rot<c_min.y)
		{
			rot=c_min.y;
		}

		if (rot>c_max.y)
		{
			rot=c_max.y;
		}		

		((AnimationInstance*)Owner())->processor->StackSetRot(rot);		

		//dir.y = 0.0f;
		dst = dir.Normalize();		

		#ifndef ANIM_STANDALONE
		

#if defined(PC)
		if(((AnimationInstance*)Owner())->res->manager->IsDebug())
		{
			//g_GameProxy.DebugLine->addLine(0x33ff0000,
			//							   &D3DXVECTOR3(gl_pose_bone.Pos().x, gl_pose_bone.Pos().y, gl_pose_bone.Pos().z),
			//							   &D3DXVECTOR3(goal.x, goal.y, goal.z));										   

			//core->Render()->DrawLine( gl_pose_bone.Pos(), COLOR_RED, goal, COLOR_RED );
			//core->Render()->DrawLine( gl_pose_bone.Pos(), Color(), 
			//	gl_pose_bone.Pos() - 50.f * Vector( gl_pose_bone._31, gl_pose_bone._32, gl_pose_bone._33 ), Color() );

		}
#endif
		#endif			

		Vector dir_vx = Vector(dir.z, 0.0f,-dir.x);
		dir_vx.Normalize();

		Vector dir_vz = Vector(dir.x, 0.0f, dir.z);
		dir_vz.Normalize();

		Vector m_v = Vector(gl_pose._31,0.0f,gl_pose._33);
		m_v.Normalize();
		
		ang = dir_vz | m_v;		
		
		if (ang> 0.9999999f) ang = 0.9999999f;
		if (ang<-0.9999999f) ang =-0.9999999f;

		ang = acosf(ang);

		if ((dir_vx | m_v)>0.0f)
		{
			ang = -ang;
		}

		if (bone.offset_index != -1)
		{
			ang += -atanf(fabs(x_offset)/dst);
		}		

		ang = ang - ((AnimationInstance*)Owner())->processor->StackAng();

		if (ang<c_min.x)
		{
			ang=c_min.x;
		}

		if (ang>c_max.x)
		{
			ang=c_max.x;
		}		

		((AnimationInstance*)Owner())->processor->StackSetAng(ang);
	  }
	  else
	  {
			// calc rot (pitch) & ang (yaw)
		    /*Vector dir = goal - eyePos;
			NxVec3 targetDir(dir.x,dir.y,dir.z);
			targetDir.normalize();

			NxReal targetPitch, lookPitch;
			NxReal targetYaw, lookYaw;

			targetPitch = SGameProxy::GetPitch( targetDir );
			lookPitch = SGameProxy::GetPitch( (NxVec3&)lookDir );

			targetYaw = SGameProxy::GetYaw( targetDir );
			lookYaw = SGameProxy::GetYaw( (NxVec3&)lookDir );
			float deltaYaw = targetYaw - lookYaw;
			if ( deltaYaw > NxPi )
			{
				deltaYaw -= NxTwoPi;
			}
			else if ( deltaYaw < -NxPi )
			{
				deltaYaw += NxTwoPi;
			}

			float angDelta = 0.1f * deltaYaw;			// уменьшаем угол, на случай, когда поворот контроллера вызывает 
			float rotDelta = 0.1f * (targetPitch - lookPitch);	// больший поворот целевой кости
			ang = cur_ang + angDelta;
			rot = cur_rot - rotDelta;
			ang = ang - ((AnimationInstance*)Owner())->processor->StackAng();
			rot = rot - ((AnimationInstance*)Owner())->processor->StackRot();
			
			if (rot<c_min.y)
			{
				rot=c_min.y;
			}
			else if (rot>c_max.y)
			{
				rot=c_max.y;
			}
			if (ang<c_min.x)
			{
				ang=c_min.x;
			}
			else if (ang>c_max.x)
			{
				ang=c_max.x;
			}

			((AnimationInstance*)Owner())->processor->StackSetRot(rot-cur_rot);
			((AnimationInstance*)Owner())->processor->StackSetAng(ang-cur_ang);

			if ( ((AnimationInstance*)Owner())->res->manager->IsDebug() )
			{
				core->Render()->DrawSphere( eyePos, 0.1f, COLOR_YELLOW );
				core->Render()->DrawLine( eyePos, COLOR_RED, eyePos + 100.f * lookDir, COLOR_RED );
				core->Render()->DrawLine( eyePos, COLOR_GREEN, eyePos + 100.f * (Vector&)targetDir, COLOR_GREEN );
			}*/
	  }


		if ( ((AnimationInstance*)Owner())->res->manager->IsDebug() )
		{
			//core->Render()->PrintText(20, bone.index*10,up_left, COLOR_GREEN, 
			//	"controller %s rot %.3f cur rot %.3f stack rot delta %.3f ang %.3f cur ang %.3f", 
			//	bone.name, rot, cur_rot, ((AnimationInstance*)Owner())->processor->StackRot(), ang, cur_ang); 
		}

		Matrix needed;
		needed.RotateY(cur_ang * cur_weight);

		Matrix rot_mat;
		rot_mat.RotateX(-cur_rot * cur_weight);				
		
		/*
		Matrix tst(true);

		IQuaternion q1, q2;
		q1.Set(tst);
		q2.Set(needed);		

		IQuaternion nd_q;
		nd_q.SLerp(q1,q2,cur_weight);

		nd_q.GetMatrix( needed );
		*/
		
		((AnimationInstance*)Owner())->processor->StackSetMat(needed);
		((AnimationInstance*)Owner())->processor->RiseDepth();

		tmp = curGl.Pos();
		curGl.Pos() = 0.0f;
		
		//preRotation.Identity();
		curGl = curGl * preRotation * stackMat_inv * rot_mat * stackMat * needed;		
		curGl.Pos() = tmp;

		if (parentMat)
		{
			Matrix invParent = *parentMat;
			invParent.Inverse();

			(*curMat) = curGl * invParent;
		}		
	}
}

void AnimationControllerLookAt::BoneAffector::SetBonesNames(const char* boneName, const char* boneOffsetName)
{
	inited = false;	
	String::Copy(bone.name,63,boneName);
	String::Copy(bone.boneOffset,63,boneOffsetName);
}

void AnimationControllerLookAt::BoneAffector::SetWeight(float weight, float time)
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

float AnimationControllerLookAt::BoneAffector::GetWeight()
{
	return cur_weight;
}

void AnimationControllerLookAt::BoneAffector::SetSpeed(float spd)
{
	speed = spd;
}

void AnimationControllerLookAt::BoneAffector::Reset()
{
	cur_ang = 0.0f;
	cur_rot = 0.0f;

	ang = 0.0f;
	rot = 0.0f;

	useEyeVec = false;
}

void AnimationControllerLookAt::BoneAffector::SetConstraints(const Vector& min,const Vector& max)
{
	c_min = min;
	c_max = max;
}

void AnimationControllerLookAt::BoneAffector::UpdateAngle(float dt, float& cur_angle, float angle, float c_mn, float c_mx)
{
	if ( useEyeVec )
	{

		cur_angle = angle;
		return;
	}
	//float _speed = useEyeVec ? 3.f * fabs( angle - cur_angle ) : speed;

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
		if (fabs(cur_angle - angle)<speed * dt)
		{
			cur_angle = angle;
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
}

void AnimationControllerLookAt::BoneAffector::Update(float dt)
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

AnimationControllerLookAt::AnimationControllerLookAt()
{	
	active = true;
	weight = 0.0f;			
	processor = NULL;
}

bool AnimationControllerLookAt::Init(IAnimationProcessor* anim_processor, ControllerType type,
							         const char* boneName,const char* boneOffsetName,
							         const Vector& min,const Vector& max, float speed)
{
	processor = (AnimationProcessor*)anim_processor;

	processor->anim->AddAffector(&affector, 0);
	affector.SetBonesNames(boneName,boneOffsetName);
	affector.SetConstraints( min, max);	
	affector.SetSpeed(speed);
	affector.SetWeight(weight, 0.01f);		

	return true;	
}

void AnimationControllerLookAt::Activate(bool act)
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

bool AnimationControllerLookAt::IsActive()
{
	return active;
}

void AnimationControllerLookAt::SetWeight(float wght, float time)
{	
	weight = wght;
	if (!active) return;

	if (affector.GetWeight()<0.01f && wght>0.01f)
	{
		affector.Reset();
	}
	
	affector.SetWeight(weight,time);	
}

float AnimationControllerLookAt::GetWeight()
{	
	// maybe hasn't need in this
	//if (!active) return -1.0f;

	return weight;
}		

float AnimationControllerLookAt::GetCurWeight()
{
	// maybe hasn't need in this
	//if (!active) return -1.0f;

	return affector.GetWeight();	
}

void AnimationControllerLookAt::SetGoal(const Vector& goal)
{	
	//if (!active) return;

	affector.goal = goal;	
	affector.useEyeVec = false;
}

void AnimationControllerLookAt::SetAdditionalParameter(const void* userData)
{
	EyeData* eye = (EyeData*)userData;
	affector.eyePos = eye->eyePos;
	affector.lookDir = eye->lookDir;
	affector.useEyeVec = true;
}

void AnimationControllerLookAt::SetConstraints(const Vector& min,const Vector& max)
{			
	affector.SetConstraints( min, max);	
}

void AnimationControllerLookAt::SetPreRotation(Matrix& transform)
{
	affector.preRotation = transform;
}

void AnimationControllerLookAt::SetBendDirection(const Vector& dir)
{	
}

void AnimationControllerLookAt::SetOffset(const Vector& offset)
{
	affector.offset = offset;	
}

void AnimationControllerLookAt::SetSpeed(float speed)
{
	affector.SetSpeed(speed);	
}

#ifdef ANIM_STANDALONE
void AnimationControllerLookAt::GetEDMatrix(Matrix& mat)
{
	mat = affector.clearMat;
}
#endif

void AnimationControllerLookAt::Update(float dt)
{	
}

void AnimationControllerLookAt::Release()
{

}
