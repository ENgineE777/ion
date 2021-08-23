
#include "AnimationInstance.h"
#include "AnimationService.h"
#include "Core/Core.h"

AnimationInstance::AnimationInstance(AnimationRes* rs) : controllers(_FL_)
{
	res = rs;
	em_events = NULL;

	cur_node = NULL;

	cur_played_top_player = -1;
	num_active_top_layers = 0;

	actived_link = NULL;
	
	active_table = NULL;

	paused = true;		

	cur_rec_time = 0.0f;
	cur_rec_frame = 0;

	cur_update_rec_frame = 0;

	freq = 0.001f;
	update_time = 0.0f;

	bonesReady = false;

	recording = false;
}

AnimationInstance::~AnimationInstance()
{

}

void AnimationInstance::Init()
{	
	processor = (AnimationProcessor*)res->manager->CreateAnimationProcessor(this);		
	
	player.Init(this, processor);	

	for (int i=0;i<8;i++)
	{
		rec_clip[i] = (AnimationClip*)processor->CreateClip();		
	}
	

	for (int i=0;i<res->controllers.Size();i++)
	{
		AnimationRes::Controller* cntrl = res->controllers.Get(i);		
		AddController(cntrl->type, cntrl->id.s,cntrl->boneName.s, cntrl->boneOffsetName.s,cntrl->min, cntrl->max, cntrl->speed);
	}

#ifdef ANIM_AFFECT_PROP

	for (int i=0;i<res->affectors.Size();i++)
	{
		AnimationRes::AffectorParam* affector_param = res->affectors.Get(i);
				
		for (affecorDeclaratorBase* decl = affecorDeclaratorBase::GetFirst(); decl != NULL; decl = decl->GetNext())
		{
			if (strcmp(decl->Name(), affector_param->name.s) == 0)
			{
				IAnimationAffector* affector = decl->Create();				
				AddAffector(affector, 0);

				affector->Init();
				affector->ReInit();

				for (int j=0;j<affector_param->params.Size();j++)
				{
					AnimationRes::Param* param = affector_param->params.Get(j);

					int index = affector->GetPropIndex(param->name.s);

					if (index>=0)
					{					
						if (param->type == 0)
						{
							affector->SetPropAsFloat(index, param->flt_val);							
						}
						else
						{
							affector->SetPropAsString(index, param->str_val.s);
						}
					}
				}

				break;
			}
		}
	}
#endif

	ActivateTableNames(res->startTable.s);
	Restart();	
}

void AnimationInstance::CheckLinkActivation()
{
	if (!actived_link) return;

	if (player.GetNumPlayed()>0 && actived_link->activeTime>0.0f)
	{
		float dt = 0.0f;

		if (cur_played_top_player >= 0.0f)
		{			
			dt = cur_played_top_player;
		}

		for (int i=0;i<player.GetNumPlayed();i++)
		{
			if (!player.Get(i)->topLayer) continue;

			float delta = player.Get(i)->clip->GetCurTime();

			if (actived_link->activeTime-dt > delta)
			{
				return;
			}

			break;
		}		
	}
	
	GotoNode(actived_link->destination.s, actived_link->blendTime);
}

void AnimationInstance::ActivateLink(const char* name, bool ignoreActivated)
{		
	ActivateLink(HashValue(name), ignoreActivated);	
}

void AnimationInstance::ActivateLink(unsigned int  hash, bool ignoreActivated)
{
	if (actived_link && !ignoreActivated) return;

	AnimationRes::AnimLink* link = cur_node->links.Get(hash);

	if (link)
	{
		actived_link = link;
		CheckLinkActivation();
	}
	else
	{				
		//res->manager->Trace("Warrning in AnimGraph %s: - can't Activate Link in Node %s", res->graph_name.s, cur_node->name.s);
	}	
}

bool AnimationInstance::TestActivateLink(const char* name)
{
	if (cur_node->links.Get(name))
	{
		return true;
	}

	return false;
}

bool AnimationInstance::TestActivateLink(unsigned int hash)
{
	if (cur_node->links.Get(hash))
	{
		return true;
	}

	return false;
}

const char* AnimationInstance::CurrentNodeName()
{
	return cur_node->name.s;
}

const char* AnimationInstance::GetNodeConstant(const char* name, const char* nodeName)
{
	AnimationRes::NodeConstant* cnst = NULL;

	if (!nodeName)
	{
		cnst = cur_node->constants.Get(name);
	}
	else
	{
		AnimationRes::AnimNode* node = res->nodes.Get(nodeName);

		if (node)
		{
			cnst = node->constants.Get(name);
		}
	}

	if (!cnst) return NULL;

	return cnst->value.s;
}

void AnimationInstance::Pause(bool enable)
{
	paused = enable;
}

bool AnimationInstance::IsPaused()
{
	return paused;
}

void AnimationInstance::Restart()
{
	((AnimationProcessor*)processor)->allow_blend = false;

	//Pause(false);
	StopPlayer(true, 0.0f);

	for (int i=0;i<controllers.Size();i++)
	{
		IAnimationController* cntrl = controllers.Get(i);		
		cntrl->SetWeight(0.0f, 0.0f);		
	}

	cur_node = NULL;
	GotoNode(res->startNode.s, 0.0f);	
}

