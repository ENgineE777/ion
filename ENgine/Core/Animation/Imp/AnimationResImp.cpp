
#include "AnimationResImp.h"
#include "AnimationServiceImp.h"
#include "Core/Core.h"

AnimationResImp::AnimationResImp(AnimationService* mngr) : AnimationRes(), ls_clips(_FL_)
{
	manager = mngr;
	master_clip = NULL;
}

bool AnimationResImp::PreLoad(const char* name)
{	
	ClipDesc desc;
	desc.clip_name.Init(name, "");
	desc.channels.resize(5);
	desc.channels[0].Copy("Head");
	desc.channels[1].Copy("Hnd");
	desc.channels[2].Copy("Weapon");
	desc.channels[3].Copy("Spine");
	desc.channels[4].Copy("Lg");

	LoadClip( name, &desc);

	return true;
}

void AnimationResImp::LoadClip(const char* name, ClipDesc* desc)
{		
	if (!ls_clips.Get(name))	
	{		
		Mesh::Animation* clip = manager->LoadClip(name);
									
		if (clip)
		{			
			Clip* ls_clip = ls_clips.Add(name);

			ls_clip->clip = clip;			

			if (clip->num_bones>0)
			{
				ls_clip->length = clip->time_lenght;
			}
			else
			{
				ls_clip->length = 0.0f;
			}

			if (!master_clip)
			{
				master_clip = clip;
			}

			for (int i=0; i<(int)master_clip->num_bones; i++)
			{					
				Mesh::BoneAnimTrack* bone = NULL;
				
				/*for (int k=0; k<(int)clip->numChannels; k++)
				{
					Mesh::AnimChannel* channel = &clip->channels[k];

					bool skip = true;

					for (int l=0; l<(int)desc->channels.size(); l++)
					{
						if (CrossString::IsEqual(desc->channels[l].s, channel->chanelName))
						{
							skip = false;
							break;
						}						
					}

					if (!skip)// || desc->channels.size() == 0)
					{						
						for (int l=0; l<(int)channel->numBones; l++)
						{
							if (CrossString::IsEqual(master_clip->bones[i].boneName,clip->bones[channel->startBone + l].boneName))
							{
								bone = &clip->bones[channel->startBone + l];
								break;
							}
						}							
					}

					if (bone) break;
				}*/

				for (int l=0; l<(int)clip->num_bones; l++)
				{
					if (String::IsEqual(master_clip->bones[i].name,clip->bones[l].name))
					{
						bone = &clip->bones[l];
						break;
					}
				}	

				ls_clip->mapping.push_back(bone);
			}
		}					
		else
		{			
			manager->Trace("%s: failed to open anim file, %s\n", __FUNCTION__, name);		
		}
	}
}

bool AnimationResImp::ClipsLoaded()
{
	if (ls_clips.Size()==0) return false;

	return true;
}

void AnimationResImp::Release()
{	
	for (int i=0;i<ls_clips.Size();i++)
	{
		Clip* clip = ls_clips.Get(i);	
		
		manager->DeleteClip(clip->clip);		

		delete clip;
	}

	ls_clips.Clear(false);

	AnimationRes::Release();

	delete this;
	//DEL(this,AnimationResImp)
}