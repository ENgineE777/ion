
#include "Core/Animation/Base/AnimationInstance.h"
#include "AnimationProcessorImp.h"
#include "AnimationResImp.h"
#include "AnimationClipImp.h"
#include "AnimationServiceImp.h"

AnimationProcessorImp::AnimationProcessorImp(AnimationInstance* instance) : AnimationProcessor(instance)
{	
	res = (AnimationResImp*)anim->res;	
}

void AnimationProcessorImp::BonesMaping(Mesh::Bone* gm_bones, int numBones)
{
	bones.clear();
	bonesIndices.Clear();
	root_bone = -1;
	
	if (!String::IsEmpty(res->movementBone.s))
	{		
		for (int i=0;i<numBones; i++)
		{		
			if (String::IsEqual(gm_bones[i].name, res->movementBone.s))
			{
				colapse_to = i;
				break;
			}				
		}		
	}
	
	for (int i=0;i<numBones; i++)
	{		
		int* index = bonesIndices.Add(gm_bones[i].name);
		*index = i;

		bones.push_back(BoneMat());
		BoneMat* boneMat = &bones[bones.size()-1];
	
		boneMat->index = -1;
		boneMat->mc = false;		

		boneMat->parent = gm_bones[i].parent;
		
		boneMat->initInvMat = gm_bones[i].bindMatrix;
		boneMat->curMat = gm_bones[i].bindMatrix;
		boneMat->curMat.Inverse();		

		if (boneMat->parent != -1)
		{				
			boneMat->curLocMat = boneMat->curMat * gm_bones[boneMat->parent].bindMatrix;					
		}
		else
		{
			boneMat->curLocMat = boneMat->curMat;
		}	
		
		boneMat->initMat = boneMat->curLocMat;

		if (res->master_clip)
		{
			for (int j=0;j<(int)res->master_clip->num_bones;j++)
			{
				const char* boneName = res->master_clip->bones[j].name;

				if (String::IsEqual(boneName,gm_bones[i].name))
				{				
					boneMat->index = j;

					//Mesh::AnimKey* key = &res->master_clip->keys[res->master_clip->bones[j].startKey];
									
					boneMat->mc_rot.Set(boneMat->initMat);
					boneMat->mc_pos = boneMat->initMat.Pos();	

					boneMat->mc = true;

					break;
				}
			}		
		}		

		if (i==0 && root_bone<0)
		{
			root_bone = boneMat->index;
		}

		if (i==0 && boneMat->index<0)
		{			
			#ifndef ANIM_STANDALONE
			res->manager->Trace("%s : Error in mapping %s - root bone are not same\n", __FUNCTION__, res->graph_name.s);
			#endif
			//boneMat->index = actor_instance->GetEnabledNode(0);
		}
	}
}