bool AnimationInstance::ActivateTableNames(const char* name)
{
	AnimationRes::NamesTable* table = res->namesTables.Get(name);

	if (table)
	{
		active_table = table;
		return true;
	}

	return false;
}

void AnimationInstance::GotoNode(const char* name, float blendTime)
{
	GotoNode(res->nodes.Get(name), blendTime);	
}

void AnimationInstance::ResetNode(float blendTime)
{
	AnimationRes::AnimNode* node = cur_node;

	if (node)
	{
		cur_node = null;
		GotoNode(node, blendTime);
	}	
}

void AnimationInstance::GotoNode(unsigned int hash, float blendTime)
{
	GotoNode(res->nodes.Get(hash), blendTime);
}

bool AnimationInstance::PlayLayer(const char* name)
{
	return PlayLayer(HashValue(name));	
}

bool AnimationInstance::PlayLayer(unsigned int hash)
{			
 	if (cur_node->topLayers.Get(hash)) return false;	

	if (!player.Get(hash))
	{	
		return PlayAnimation(hash, false, -1.0f, -1.0f);
	}

	return false;
}

bool AnimationInstance::IsLayerPlaying(const char* name)
{
	return IsLayerPlaying(HashValue(name));
}

bool AnimationInstance::IsLayerPlaying(unsigned int hash)
{
	return cur_node->topLayers.Get(hash) || player.Get(hash);
}

bool AnimationInstance::PlayAnimation(unsigned int hash, bool topLayer, float blendTime, float syncTime)
{
	AnimationRes::ClipDesc* clip = cur_node->layers.Get(hash);

	if (clip)
	{			
		return PlayAnimation(cur_node->layers.GetName(hash), clip, topLayer, blendTime, syncTime);
	}

	return false;
}

void AnimationInstance::StopLayer(const char* name, float time)
{
	StopLayer(HashValue(name), time);
}

void AnimationInstance::StopLayer(unsigned int hash, float time)
{	
	if (cur_node->topLayers.Get(hash)) return;	

	PlayElement* elem = player.Get(hash);

	if (elem)
	{
		elem->clip->Stop(time);
	}
}

bool AnimationInstance::DecNumPlays(PlayElement* elem)
{
	if (elem->topLayer)
	{		
		elem->owner->ResetDeltaPos();
	}
	
	if (elem->num_plays<9000)
	{
		elem->num_plays--;
	}			

	ProcessEvents(elem, 2, null);
	
	if (elem->num_plays==0)
	{
		elem->owner->StopLayer(elem);
		return false;
	}

	if (elem->desc->playMode == 0)
	{
		elem->cur_event = 0;		
	}
	else
	{
		elem->cur_event = (int)elem->desc->events.size()-1;;
	}

	return true;
}

void AnimationInstance::StopLayer(PlayElement* elem)
{
	if (elem->topLayer)
	{
		num_active_top_layers--;
	}

	if (elem->desc->playMode==0)
	{
		elem->clip->SetCurTime(1.0f);
	}
	else
	{
		elem->clip->SetCurTime(0.0f);
	}
	
	elem->clip->Pause(true);

	if (num_active_top_layers==0)
	{				
		if (cur_played_top_player >= 0.0f)
		{			
			cur_played_top_player++;

			if (cur_played_top_player<cur_node->topLayers.Size())
			{				
				elem->clip->Stop(0.0f);			
				
				player.Delete(elem);
				ResetDeltaPos(true);

				processor->StartBlend( 0.2f, false);
				
				PlayAnimation(HashValue(cur_node->topLayers.Get(cur_played_top_player)->s), true, 0.0f, -1.0f);

				return;
			}
		}		
		
		if (actived_link != NULL)
		{
			GotoNode(actived_link->destination.s, actived_link->blendTime);
			return;
		}

		if (!String::IsEmpty(cur_node->defLink.destination.s))
		{
			const char* dest = cur_node->defLink.destination.s;			
			GotoNode(cur_node->defLink.destination.s, cur_node->defLink.blendTime);			
			if (em_events) em_events->OnActiveDefaultLink(dest);
		}

		return;
	}

	elem->clip->Stop();
	return;
}

bool AnimationInstance::SetLayerWeight(const char* name, float weight, float time)
{
	return SetLayerWeight(HashValue(name), weight, time);
}

bool AnimationInstance::SetLayerWeight(unsigned int hash, float weight, float time)
{
	PlayElement* elem = player.Get(hash);

	if (elem)
	{
		if (time<0.0f)
		{
			time = elem->desc->timeIn;
		}

		elem->clip->SetWeight(weight, time);
		return true;
	}

	return false;
}

float AnimationInstance::GetLayerWeight(const char* name)
{
	return GetLayerWeight(HashValue(name));
}

float AnimationInstance::GetLayerWeight(unsigned int hash)
{
	PlayElement* elem = player.Get(hash);

	if (elem)
	{
		return elem->clip->GetWeight();		
	}

	return -1.0f;
}

void AnimationInstance::SetLayerPlaySpeed(const char* name, float speed)
{	
	SetLayerPlaySpeed(HashValue(name), speed);
}

void AnimationInstance::SetLayerPlaySpeed(unsigned int hash, float speed)
{	
	PlayElement* elem = player.Get(hash);

	if (elem)
	{
		elem->clip->SetPlaySpeed(speed);
	}
}

