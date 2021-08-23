
#include "AnimationService.h"
#include "AnimationInstance.h"
#include "AnimationRes.h"
#include "Core/Core.h"
#include "AnimationFbxLoader.h"

AnimationService::AnimationService() :
animations(_FL_),
clips(_FL_)
{
	debugOn = false;
	drawSkeletonOn = false;	
}

void AnimationService::Update(float dt)
{
	for (int i=0;i<(int)anim_instances.size();i++)
	{
		if (!anim_instances[i]->IsPaused())
		{
			anim_instances[i]->Update(dt);
		}

		#if !defined(PSP) && !defined(ANIM_STANDALONE)

		#if defined(PC)
		
		if (debugOn)
		{					
			Matrix* mat = &((AnimationProcessor*)anim_instances[i]->processor)->globalMatrix;
			//core->Render()->PrintText3D(mat->Pos(), 15.0f, COLOR_YELLOW,"Node: %s", anim_instances[i]->CurrentNodeName());			
		}

		#endif

		#endif
	}

/*
	core->Render()->PrintText(512, 384, up_left, COLOR_RED, "AnimService: animations - %d, instances - %d",
		                                                     animations.Size(), anim_instances.size());

	core->Render()->PrintText(512, 400, up_left, COLOR_RED, "Render: textures - %d, vshaders - %d",
		                                                     core->Render()->GetTexturesMapSize(),
														     core->Render()->GetVShadersMapSize());
	
	core->Render()->PrintText(512, 416, up_left, COLOR_RED, "Render: pshaders - %d, techniques - %d",
															 core->Render()->GetPShadersMapSize(),
															 core->Render()->GetTechniquesMapSize());

	core->Render()->PrintText(512, 432, up_left, COLOR_RED, "Model: models - %d", core->Model()->GetModelsMapSize());
	*/
}

AnimationInstance* AnimationService::AddAnim(AnimationRes* anim)
{
	AnimationInstance* instance = anim->CreateReference();
	anim_instances.push_back(instance);

	return instance;
}

void AnimationService::ReleaseAnim(AnimationInstance* anim)
{	
	for (int i=0; i < (int)anim_instances.size();i++)
	{
		if (anim == anim_instances[i])
		{
			for (int j=i; j < (int)anim_instances.size()-1;j++)
			{
				anim_instances[j] = anim_instances[j+1];
			}

			anim_instances.pop_back();

			break;
		}		
	}

	if (anim->res->DecRef())
	{		
		animations.Delete(anim->res, false);
		anim->res->Release();
	}	
}

Mesh::Animation* AnimationService::LoadClip(const char* name)
{
	if (!name[0]) return null;

	Clip* clip = clips.Get(name);

	if (!clip)
	{		
		Mesh::Animation* anim = new Mesh::Animation();
				
		FileBuffer* file = null;

		bool res = false;
#ifdef FBX
		res = AnimationFbxLoader::Load(anim, name);
		if (res) core->Files()->RegFile(name);
#endif
		if (!res)
		{
			FileBuffer* file = core->Files()->LoadFile(name);

			if (file)
			{
				byte* ptr = file->GetBuffer();

				if (*((dword*)&ptr[0]) == Mesh::animMagicID &&
					*((dword*)&ptr[4]) == Mesh::animSubID)
				{
					res = true;

					anim->num_bones = *((dword*)&ptr[8]);
					anim->time_lenght = *((float*)&ptr[12]);

					ptr += 16;

					anim->bones = (Mesh::BoneAnimTrack*)ptr;
					ptr += sizeof(Mesh::BoneAnimTrack) * anim->num_bones;

					for (int i=0;i<anim->num_bones;i++)
					{
						anim->bones[i].keys = (Mesh::BoneKey*)ptr;
						ptr += sizeof(Mesh::BoneKey) * anim->bones[i].num_keys;
					}
				}
				else
				{
					file->Release();
					file = null;
				}				

				res = true;
			}
		}


		if (!res)
		{
			delete anim;
			anim = null;
		}

		if (!anim) return null;

		clip = clips.Add(name);
		clip->ref = 0;
		clip->anim = anim;
		clip->buffer = file;
	}

	clip->ref++;
	return clip->anim;
}

void AnimationService::DeleteClip(Mesh::Animation* anim)
{
	for (int i=0; i < (int)clips.Size();i++)
	{
		Clip* clip = clips.Get(i);

		if (clip->anim == anim)
		{
			clip->ref--;

			if (!clip->ref)
			{
				if (clip->buffer)
				{
					clip->buffer->Release();
				}
				else
				{
					if (clip->anim->num_bones)
					{
						for (int j=0; j<clip->anim->num_bones;j++)
						{
							if (clip->anim->bones[j].num_keys)
							{
								delete[] clip->anim->bones[j].keys;
							}
						}

						delete[] clip->anim->bones;
					}
					delete clip->anim;
				}

				clips.Delete(i);
			}

			break;
		}		
	}
}

void AnimationService::Trace(const char* formated, ...)
{

}

void AnimationService::SetDebug(bool enable)
{
	debugOn = enable;
}

bool AnimationService::IsDebug()
{
	return debugOn;
}

void AnimationService::SetDrawSkeleton(bool enable)
{
	 drawSkeletonOn = enable;
}

bool AnimationService::IsDrawSkeleton()
{
	return drawSkeletonOn;
}
