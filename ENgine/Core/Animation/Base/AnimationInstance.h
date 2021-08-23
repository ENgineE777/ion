#pragma once

#include "Core/Animation/IAnimationInstance.h"
#include "Core/Animation/Base/AnimationRes.h"
#include "Core/Animation/Base/AnimationProcessor.h"
#include "Core/Animation/Base/AnimationClip.h"

class DEF_EXPORT AnimationInstance : public IAnimationInstance
{						
	HashMap<IAnimationController> controllers;

	struct PrePlayElement
	{
		AnimationRes::Str64 name;		
		AnimationRes::ClipDesc* desc;

		int   cur_event;
		IAnimationClip::PlayData playdata;		
	};			

	std::vector<PrePlayElement> pre_player;
	AnimationRes::AnimLink* actived_link;	
	AnimationRes::NamesTable* active_table;

	bool paused;

	float update_time;
	float freq;
	bool  bonesReady;

public:

	struct AnimAffector
	{
		int level;
		IAnimationAffector* affector;

		AnimAffector()
		{
			level = 0;
			affector = NULL;
		}
	};

	std::vector<AnimAffector> affectors;

	AnimationRes* res;
	AnimationProcessor* processor;

	struct PlayElement
	{
		bool  inUse;
		unsigned int hash;
		char name[64];
		AnimationInstance* owner;
		int num_plays;
		bool topLayer;
		int  cur_event;		
		AnimationRes::ClipDesc* desc;
		AnimationClip* clip;
	};		

	class ClipPlayer
	{
		int cur_elem;
		int played_elem;
		PlayElement elems[32];
		int indices[32];

		public:

		void Init(AnimationInstance* anim, IAnimationProcessor* processor)
		{
			cur_elem = 0;
			played_elem = 0;

			for (int i=0;i<32;i++)
			{
				elems[i].owner = anim;
				elems[i].inUse = false;
				elems[i].clip = (AnimationClip*)processor->CreateClip();				
			}
		};

		void Prepare()
		{
			for (int i=0;i<32;i++)
			{				
				elems[i].clip->Prepare(elems[i].owner);
			}
		};

		int GetNumPlayed()
		{
			return played_elem;
		};

		PlayElement* Add(const char* id)
		{
			if (played_elem >= 32) return NULL;

			while (elems[cur_elem].inUse)
			{
				cur_elem++;
				
				if (cur_elem>=32)
				{
					cur_elem = 0;
				}
			}

			PlayElement* elem = &elems[cur_elem];
			elem->inUse = true;
			elem->hash = HashValue(id);

			String::Copy(elem->name, 64, id);

			indices[played_elem] = cur_elem;
			played_elem++;

			cur_elem++;

			if (cur_elem>=32)
			{
				cur_elem = 0;
			}

			return elem;
		}

		PlayElement* Get(int index)
		{
			return &elems[indices[index]];
		}

		PlayElement* Get(unsigned int hash)
		{
			for (int i=0;i<played_elem;i++)
			{
				if (elems[indices[i]].hash == hash)
				{
					if (!elems[indices[i]].clip->need_delete)
					{
						return &elems[indices[i]];
					}
				}				
			}

			return NULL;
		}		

		const char* GetName(int index)
		{
			return elems[indices[index]].name;
		};

		void Delete(int index)
		{
			if (played_elem==0) return;

			elems[indices[index]].inUse = false;			

			for (int i=index; i<played_elem-1;i++)
			{
				indices[i] = indices[i+1];
			}

			played_elem--;
		}

		void Delete(PlayElement* elem)
		{
			if (played_elem==0) return;

			for (int i=0;i<played_elem;i++)
			{
				if (&elems[indices[i]] == elem)
				{
					Delete(i);
					return;
				}
			}			
		}

		void Release()
		{
			for (int i=0;i<32;i++)
			{
				delete elems[i].clip;
			}
		}
	};

	struct RecordedElement
	{
		AnimationRes::ClipDesc* desc;		
		float time;
		float weight;
	};

	struct RecordedFrame
	{
		float time;
		int num_elems;
		int blendFrame;
		float blendTime;
		float blendSpeed;
		int num_events;
		AnimationRes::AnimNode* node;
		RecordedElement elems[8];	
		AnimationRes::Event events[4];
	};

	bool recording;
	AnimationClip* rec_clip[8];
	float cur_rec_time;
	int cur_rec_frame;
	int cur_update_rec_frame;
	std::vector<RecordedFrame> frames;

	int num_active_top_layers;
	int cur_played_top_player;
	
	IAnimationEvents* em_events;	
	AnimationRes::AnimNode*	  cur_node;

	mutable Matrix res_mat;

	ClipPlayer player;

	AnimationInstance(AnimationRes* rs);
	virtual ~AnimationInstance();

	virtual void Init();

	void CheckLinkActivation();

	virtual void ActivateLink(const char* name, bool ignoreActivated = false);
	virtual void ActivateLink(unsigned int hash, bool ignoreActivated = false);

	virtual bool TestActivateLink(const char* name);
	virtual bool TestActivateLink(unsigned int hash);

	virtual const char* CurrentNodeName();
	virtual const char* GetNodeConstant(const char* name, const char* nodeName = NULL);
	
	virtual void Pause(bool enable);
	virtual bool IsPaused();

	virtual void Restart();

	virtual bool ActivateTableNames(const char* name);

	virtual void ResetNode(float blendTime = 0.2f);