void AnimationInstance::SetLayerPosition(const char* name, float pos)
{
	SetLayerPosition(HashValue(name), pos);
}

void AnimationInstance::SetLayerPosition(unsigned int hash, float pos)
{
	PlayElement* elem = player.Get(hash);

	if (elem)
	{
		elem->clip->SetCurTime(pos);
	}
}

void AnimationInstance::GetMovement(Matrix& mat)
{	
	processor->GetMovement(mat);
}

bool AnimationInstance::IsNodeHasMovement()
{
	if (cur_node)
	{
		if (cur_node->getMovement>0) return true;
	}

	return false;
}

void AnimationInstance::BonesMaping(Mesh::Bone* gm_bones, int numBones)
{
	processor->BonesMaping( gm_bones, numBones);	
	player.Prepare();

	for (int i=0;i<8;i++)
	{		
		rec_clip[i]->Prepare(this);
	}
}

void AnimationInstance::GotoNode(AnimationRes::AnimNode* node, float blendTime)
{
	if (!node) return;
	if (cur_node == node) return;

	float syncTime = -1.0f;

	if (actived_link)
	{
		if (actived_link->syncTiming>0)
		{
			if (player.GetNumPlayed()>0)
			{						
				syncTime = player.Get(0)->clip->GetCurTime();

				if (actived_link->syncTiming==2)
				{
					syncTime = 1 - syncTime;
				}

				if (player.Get(0)->desc->playMode == 1)
				{
					syncTime = 1 - syncTime;
				}
			}
		}
				
		/*res->manager->Trace("AnimGraph \"%s\": Activate Link \"%s\" in Node \"%s\" dest - \"%s\"", res->graph_name.s, cur_node->links.GetName(actived_link), cur_node->name.s, actived_link->destination.s);		
		
		if (stricmp("SitCoverRighUpShootReload", actived_link->destination.s) == 0)
		{
			int k=0;
			k++;
		}*/
	}

	cur_node = node;
		
	StopPlayer(false, blendTime);
	actived_link = NULL;	

	for (int i=0;i<controllers.Size();i++)
	{				
		controllers.Get(i)->Activate(true);
	}

	for (int i=0;i<res->controllers.Size();i++)
	{				
		AnimationRes::Controller* cntrl = res->controllers.Get(i);

		AnimationRes::ControllerParam* param = cur_node->controllerParams.Get(cntrl->id.s);
			
		IAnimationController* controller = controllers.Get(cntrl->id.s);
				
		if (controller)
		{
			if (param)
			{
				if (param->act_set)
				{
					controller->Activate(param->active);					
				}
				else
				{
					controller->Activate(cntrl->active);					
				}

				if (param->constr_set)
				{
					SetControllerConstraintsImpl(controller, param->min, param->max);
				}
				else
				{
					SetControllerConstraintsImpl(controller, cntrl->min, cntrl->max);
				}

				if (param->rot_set)
				{
					SetControllerPreRotationImpl(controller, param->postRotMat);					
				}
				else
				{
					SetControllerPreRotationImpl(controller, cntrl->postRotMat);					
				}

				if (param->bend_set)
				{
					SetControllerBendDirectionImpl(controller, param->bendDir);
				}
				else
				{
					SetControllerBendDirectionImpl(controller, cntrl->bendDir);
				}

				if (param->speed_set)
				{
					SetControllerSpeedImpl(controller, param->speed);
				}
				else
				{
					SetControllerSpeedImpl(controller, cntrl->speed);
				}				
			}
			else
			{
				controller->Activate(cntrl->active);				

				SetControllerConstraintsImpl(controller, cntrl->min, cntrl->max);
				SetControllerPreRotationImpl(controller, cntrl->postRotMat);
				SetControllerSpeedImpl(controller, cntrl->speed);
				SetControllerBendDirectionImpl(controller, cntrl->bendDir);
			}
		}
	}

	if (cur_node->topLayersPlayMode > 0)
	{
		cur_played_top_player = 0;		

		PlayAnimation(HashValue(cur_node->topLayers.Get(cur_played_top_player)->s), true, 0.0f, syncTime);		
	}
	else
	{
		cur_played_top_player = -1;		

		for (int i=0;i<cur_node->topLayers.Size();i++)
		{								
			PlayAnimation(HashValue(cur_node->topLayers.Get(i)->s), true, 0.0f, syncTime);
		}
	}	

	for (int i=0;i<(int)pre_player.size();i++)
	{		
		PrePlayElement* el = &pre_player[i];
		if (PlayAnimation(el->name.s, el->desc, false, 0.0f, 0))
		{
			PlayElement* pl_el = player.Get(player.GetNumPlayed()-1);
			//pl_el->cur_event = el->cur_event;

			float tm = el->playdata.time;
			if (pl_el->desc->playMode == 0)
			{
				for (int j = 0; j<(int)pl_el->desc->events.size(); j++)
				{
					AnimationRes::Event* evnt = &pl_el->desc->events[j];

					if (tm>=evnt->time)
					{
						pl_el->cur_event++;
					}
				}
			}
			else
			{
				for (int j = pl_el->cur_event; j>=0; j--)
				{
					AnimationRes::Event* evnt = &pl_el->desc->events[j];

					if (tm<=evnt->time)
					{
						pl_el->cur_event--;
					}
				}
			}

			pl_el->clip->SetPlayData(el->playdata);			
		}
	}

	pre_player.clear();


	/*bool full = false;

	if (cur_node->getMovement == 0)
	{
		full = true;
	}
	else
	if (player.GetNumPlayed()>0)
	{
		if (player.Get(0)->clip->GetMaxTime()<0.75f)
		{
			full = true;
		}
	}
	else
	if (player.GetNumPlayed()==0)
	{
		full = true;
	}*/

	ResetDeltaPos(true);
}

