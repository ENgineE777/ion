
#include "Core/Core.h"

#include "Core/Animation/Base/AnimationInstance.h"
#include "AnimationServiceImp.h"
#include "AnimationProcessorImp.h"
#include "Core/Animation/Base/AnimationControllerLookAt.h"
#include "Core/Animation/Base/AnimationControllerIK.h"

#include "stdarg.h"

void DrawSkeleton(AnimationInstance* anim)
{	
	/*for (int i=0; i< (int)anim->processor->bones.size(); i++)
	{
		AnimationProcessor::BoneMat* boneMat = &anim->processor->bones[i];

		if (boneMat->parent != -1)
		{	
			const Matrix* mt;
			Vector pos1, pos2;
			
			mt = anim->GetBoneMatrixMulGlTrans(i);
			pos1 = mt->Pos();

			
			mt = anim->GetBoneMatrixMulGlTrans(boneMat->parent);
			pos2 = mt->Pos();
						
			core->Render()->DrawLine(pos1, COLOR_YELLOW, pos2, COLOR_YELLOW, true);				
		}
	}*/
}

AnimationServiceImp::AnimationServiceImp(): AnimationService()
{	
}

AnimationServiceImp::~AnimationServiceImp()
{
	ReleaseRes();
}

bool AnimationServiceImp::Init()
{
	return true;
}

IAnimationInstance* AnimationServiceImp::LoadAnimation(const char* name, const char* file, int line)
{	
	if (!name[0]) return null;

	AnimationResImp* anim = (AnimationResImp*)animations.Get(name);

	if (!anim)
	{		
		anim = new AnimationResImp(this);
		
		if (!anim->Load(name))
		{
			delete anim;
			anim = NULL;
		}

		if (anim)
		{
			animations.Add(name,anim);
		}
	}	

	if (anim)
	{				
		return AddAnim(anim);
	}

	return NULL;
}

IAnimationProcessor* AnimationServiceImp::CreateAnimationProcessor(AnimationInstance* instance)
{
	return new AnimationProcessorImp(instance);
}

IAnimationController* AnimationServiceImp::CreateController(AnimationInstance* instance,
														   IAnimationController::ControllerType type,
														   const char* boneName, const char* boneOffsetName,
														   const Vector& min,const Vector& max, float speed)
{	
	IAnimationController* cnt = NULL;
	bool res = false;

	AnimationProcessor* anim_proc = (AnimationProcessor*)instance->processor;

	if (type == IAnimationController::lookat)
	{
		AnimationControllerLookAt* cntrl = new AnimationControllerLookAt();		
		res = cntrl->Init(anim_proc, type, boneName, boneOffsetName, min, max, speed);
		cnt = cntrl;
	}
	else
	{
		AnimationControllerIK* cntrl = new AnimationControllerIK();		
		res = cntrl->Init(anim_proc, type, boneName, boneOffsetName, min, max, speed);
		cnt = cntrl;
	}

	if (res)
	{
		return cnt;
	}

	delete cnt;	
	return NULL;
}

void AnimationServiceImp::Update(float dt)
{		

#if defined(PC)
	//CStopWatch timer;	
	//timer.Start();
#endif

	AnimationService::Update(dt);

#if defined(PC)

	//timer.Stop();
	//long double duration = timer.GetDuration();

#ifndef ANIM_STANDALONE
	//g_GameProxy.DebugFont->addText(-1.0f, 0.8f, "Animation Time: %f" , duration);
#endif

#endif

#ifndef ANIM_STANDALONE
	if (IsDrawSkeleton())
	{
		for (int i=0;i<(int)anim_instances.size();i++)
		{
			DrawSkeleton(anim_instances[i]);
		}
	}
#endif	
}

void AnimationServiceImp::Trace(const char* formated, ...)
{
	static char buffer[4096];

	buffer[0] = 0;

	va_list	args;

	va_start( args, formated );
	vsnprintf( buffer, sizeof( buffer ), formated, args );	
	va_end( args );

#if !defined(ANIM_STANDALONE)
		core->TraceTo("Animation", buffer);		
#endif	
}

void AnimationServiceImp::ReleaseRes()
{	
	animations.Clear();
}

#ifdef PC
void AnimationServiceImp::Save(const char* mdl, const char* path, Platform platform)
{
	Clip* clip = clips.Get(mdl);

	if (clip)
	{
		File* file = new File();

		if (file->Init(path, File::write))
		{
			file->Write(&Mesh::animMagicID, sizeof(Mesh::animMagicID));
			file->Write(&Mesh::animSubID, sizeof(Mesh::animSubID));

			file->Write(&clip->anim->num_bones, sizeof(int));
			file->Write(&clip->anim->time_lenght, sizeof(float));

			file->Write(clip->anim->bones, sizeof(Mesh::BoneAnimTrack) * clip->anim->num_bones);

			for (int i=0;i<clip->anim->num_bones;i++)
			{
				file->Write(clip->anim->bones[i].keys, sizeof(Mesh::BoneKey) * clip->anim->bones[i].num_keys);
			}

			file->Release();
		}
	}
}
#endif