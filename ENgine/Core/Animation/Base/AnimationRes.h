#pragma once

#include "Core/Animation/IAnimationService.h"

class AnimationService;
class AnimationInstance;

class AnimationRes
{	
public:
	
	struct Str64
	{
		char s[64];

		Str64()
		{
			s[0] = 0;
		}

		void Copy(const char* src)
		{
			String::Copy(s, 64, src);
		}		
	};

	struct Str1024
	{
		char s[1024];

		Str1024()
		{
			s[0] = 0;
		}

		void Copy(const char* src)
		{
			String::Copy(s, 1024, src);
		}		
	};


	struct Param
	{
		Str64 name;
		int type;
		float flt_val;
		Str64 str_val;
	};

	struct AffectorParam
	{
		Str64 name;
		HashMap<Param> params;

		AffectorParam() : params(_FL_)
		{
		};
		
		~AffectorParam()
		{
			params.Clear();
		}
	};
	
	HashMap<AffectorParam> affectors;

	struct Controller
	{	
		IAnimationController::ControllerType type;
		Str64	    id;
		Str64		boneName;
		Str64		boneOffsetName;
		Vector		min;
		Vector		max;		
		Matrix     postRotMat;
		Vector     bendDir;
		float       speed;
		bool        active;

		Controller()
		{					
			min.Set(0.0f);
			max.Set(0.0f);
			postRotMat.Identity();
			bendDir = Vector(1.0f,0.0f,0.0f);
			speed = 1.0f;
			active = true;
			type = IAnimationController::lookat;
		}
	};

	struct Event
	{
		Str64 id;
		Str64 param;
		float time;
		bool  unskeepable;

		Event()
		{			
			time = 0.0f;
			unskeepable = false;
		}
	};	

	struct NamesTable
	{
		Str64 name;
		HashMap<Str64> clips;	

		NamesTable() : clips(_FL_)
		{
		};

		~NamesTable()
		{
			clips.Clear();
		};
	};		

	struct ClipName
	{
		struct clip_desc
		{
			Str1024 name;
			bool isVirtual;
		};

		std::vector<clip_desc> clips;

		void Init(const char* str, const char* virt)
		{
			int last_index = 0;
			int len = (int)strlen(str);

			for (int i=0;i<len;i++)
			{
				if (str[i] == '%' || i == len - 1)
				{
					clip_desc out;	
					out.isVirtual = false;

					int border = i;
					int index = 0;
					if (i == len - 1) border++;

					for (int j=last_index;j<border;j++)
					{						
						out.name.s[index] = str[j];
						index++;
					}

					out.name.s[index] = 0;

					clips.push_back(out);
					last_index = i+1;
				}				
			}	

			for (int i=0;i<(int)strlen(virt);i++)
			{
				if (virt[i] == 'v')
				{
					clips[i].isVirtual = true;
				}
			}

		};

		const char* Select()
		{
			if (clips.size()==0)
			{
				return "";

			}

			int index = 0;

			if (clips.size()>1)
			{											
				index = (int)(rnd() * clips.size() * 0.99999f);
			}

			return clips[index].name.s;
		};

		int GetNum()
		{
			return (int)clips.size();
		};

		bool IsVirtual(int index)
		{
			return clips[index].isVirtual;
		}

		const char* GetName(int index)
		{
			return clips[index].name.s;
		};
	};

	struct ClipDesc
	{
		Str64       name;	
		ClipName	clip_name;
		float		playSpeed;
		float		timeIn;
		float		timeOut;		
		bool		mix;		
		int			numLoops;
		int			playMode;
		int			blendMode;
		float		startWeight;
		bool		additiveBlend;
		std::vector<Str64> channels;
		std::vector<Event> events;

		ClipDesc()
		{			
			playSpeed = 1.0f;			
			mix = false;			
			numLoops = 10000;
			timeIn = 0.3f;
			timeOut = 0.3f;			
			playMode = 0;
			blendMode = 0;
			startWeight = -1.0f;
			additiveBlend = false;
		};
	};

	struct ControllerParam
	{
		int         active;
		bool	    act_set;
		Str64		name;
		bool        constr_set;
		Vector		min;		
		Vector		max;				
		Matrix     postRotMat;
		bool        rot_set;
		Vector     bendDir;
		bool        bend_set;
		float       speed;
		bool        speed_set;		

		ControllerParam()
		{
			active = -1;
			min.Set(0.0f);
			max.Set(0.0f);
			postRotMat.Identity();
			speed = -1.0f;
			constr_set = false;			
			rot_set = false;
			act_set = false;
			speed_set = false;
			bendDir = 0.0f;
			bend_set = false;			
		};
	};

	struct NodeConstant
	{
		Str64 value;		
	};

	struct AnimLink
	{
		Str64 destination;
		float blendTime;
		float activeTime;
	    int   syncTiming;
		bool  blendInGlobal;

		AnimLink()
		{			
			blendTime = 0.2f;			
			activeTime = -1.0f;
			syncTiming = 0;
			blendInGlobal = false;
		};
	};	

	struct AnimNode
	{					
		Str64					 name;
		int						 topLayersPlayMode;
		HashMap<Str64>			 topLayers;
		HashMap<ControllerParam> controllerParams;
		HashMap<NodeConstant>    constants;
		HashMap<ClipDesc>		 layers;
		AnimLink				 defLink;
		HashMap<AnimLink>		 links;
		int						 getMovement;

		AnimNode() : topLayers(_FL_), controllerParams(_FL_),
					 constants(_FL_), layers(_FL_), links(_FL_)
		{
			topLayersPlayMode = 0;
			getMovement = 1;
		};
		
		~AnimNode()
		{
			topLayers.Clear();
			controllerParams.Clear();
			constants.Clear();
			layers.Clear();
			links.Clear();
		};
	};	
	
	AnimationService* manager;

	Str64					   graph_name;
	Str64					   startNode;	
	Str64					   movementBone;	
	HashMap<Controller>		   controllers;	
	HashMap<AnimNode>		   nodes;
	Str64					   startTable;
	HashMap<NamesTable>		   namesTables;

	int     ref_counter;

	AnimationRes();
	virtual ~AnimationRes() {};

	void AddRef();
	bool DecRef();

	virtual AnimationInstance* CreateReference();	
	virtual bool Load(const char* name);

	//virtual void ReadNode(CLuaTable& table);

	virtual bool PreLoad(const char* name) = 0;
	virtual void LoadClip(const char* name, ClipDesc* desc) = 0;	
	virtual bool ClipsLoaded() = 0;
	virtual void Release();
};