void AnimationInstance::CalcBones()
{
	processor->ProcessBones();	
}

int AnimationInstance::GetNumBones()
{
	return processor->GetNumBones();
}

int AnimationInstance::GetBoneIndex(const char* name)
{
	return processor->GetBoneIndex(name);
}

int AnimationInstance::GetBoneParentIndex(int index)
{
	return processor->GetBoneParentIndex(index);	
}

Matrix* AnimationInstance::GetBoneMatrix(int index)
{
	if (index<0) return NULL;
	return processor->GetBoneMatrix(index);	
}

Matrix* AnimationInstance::GetBoneLocalMatrix(int index)
{
	if (index<0) return NULL;
	return processor->GetBoneLocalMatrix(index);	
}

Matrix* AnimationInstance::GetBoneMatrixMulGlTrans(int index)
{
	if (index<0) return NULL;

	const Matrix* tr = processor->GetBoneMatrix(index);
	
	if (tr)
	{
		res_mat = *tr * processor->globalMatrix;
		return &res_mat;
	}

	return NULL;	
}

Matrix* AnimationInstance::GetBoneDelta(int index)
{
	if (index<0) return NULL;
	return processor->GetBoneDelta(index);
}


float* AnimationInstance::GetBBox()
{
	return processor->GetBBox();
}

void AnimationInstance::SetEvents(IAnimationEvents* events)
{
	em_events = events;
}

void AnimationInstance::SetTransform(Matrix& transform)
{	
	processor->SetTransform(transform);	

#ifndef PC

	/*Vector pos;
	core->Render()->GetCamPos(pos);
	pos -= transform.Pos();
	float len = pos.Length();
	float blend = 0.0f;

	if (len<7)
	{
		blend = 0.0f;		
	}
	else
	if (len<50)
	{
		blend = (len - 7.0f) / 43.0f;
	}
	else
	{
		blend = 1.0f;
	}

	freq = 0.02f + 0.025f * blend;*/

	//freq = 0.5f;

#endif


	//DrawDebug();
}

void AnimationInstance::GetTransform(Matrix& transform)
{
	transform = processor->globalMatrix;
}

void AnimationInstance::AddController(IAnimationController::ControllerType type,
									  const char* name, const char* boneName,const char* boneOffsetName, 
									  Vector& min, Vector& max, float speed)
{
	if (controllers.Get(name)) return;

	IAnimationController* cntrl = res->manager->CreateController(this, type, boneName, boneOffsetName, min, max, speed);

	if (cntrl)
	{
		controllers.Add(name, cntrl);
	}
}

const char* AnimationInstance::GetControllerName(int index)
{
	return controllers.GetName(index);
}

int AnimationInstance::GetControllersCount()
{
	return controllers.Size();
}

void AnimationInstance::SetMovementBone(const char* bone)
{
	processor->SetMovementBone(bone);
}

#ifdef ANIM_STANDALONE
void AnimationInstance::GetControllerEDMatrix(const char* id, Matrix& mat)
{
	IAnimationController* cntrl = controllers.Get(id);

	if (cntrl)
	{
		cntrl->GetEDMatrix(mat);
	}
}
#endif

void AnimationInstance::SetControllerWeight(int index,float weight,float time)
{	
	SetControllerWeightImpl(controllers.Get(index), weight, time);
}

void AnimationInstance::SetControllerWeight(const char* name,float weight,float time)
{
	SetControllerWeightImpl( controllers.Get(name), weight, time);	
}

void AnimationInstance::SetControllersWeight(float weight,float time)
{
	for (int j=0; j<(int)controllers.Size();j++)
	{
		SetControllerWeightImpl( controllers.Get(j), weight, time);	
	}
}

void AnimationInstance::SetControllerWeightImpl(IAnimationController* controller,float weight,float time)
{
	if (!controller) return;	
	controller->SetWeight(weight,time);	
}

float AnimationInstance::GetControllerWeight(int index)
{
	return GetControllerWeightImp(controllers.Get(index));
}

float AnimationInstance::GetControllerWeight(const char* name)
{
	return GetControllerWeightImp(controllers.Get(name));
}

float AnimationInstance::GetControllerWeightImp(IAnimationController* controller)
{
	if (!controller) return -1.0f;	
	return controller->GetWeight();
}

void AnimationInstance::SetControllerGoal(int index,const Vector& goal)
{
	SetControllerGoalImpl( controllers.Get(index), goal);
}

void AnimationInstance::SetControllersGoal(const Vector& goal)
{
	for (int j=0; j<(int)controllers.Size();j++)
	{
		SetControllerGoalImpl( controllers.Get(j), goal);
	}
}