void AnimationProcessorImp::ProcessBones()
{	
	AnimationInstance::RecordedFrame* frame = NULL;

	if (anim->frames.size() != 0)
	{
		frame = &anim->frames[anim->cur_update_rec_frame];

		for (int j=0; j<frame->num_elems; j++)
		{				
			AnimationClipImp* clip_ls = (AnimationClipImp*)anim->rec_clip[j];
			clip_ls->ResetKeys();
		}
	}


	for (int i=0; i<(int)bones.size(); i++)
	{
		BoneMat* boneMat = &bones[i];		

		if (boneMat->mc)
		{		
			Vector pos = boneMat->mc_pos;
			Quaternion rot = boneMat->mc_rot;
			Vector scale = 1.0f;

			int count = anim->player.GetNumPlayed();

			if (frame)
			{
				count = frame->num_elems;
			}
				
			for (int j=0; j<count; j++)
			{				
				AnimationClipImp* clip_ls = NULL;
				Mesh::Animation* clip = NULL;
				Mesh::BoneAnimTrack* bone = NULL;
				AnimationRes::ClipDesc* desc = NULL;

				if (!frame)
				{
					AnimationInstance::PlayElement* elem = anim->player.Get(j);

					clip_ls = (AnimationClipImp*)elem->clip;
					clip = clip_ls->playInfo.clip->clip;
					bone = clip_ls->playInfo.clip->mapping[boneMat->index];
					desc = elem->desc;
				}
				else
				{
					clip_ls = (AnimationClipImp*)anim->rec_clip[j];
					clip = clip_ls->playInfo.clip->clip;
					bone = clip_ls->playInfo.clip->mapping[boneMat->index];
					desc = frame->elems[j].desc;
				}


				if (!bone) continue;

				Vector bone_pos;
				Quaternion bone_rot;				
				Vector bone_scale = 1.0f;
				
				if ( fabs(clip_ls->cur_time - clip_ls->playInfo.clip->length) <0.01f)
				{
					Mesh::BoneKey* key = &bone->keys[0];

					bone_rot = key->rot;
					bone_pos = key->pos;
					bone_scale = key->scale;
				}
				else
				{
					int k = clip_ls->cur_keys[i];

					if (desc->playMode == 0)
					{
						k = clip_ls->cur_keys[i];

						for (k=clip_ls->cur_keys[i]; k<bone->num_keys-2;k++)
						{
							if (bone->keys[k+1].time > clip_ls->cur_time)
							{
								break;
							}
						}						

						clip_ls->cur_keys[i] = k;
					}
					else
					{						
						for (k=clip_ls->cur_keys[i];k<bone->num_keys-2;k++)
						{
							if (bone->keys[bone->num_keys-2 - k].time < clip_ls->cur_time)
							{
								break;
							}
						}

						clip_ls->cur_keys[i] = k;
						k = bone->num_keys-2 - k;
					}					

					float slerp = (clip_ls->cur_time - bone->keys[k].time) / (bone->keys[k+1].time - bone->keys[k].time);
					//slerp = __min(1.0f, __max(0.0f, slerp)); //!!IB - ????????

					//assert(slerp<=1.0f);
					//assert(slerp>=0.0f);
					
					bone_pos.Lerp( bone->keys[k].pos, bone->keys[k+1].pos, slerp);
					bone_rot.SLerp( bone->keys[k].rot, bone->keys[k+1].rot, slerp);
					bone_scale.Lerp( bone->keys[k].scale, bone->keys[k+1].scale, slerp);
				}
				
				if (clip_ls->GetWeight()>0.99f)
				{
					pos = bone_pos;
					rot = bone_rot;
					scale = bone_scale;
				}
				else
				{				
					pos.Lerp( pos, bone_pos, clip_ls->GetWeight());
					rot.SLerp( rot, bone_rot, clip_ls->GetWeight());
					scale.Lerp( scale, bone_scale, clip_ls->GetWeight());
				}			
			}		
						

			rot.GetMatrix(boneMat->curLocMat);			
			//boneMat->curLocMat.Scale(scale);
			boneMat->curLocMat.Pos() = pos;
			
			if ((colapse_to<0 && i==0) && anim->cur_node->getMovement)
			{
				//boxOffset.x = -boneMat->curLocMat._41;
				//boxOffset.z = -boneMat->curLocMat._43;

				cur_pos = Vector(pos.x,0.0f,pos.z);				

				boneMat->curLocMat._41 = 0;				
				boneMat->curLocMat._43 = 0;	
			}			
		}
		else
		{
			boneMat->curLocMat = boneMat->initMat;
		}
	}	

	AnimationProcessor::ProcessBones();

	/*if (anim->player.GetNumPlayed()>0)
	{
		bool inited = false;
		Vector minmax[2];
		minmax[0] = Vector(-2.0f,-2.0f,-2.0f);
		minmax[1] = Vector( 2.0f, 2.0f, 2.0f);

		Vector minmax_tmp[2];

		int count = anim->player.GetNumPlayed();

		if (frame)
		{
			count = frame->num_elems;
		}

		for (int j=0; j<count; j++)
		{			
			AnimationClipImp* clip_ls = NULL;
			Mesh::Animation* clip = NULL;	
			AnimationRes::ClipDesc* desc = NULL;

			if (!frame)
			{
				AnimationInstance::PlayElement* elem = anim->player.Get(j);

				clip_ls = (AnimationClipImp*)elem->clip;
				clip = clip_ls->playInfo.clip->clip;					
				desc = elem->desc;
			}
			else
			{
				clip_ls = (AnimationClipImp*)anim->rec_clip[j];
				clip = clip_ls->playInfo.clip->clip;
				desc = frame->elems[j].desc;
			}			
			
			Vector minmax_tmp[2];
			Vector minmax_tmp2[2];

			minmax_tmp[0] = Vector( 1000000, 1000000, 1000000);
			minmax_tmp[1] = Vector(-1000000,-1000000,-1000000);			

			for (int k=0; k<(int)clip->numChannels; k++)
			{
				Mesh::AnimChannel* channel = &clip->channels[k];

				if (!channel->numBBoxes)
					continue;
						
				if (fabs(clip_ls->cur_time - clip_ls->playInfo.clip->length) <0.01f)
				{
					Mesh::AnimBBox* box = &clip->bboxes[channel->startBBox + channel->numBBoxes - 1];

					minmax_tmp2[0] = box->minMax[0];
					minmax_tmp2[1] = box->minMax[1];			
				}
				else
				{
					int t = clip_ls->cur_bbkeys[k];

					if (desc->playMode == 0)
					{												
						for (t=clip_ls->cur_bbkeys[k];t<(int)(channel->numBBoxes-2);t++)
						{
							if (clip->bboxes[channel->startBBox + t+1].time-clip->bboxes[channel->startBBox].time > clip_ls->cur_time)
							{
								break;
							}
						}

						clip_ls->cur_bbkeys[k] = t;
					}
					else
					{
						t = 0;

						for (t=0;t<(int)(channel->numBBoxes-2);t++)
						{
							if (clip->bboxes[channel->numBBoxes-2 - t + channel->startBBox].time-clip->bboxes[0].time < clip_ls->cur_time)
							{
								break;
							}
						}

						clip_ls->cur_bbkeys[k] = t;
						t = channel->numBBoxes-2 - t;						
					}										

					float slerp = (clip_ls->cur_time - (clip->bboxes[channel->startBBox + t].time - clip->bboxes[0].time)) / (clip->bboxes[channel->startBBox+t+1].time - clip->bboxes[channel->startBBox+t].time);
					//slerp = __min(1.0f, __max(0.0f, slerp)); //!!IB - ????????

					//assert(slerp<=1.0f);
					//assert(slerp>=0.0f);

					minmax_tmp2[0].Lerp(clip->bboxes[channel->startBBox+t].minMax[0], clip->bboxes[channel->startBBox+t+1].minMax[0], slerp);
					minmax_tmp2[1].Lerp(clip->bboxes[channel->startBBox+t].minMax[1], clip->bboxes[channel->startBBox+t+1].minMax[1], slerp);
				}

				minmax_tmp[0].x = MathMin(minmax_tmp2[0].x,minmax_tmp[0].x);	
				minmax_tmp[0].y = MathMin(minmax_tmp2[0].y,minmax_tmp[0].y);	
				minmax_tmp[0].z = MathMin(minmax_tmp2[0].z,minmax_tmp[0].z);	

				minmax_tmp[1].x = MathMax(minmax_tmp2[1].x,minmax_tmp[1].x);	
				minmax_tmp[1].y = MathMax(minmax_tmp2[1].y,minmax_tmp[1].y);	
				minmax_tmp[1].z = MathMax(minmax_tmp2[1].z,minmax_tmp[1].z);			
			}

			if (inited)
			{
				minmax[0].x = MathMin(minmax[0].x,minmax_tmp[0].x);	
				minmax[0].y = MathMin(minmax[0].y,minmax_tmp[0].y);	
				minmax[0].z = MathMin(minmax[0].z,minmax_tmp[0].z);	

				minmax[1].x = MathMax(minmax[1].x,minmax_tmp[1].x);	
				minmax[1].y = MathMax(minmax[1].y,minmax_tmp[1].y);	
				minmax[1].z = MathMax(minmax[1].z,minmax_tmp[1].z);				
			}
			else
			{
				minmax[0] = minmax_tmp[0];
				minmax[1] = minmax_tmp[1];
				inited = true;
			}
		}

		bbox[0] = minmax[0].x - cur_pos.x;
		bbox[1] = minmax[0].y - cur_pos.y;
		bbox[2] = minmax[0].z - cur_pos.z;

		bbox[3] = minmax[1].x - cur_pos.x;
		bbox[4] = minmax[1].y - cur_pos.y;
		bbox[5] = minmax[1].z - cur_pos.z;
	}*/
}