	virtual void GotoNode(const char* name, float blendTime = 0.2f);
	virtual void GotoNode(unsigned int hash, float blendTime = 0.2f);	

	virtual bool PlayLayer(const char* name);
	virtual bool PlayLayer(unsigned int hash);

	virtual bool IsLayerPlaying(const char* name);
	virtual bool IsLayerPlaying(unsigned int hash);

	virtual void StopLayer(const char* name, float time = -1);
	virtual void StopLayer(unsigned int hash, float time = -1);

	virtual bool SetLayerWeight(const char* name, float weight, float time = -1.0f);
	virtual bool SetLayerWeight(unsigned int hash, float weight, float time = -1.0f);

	virtual float GetLayerWeight(const char* name);
	virtual float GetLayerWeight(unsigned int hash);

	virtual	void SetLayerPlaySpeed(const char* name, float speed);
	virtual void SetLayerPlaySpeed(unsigned int hash, float speed);

	virtual	void SetLayerPosition(const char* name, float pos);
	virtual void SetLayerPosition(unsigned int hash, float pos);

	virtual void GetMovement(Matrix& mat);
	virtual bool IsNodeHasMovement();
	
	virtual void BonesMaping(Mesh::Bone* bones, int numBones);

	void GotoNode(AnimationRes::AnimNode* node, float blendTime);
	bool DecNumPlays(PlayElement* elem);
	void StopLayer(PlayElement* elem);
			
	void    CalcBones();
	virtual int  GetNumBones();
	virtual int  GetBoneIndex(const char* name);
	virtual int  GetBoneParentIndex(int index);
	virtual Matrix* GetBoneMatrix(int index);
	virtual Matrix* GetBoneLocalMatrix(int index);
	virtual Matrix* GetBoneMatrixMulGlTrans(int index);	
	virtual Matrix* GetBoneDelta(int index);

	virtual float* GetBBox();
	void SetBBox(float* box);	

	virtual void SetEvents(IAnimationEvents* events);

	virtual void SetTransform(Matrix& transform);	
	virtual void GetTransform(Matrix& transform);	

	virtual void AddController(IAnimationController::ControllerType type,
							   const char* name, const char* boneName,const char* boneOffsetName,
							   Vector& min, Vector& max, float speed);

	virtual const char* GetControllerName(int index);
	virtual int  GetControllersCount();

	virtual void SetMovementBone(const char* bone);

	virtual AnimationRes* GetRes();

	#ifdef ANIM_STANDALONE
	virtual void  GetControllerEDMatrix(const char* id, Matrix& mat);
	#endif

	virtual void SetControllerWeight(int index, float weight, float time);
	virtual void SetControllerWeight(const char* name, float weight, float time);
	virtual void SetControllersWeight(float weight,float time);
	void SetControllerWeightImpl(IAnimationController* controller, float weight, float time);

	virtual float GetControllerWeight(int index);
	virtual float GetControllerWeight(const char* name);
	float GetControllerWeightImp(IAnimationController* controller);

	virtual void SetControllerGoal(int index, const Vector& goal);
	virtual void SetControllerGoal(const char* name, const Vector& goal);
	virtual void SetControllersGoal(const Vector& goal);
	void SetControllerGoalImpl(IAnimationController* controller, const Vector& goal);
	
	virtual void SetControllerAdditionalParameter(int index, void* userData);
	virtual void SetControllerAdditionalParameter(const char* name, void* userData);
	virtual void SetControllersAdditionalParameter(void* userData);
	void SetControllerAdditionalParameterImpl(IAnimationController* controller, void* userData);

	virtual void SetControllerConstraints(const char* name, const Vector& min, const Vector& max);
	void SetControllerConstraintsImpl(IAnimationController* controller, const Vector& min, const Vector& max);	

	virtual void SetControllerPreRotation(const char* name, Matrix& transform);
	void SetControllerPreRotationImpl(IAnimationController* controller, Matrix& transform);
	
	virtual void SetControllerBendDirection(const char* name, const Vector& dir);
	void SetControllerBendDirectionImpl(IAnimationController* controller, const Vector& dir);	

	virtual void SetControllerSpeed(const char* name,float speed);
	virtual void SetControllersSpeed(float speed);
	void SetControllerSpeedImpl(IAnimationController* controller,float speed);

	virtual void SetControllerOffset(const char* name, const Vector& offset);

	virtual void RemoveController(const char* boneName);
	
	bool PlayAnimation(unsigned int hash, bool topLayer, float blendTime, float syncTime);
	bool PlayAnimation(const char* command, AnimationRes::ClipDesc* desc, bool topLayer, float inTime = -1.0f, float start_time = -1.0f);	
	void StopPlayer(bool stop_all, float blendTime);

	virtual void AddAffector(IAnimationAffector* affector, int level);
	virtual void DeleteAffector(IAnimationAffector* affector);

	void ProcessEvents(PlayElement* elem, int mode, RecordedFrame* frame);

	void Update(float dt);

	void ResetDeltaPos(bool full = false);

	const char* SeletClipNameFromTable(const char* name);

	virtual void DrawDebug();

	// Chache
	virtual void StartRecord();
	virtual void RecordAction(int cmd, const char* name, float time);	
	virtual void ProcessRecord(float dt);
	virtual void FinishRecord();	
	virtual void SetTime(float time);

	virtual void Release();

	virtual bool BonesReady();

	virtual void PrepareRenderData(Vector4* data, dword* cmp_bones, int num_complete_bones);
};