void AnimationInstance::SetControllerGoal(const char* name,const Vector& goal)
{
	SetControllerGoalImpl( controllers.Get(name), goal);	
}

void AnimationInstance::SetControllerGoalImpl(IAnimationController* controller,const Vector& goal)
{
	if (controller) controller->SetGoal(goal);// + processor->GetRootOffset());	
}

void AnimationInstance::SetControllerAdditionalParameter(int index, void* userData)
{
	SetControllerAdditionalParameterImpl( controllers.Get(index), userData);
}

void AnimationInstance::SetControllersAdditionalParameter(void* userData)
{
	for (int j=0; j<(int)controllers.Size();j++)
	{
		SetControllerAdditionalParameterImpl( controllers.Get(j), userData);
	}
}

void AnimationInstance::SetControllerAdditionalParameter(const char* name, void* userData)
{
	SetControllerAdditionalParameterImpl( controllers.Get(name), userData);	
}

void AnimationInstance::SetControllerAdditionalParameterImpl(IAnimationController* controller, void* userData)
{
	if (controller) controller->SetAdditionalParameter(userData);// + processor->GetRootOffset());	
}

void AnimationInstance::SetControllerConstraints(const char* name,const Vector& min,const Vector& max)
{
	SetControllerConstraintsImpl(controllers.Get(name), min, max);
}

void AnimationInstance::SetControllerConstraintsImpl(IAnimationController* controller,const Vector& min,const Vector& max)
{	
	if (controller) controller->SetConstraints(min, max);		
}

void AnimationInstance::SetControllerPreRotation(const char* name, Matrix& transform)
{
	SetControllerPreRotationImpl(controllers.Get(name), transform);	
}

void AnimationInstance::SetControllerPreRotationImpl(IAnimationController* controller, Matrix& transform)
{		
	if (controller)
	{						
		controller->SetPreRotation(transform);	
	}
}

void AnimationInstance::SetControllerBendDirection(const char* name,const Vector& dir)
{
	SetControllerBendDirectionImpl(controllers.Get(name), dir);	
}

void AnimationInstance::SetControllerBendDirectionImpl(IAnimationController* controller, const Vector& dir)
{
	if (controller)
	{						
		controller->SetBendDirection(dir);	
	}
}

void AnimationInstance::SetControllersSpeed(float speed)
{
	for (int j=0; j<(int)controllers.Size();j++)
	{
		SetControllerSpeedImpl( controllers.Get(j), speed);
	}
}

void AnimationInstance::SetControllerSpeed(const char* name,float speed)
{
	SetControllerSpeedImpl(controllers.Get(name), speed);
}

void AnimationInstance::SetControllerSpeedImpl(IAnimationController* controller,float speed)
{
	if (controller) controller->SetSpeed(speed);	
}

void AnimationInstance::SetControllerOffset(const char* name, const Vector& offset)
{
	IAnimationController* controller = controllers.Get(name);
	if (controller) controller->SetOffset(offset);	
}

void AnimationInstance::RemoveController(const char* boneName)
{
	// FIX ME!!!
}

bool AnimationInstance::PlayAnimation(const char* command, AnimationRes::ClipDesc* desc, bool topLayer, float inTime, float start_time)
{	
	PlayElement* elem = player.Get(HashValue(command));

	if (!elem)
	{				
		elem = player.Add(command);

		if (processor->InitClip(elem->clip, desc, inTime))
		{			
			elem->desc = desc;

			if (elem->desc->playMode == 0)
			{
				elem->cur_event = 0;				
			}
			else
			{
				elem->cur_event = (int)elem->desc->events.size()-1;;
			}			

			elem->num_plays = desc->numLoops;
			elem->topLayer = topLayer;
		
			if (topLayer) num_active_top_layers++;

			if (start_time>0.0f)
			{
				if (desc->playMode == 1)				
				{
					start_time = 1-start_time;
				}

				elem->clip->SetCurTime(start_time);
			}				

			elem->clip->SetCustomData(elem);

			if (desc->startWeight >= 0.0f && topLayer)
			{
				elem->clip->SetWeight(desc->startWeight, 0.0f);
			}			

			return true;
		}
		else
		{
			player.Delete(player.GetNumPlayed()-1);
		}
	}

	return false;
}