void AnimationProcessorImp::Update(float dt)
{
	ProcessBones();
	AnimationProcessor::Update(dt);	
}

IAnimationClip* AnimationProcessorImp::CreateClip()
{
	return new AnimationClipImp();
}

bool AnimationProcessorImp::InitClip(IAnimationClip* clip,void* ds, float inTime)
{
	AnimationRes::ClipDesc* desc = (AnimationRes::ClipDesc*)ds;

	AnimationClipImp::PlayBackInfo playInfo;
		
	playInfo.mix = desc->mix;
	playInfo.playSpeed = desc->playSpeed;	
	
	if (inTime<0.0f)
	{
		playInfo.timeIn = desc->timeIn;
	}
	else
	{
		playInfo.timeIn = inTime;
	}

	playInfo.timeOut = desc->timeOut;
	playInfo.playMode = desc->playMode;
		
	playInfo.additiveBlend = desc->additiveBlend;	

	const char* clip_name = anim->SeletClipNameFromTable(desc->clip_name.Select());

	if (!clip_name)
	{
		clip_name = desc->clip_name.Select();
	}

	AnimationResImp::Clip* clp = res->ls_clips.Get(clip_name);

	if (clp)
	{	
		playInfo.clip = clp;
		((AnimationClipImp*)clip)->Init(&playInfo);

		return true;
	}

	return false;
}