void AnimationInstance::StopPlayer(bool stop_all, float blendTime)
{
	bool blendInGlobal = false;

	if (actived_link)
	{
		blendInGlobal = actived_link->blendInGlobal;
	}

	processor->StartBlend( blendTime, blendInGlobal);	

	num_active_top_layers = 0;

	for (int i=0; i<player.GetNumPlayed();i++)
	{
		PlayElement* elem = player.Get(i);	

		if (elem->clip->NeedDelete()) continue;

		if (elem->clip->IsPaused() || elem->desc->blendMode==0 || stop_all || cur_node->topLayers.Get(HashValue(elem->desc->name.s)))
		{					
			ProcessEvents(elem, 1, null);			
		}
		else		
		if (elem->desc->blendMode==2)
		{			
			AnimationRes::ClipDesc* clip = cur_node->layers.Get(elem->desc->name.s);

			if (!clip)
			{
				ProcessEvents(elem, 1, null);
			}
			else
			{
				// process all events that will not be processed in next node
				float tm = elem->clip->GetCurTime();
				if (elem->desc->playMode == 0)
				{
					for (int j = elem->cur_event; j<(int)elem->desc->events.size(); j++)
					{
						AnimationRes::Event* evnt = &elem->desc->events[j];
						
						if ( !evnt->unskeepable )
							continue;

						int k;
						for ( k = 0; k < (int)clip->events.size(); k++ )
						{
							if ( tm < clip->events[k].time && !strcmp( evnt->id.s, clip->events[k].id.s ) )
								break;
						}

						if ( k == clip->events.size() )
						{
							em_events->ProcessEvent(evnt->id.s, evnt->param.s);
						}
					}
				}
				else
				{
					for (int j = elem->cur_event; j>=0; j--)
					{
						AnimationRes::Event* evnt = &elem->desc->events[j];

						if ( !evnt->unskeepable )
							continue;

						int k;
						for ( k = 0; k < (int)clip->events.size(); k++ )
						{
							if ( tm > clip->events[k].time && !strcmp( evnt->id.s, clip->events[k].id.s ) )
								break;
						}

						if ( k == clip->events.size() )
						{
							em_events->ProcessEvent(evnt->id.s, evnt->param.s);
						}
					}
				}
			}
		}
	}

 	for (int i=0; i<player.GetNumPlayed();i++)
	{
		PlayElement* elem = player.Get(i);		

		if (elem->clip->NeedDelete()) continue;

		if (elem->clip->IsPaused() || elem->desc->blendMode==0 || stop_all || cur_node->topLayers.Get(HashValue(elem->desc->name.s)))
		{								
			elem->clip->Stop(0.0f);
			player.Delete(i);
			i--;			
		}
		else		
		if (elem->desc->blendMode==2)
		{			
			AnimationRes::ClipDesc* clip = cur_node->layers.Get(elem->desc->name.s);

			if (clip)
			{
				PrePlayElement el;
				el.name.Copy(elem->desc->name.s);
				elem->clip->GetPlayData(el.playdata);
				el.desc = clip;
				el.cur_event = elem->cur_event;								

				pre_player.push_back(el);
								
				elem->clip->Stop(0.0f);				
				player.Delete(i);
				i--;			
			}
			/*else
			{
				elem->clip->Stop(0.0f);
				player.Delete(i);
				i--;			
			}*/
		}
	}
}

void AnimationInstance::ProcessEvents(PlayElement* elem, int mode, RecordedFrame* frame)
{
	if (recording && !frame) return;

	if (!em_events) return;

	if (elem->clip->need_delete) return;

	if (elem->clip->GetWeight()<0.1f) return;

	float tm = elem->clip->GetCurTime();

	if (elem->desc->playMode == 0)
	{
		for (int j = elem->cur_event; j<(int)elem->desc->events.size(); j++)
		{
			AnimationRes::Event* evnt = &elem->desc->events[j];

			if (tm>=evnt->time || (mode == 1 && evnt->unskeepable)||(mode == 2))
			{
				if (frame)
				{
					if (frame->num_events<3)
					{
						String::Copy(frame->events[frame->num_events].id.s,63,evnt->id.s);
						String::Copy(frame->events[frame->num_events].param.s,63,evnt->param.s);
						
						frame->num_events++;						
					}
				}
				else
				{
					em_events->ProcessEvent(evnt->id.s, evnt->param.s);
				}
				elem->cur_event++;
			}
		}
	}
	else
	{
		for (int j = elem->cur_event; j>=0; j--)
		{
			AnimationRes::Event* evnt = &elem->desc->events[j];

			if (tm<=evnt->time || (mode == 1 && evnt->unskeepable)||(mode == 2))
			{
				if (frame)
				{
					if (frame->num_events<3)
					{
						String::Copy(frame->events[frame->num_events].id.s,63,evnt->id.s);
						String::Copy(frame->events[frame->num_events].param.s,63,evnt->param.s);
						
						frame->num_events++;						
					}
				}
				else
				{
					em_events->ProcessEvent(evnt->id.s, evnt->param.s);
					elem->cur_event--;
				}
			}
		}
	}
}

void AnimationInstance::Update(float dt)
{	
	if (frames.size() == 0)
	{	
		for (int i=0; i<player.GetNumPlayed(); i++)
		{

			PlayElement* elem = player.Get(i);
		
			if (!processor->UpdateClip(elem->clip, dt))
			{			
				continue;
			}

			// if elem is not in use, then it was deleted after we go to other (may be the same) node
			if ( !elem->inUse )
				break;
		
			if (em_events)
			{		
				if (!elem->clip->IsPaused()) ProcessEvents(elem, 0, null);
			}
	
			if (elem->clip->NeedDelete())
			{
				player.Delete(i);
				i--;
			}
		}

		CheckLinkActivation();
	}
	else
	{
		RecordedFrame* frame = &frames[cur_rec_frame];
		RecordedFrame* next_frame = NULL;
		float blend = 1.0f;
				
		if (cur_rec_frame<frames.size()-1 && cur_rec_frame != 0)
		{
			next_frame = &frames[cur_rec_frame+1];

			if (frame->node != next_frame->node)
			{
				next_frame = NULL;				
			}
			else
			{
				blend = (cur_rec_time - frame->time)/(next_frame->time - frame->time);
			}
		}

		processor->cur_blend_time = -1.0f;

		//Prepare Blender
		if (next_frame)
		{
			if (next_frame->blendFrame != -1)
			{
				RecordedFrame* blend_frame = &frames[next_frame->blendFrame];

				float blenderTime = 1.0f - (cur_rec_time - blend_frame->time) * next_frame->blendSpeed;
			
				if (blenderTime>0.001f)
				{
					for (int i=0;i<blend_frame->num_elems;i++)
					{
						if (processor->InitClip(rec_clip[i], blend_frame->elems[i].desc, 0.0f))
						{
							float time = blend_frame->elems[i].time;
							float weight = blend_frame->elems[i].weight;				

							rec_clip[i]->SetCurTime(time);
							rec_clip[i]->SetWeight(weight, 0.0f);
						}			
					}
	
					processor->Update(0.0f);
					processor->StartBlend(1000.0f, false);
					processor->cur_blend_time = blenderTime;
				}			
			}
		}

		for (int i=0;i<frame->num_elems;i++)
		{
			if (processor->InitClip(rec_clip[i], frame->elems[i].desc, 0.0f))
			{
				float time = frame->elems[i].time;
				float weight = frame->elems[i].weight;

				if (next_frame)
				{
					int index = -1;

					for (int j=0;j<next_frame->num_elems;j++)
					{
						if (frame->elems[i].desc == next_frame->elems[j].desc)
						{
							index = j;
							break;
						}
					}

					if (index != -1)
					{
						if (time < next_frame->elems[index].time)
						{
							time = time + (next_frame->elems[index].time - time) * blend;
						}
						else
						{
							time = time + (1.0f + next_frame->elems[index].time - time) * blend;
							if (time>1.0f) time -= 1.0f;							
						}						

						weight = weight + (next_frame->elems[index].weight - weight) * blend;
					}
				}

				rec_clip[i]->SetCurTime(time);
				rec_clip[i]->SetWeight(weight, 0.0f);
			}			
		}

		cur_update_rec_frame = cur_rec_frame;
	}
	
	update_time -= dt;

	//if (update_time < 0.0f)
	{
		processor->Update(dt);

		for (int j=0; j<(int)affectors.size();j++)
		{
			affectors[j].affector->Update(dt);
		}
	
		for (int j=0; j<(int)controllers.Size();j++)
		{
			controllers.Get(j)->Update(dt);
		}

		update_time = freq;

		bonesReady = true;
	}
	/*else
	{
		bonesReady = false;
	}*/
}

void AnimationInstance::ResetDeltaPos(bool full)
{
	processor->ResetDeltaPos(full);	
}

const char* AnimationInstance::SeletClipNameFromTable(const char* name)
{
	if (active_table)
	{
		AnimationRes::Str64* nm = active_table->clips.Get(name);

		if (nm)
		{
			return nm->s;
		}		
	}

	return NULL;
}

void AnimationInstance::DrawDebug()
{
	#if !defined(ANIM_STANDALONE)
		
	char frame[32];
	
	for (int i = 0; i < 15;i++)
	{						
		sprintf(frame,"%s%i", "Clip", i);
		
		/*Vector2 pos = Vector2(20.f, 20.f + 20.f*i);		

		if (i==0)
		{			
			char str[128];
			sprintf(str, "%s", //%i",// %4.3f %4.3f",
						 CurrentNodeName());
						 //((AnimationProcessorEM*)processor)->actor_instance->GetMotionSystem()->GetNumMotionInstances());
						 //delta_pos.x,delta_pos.y,delta_pos.z);

			//g_GameProxy.DebugFont->onSetFontText(frame, str);
			core->Render()->PrintText( pos.x, pos.y, up_left, COLOR_WHITE, str );
		}
		else
		if (i==1)
		{
			char str[256];
			str[0] = 0;

			for (int j=0;j<controllers.Size();j++)
			{
				IAnimationController* cntrl = controllers.Get(j);
				
				char tmp[128];
				sprintf(tmp, "%s %4.2f c%4.2f;", controllers.GetName(j), cntrl->GetWeight(), cntrl->GetCurWeight());
				strcat (str, tmp);
			}							
			
			core->Render()->PrintText( pos.x, pos.y, up_left, COLOR_WHITE, str );
		}
		else		
		if (i-2<player.GetNumPlayed())
		{
			char tmp[128];
			sprintf(tmp, "%s w - %4.3f t - %.3f e - %d",
					player.GetName(i-2), player.Get(i-2)->clip->GetWeight(), player.Get(i-2)->clip->GetCurrentTime(), 
					player.Get(i-2)->cur_event);
						
			core->Render()->PrintText( pos.x, pos.y, up_left, COLOR_WHITE, tmp );
		}		*/
	}	

	#endif
}

void AnimationInstance::StartRecord()
{
	Restart();
	frames.clear();
	
	cur_rec_time = 0;
	cur_rec_frame = 0;					

	recording = true;

	processor->Update(0.0f);
	ProcessRecord(0.0f);	
}

void AnimationInstance::RecordAction(int cmd, const char* name, float time)
{
	if (cmd == 0)
	{
		ActivateLink(name);
	}

	if (cmd == 1)
	{
		GotoNode(name, time);
	}

	else if (cmd == 2)
	{
		PlayLayer(name);
	}

	else if (cmd == 3)
	{
		StopLayer(name, time);
	}
}