bool AnimationProcessorImp::UpdateClip(void* cl, float dt)
{
	AnimationClipImp* clip = (AnimationClipImp*)cl;
	
	if (clip->target_weight >= 0.0f)
	{
		clip->cur_weight += clip->weight_delta * dt;

		if (clip->weight_delta > 0.0f)
		{				
			if (clip->cur_weight > clip->target_weight)
			{
				clip->cur_weight = clip->target_weight;
				clip->target_weight = -1.0f;
			}
		}
		else
		{
			if (clip->cur_weight < clip->target_weight)
			{
				clip->cur_weight = clip->target_weight;
				clip->target_weight = -1.0f;							
			}
		}
	}

	if (clip->paused) return true;	

	if (fabs(clip->playInfo.playSpeed)<0.01f) return true;

	if (clip->playInfo.clip->length<0.01f)
	{
		clip->cur_time = 0;
		return true;
	}

	clip->cur_time += dt * clip->playInfo.playSpeed;

	clip->elem->owner->ProcessEvents(clip->elem, 0, NULL);

	if (clip->playInfo.playMode == 0)
	{
		if (clip->cur_time >= clip->playInfo.clip->length)
		{
			if (clip->elem->owner->DecNumPlays(clip->elem))
			{
				clip->elem->cur_event = 0;				
				float nums = (int)(clip->cur_time / clip->playInfo.clip->length);
				clip->cur_time -= clip->playInfo.clip->length * nums;

				clip->elem->owner->ProcessEvents(clip->elem, 0, NULL);				

				clip->ResetKeys();
			}
			else
			{
				//if (anim->num_active_top_layers)
				clip->cur_time = clip->playInfo.clip->length;
				return false;
			}
		}
	}
	else
	{
		if (clip->cur_time < 0.0f)
		{			
			if (clip->elem->owner->DecNumPlays(clip->elem))
			{
				clip->elem->cur_event = (int)clip->elem->desc->events.size()-1;
				float nums = (int)(clip->cur_time / clip->playInfo.clip->length) + 1.0f;
				clip->cur_time += clip->playInfo.clip->length * nums;

				clip->elem->owner->ProcessEvents(clip->elem, 0, NULL);

				clip->ResetKeys();
			}
			else
			{
				clip->cur_time = 0.0f;
				return false;
			}
		}		
	}

	return true;
}