void AnimationInstance::ProcessRecord(float dt)
{
	if (dt>0.00001f)
	{
		cur_rec_time += dt;

		for (int i=0; i<player.GetNumPlayed(); i++)
		{
			PlayElement* elem = player.Get(i);
			
			if (!processor->UpdateClip(elem->clip, dt))
			{			
				continue;
			}		

			if (elem->clip->NeedDelete())
			{
				player.Delete(i);
				i--;
			}
		}

		CheckLinkActivation();

		return;
	}	

	frames.push_back(RecordedFrame());
	RecordedFrame* frame = &frames[frames.size()-1];

	RecordedFrame* prev_frame = NULL;
	if (frames.size() > 1) prev_frame = &frames[frames.size()-2];	

	frame->time = cur_rec_time;
	frame->node = cur_node;
	frame->blendFrame = -1;	
	frame->blendTime = 0.0f;
	frame->blendSpeed = 1.0f;
	
	if (prev_frame)
	{
		if (prev_frame->node != frame->node)
		{
			frame->blendFrame = frames.size()-2;			
			frame->blendSpeed = processor->blend_speed;
			
			if (frame->blendSpeed>0.0f)
			{
				frame->blendTime = 1.0f - (frame->time - prev_frame->time) * frame->blendSpeed;
			}
			else
			{
				frame->blendTime = 0.0f;
			}
		}
		else
		{
			if (prev_frame->blendFrame != -1 && prev_frame->blendTime>0.01f)
			{
				frame->blendFrame = prev_frame->blendFrame;
				frame->blendSpeed = prev_frame->blendSpeed;
				frame->blendTime = prev_frame->blendTime - dt * frame->blendSpeed;						
			}
		}

		if (frame->blendTime<0.0f)
		{
			frame->blendTime = 0.0f;
		}
	}	

	frame->num_elems = player.GetNumPlayed();	
	frame->num_events = 0;

	int index = 0;

	for (int i=0; i<frame->num_elems; i++)
	{
		PlayElement* elem = player.Get(i);

		if (!elem->clip->IsPaused())
		{
			ProcessEvents(elem, 0, frame);
		}

		if (elem->clip->NeedDelete())
		{
			frame->num_elems--;
			continue;
		}
						
		frame->elems[index].desc = elem->desc;
		frame->elems[index].time = elem->clip->GetCurTime();
		frame->elems[index].weight = elem->clip->GetWeight();		
		index++;
	}	
}

void AnimationInstance::FinishRecord()
{
	cur_rec_time = 0.0f;
	recording = false;
}

void AnimationInstance::SetTime(float time)
{
	int last_cur_rec_frame = cur_rec_frame;

	cur_rec_time = time;
	cur_rec_frame = 0;	

	if (cur_rec_frame==frames.size()-1) return;

	while (frames[cur_rec_frame+1].time<time + 0.001f)
	{				
		cur_rec_frame++;
		if (cur_rec_frame==frames.size()-1) break;
	}

	if (!em_events) return;

	if (last_cur_rec_frame == cur_rec_frame) return;

	if (last_cur_rec_frame>cur_rec_frame)
	{
		last_cur_rec_frame = 0;
	}

	for (int i = last_cur_rec_frame;i<=cur_rec_frame;i++)
	{
		for (int j = 0; j<frames[i].num_events;j++)
		{
			em_events->ProcessEvent(frames[i].events[j].id.s, frames[i].events[j].param.s);
		}
	}
}

void AnimationInstance::AddAffector(IAnimationAffector* affector, int level)
{
	int index = (int)affectors.size();

	for (int i=0;i<(int)affectors.size();i++)
	{
		if (affectors[i].affector == affector) return;

		if (level<affectors[i].level)
		{
			index = i;
			break;
		}
	}

	affectors.push_back(AnimAffector());

	for (int i=(int)affectors.size()-1;i>index;i--)
	{
		affectors[i] = affectors[i-1];
	}

	affectors[index].level = level;
	affectors[index].affector = affector;		
	affectors[index].affector->SetOwner(this);
}

void AnimationInstance::DeleteAffector(IAnimationAffector* affector)
{
	int index = -1;

	for (int i=0;i<(int)affectors.size();i++)
	{
		if (affectors[i].affector == affector)
		{
			index = i;
			break;
		}
	}
		
	if (index<0) return;

	for (int i=index; i<(int)affectors.size()-1;i++)
	{
		affectors[i] = affectors[i+1];
	}

	affectors.pop_back();
}

void AnimationInstance::Release()
{	
	em_events = 0;
	affectors.clear();

	StopPlayer(true, 0.0f);

	processor->Release();
	controllers.Clear();
	

	res->manager->ReleaseAnim(this);

	player.Release();

	for (int i=0;i<8;i++)
	{
		delete rec_clip[i];
	}

	delete this;
}

bool AnimationInstance::BonesReady()
{
	return bonesReady;
}

void AnimationInstance::PrepareRenderData(Vector4* data, dword* cmp_bones, int num_complete_bones)
{
	processor->PrepareRenderData(data, cmp_bones, num_complete_bones);
}

AnimationRes* AnimationInstance::GetRes()
{
	return res;
}
