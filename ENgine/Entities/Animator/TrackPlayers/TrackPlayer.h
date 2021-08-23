#pragma once

//#include "../DataTrack/BoolDataTrack.h"
#include "../DataTrack/FloatDataTrack.h"
//#include "../DataTrack/StringDataTrack.h"
#include "../DataTrack/TransformDataTrack.h"
#include "../DataTrack/Transform2DDataTrack.h"
//#include "../DataTrack/AICommands/AICommandTrack.h"
//#include "../DataTrack/AnimDataTrack.h"
//#include "../DataTrack/Vector4DataTrack.h"

#include "Core/Scene/SceneEntity.h"

class TrackPlayer : public PropertyHolder
{
	char  name[64];
	char  type[64];	

public:	

	// HAck for ProprtyHolder
	class tp_desc
	{
	public:
		const char* GetName()
		{
			return "TrackPlayer";
		};
	};

	SceneEntity* owner;

	tp_desc* Desc() 
	{
		static tp_desc desc;
		return &desc;
	};

	void DataToGizmo() {};

	typedef void (TrackPlayer::*KeyFunc)(TrackPoint* key);
	typedef void (TrackPlayer::*ChangeKeyFunc)(TrackPoint* key);

	TrackPlayer();

	virtual ~TrackPlayer();

	virtual void SetOwner(SceneEntity* own);

	virtual void Clear();

	virtual const char* GetName()
	{ 
		return name;
	};

	virtual void SetName(const char* nm)
	{ 
		String::Copy(name, 64, nm);
	};

	virtual const char* GetType()
	{ 
		return type;
	};

	virtual void SetType(const char* tp)
	{ 
		String::Copy(type, 64, tp);
	};

#ifdef EDITOR
	virtual void InitControls(BaseWidget* parent);
	virtual void PrepareControls(TimeLineWidget* timeline, int line);	
	virtual void StartEditProp();
	virtual bool UpdateWidgets();
	virtual void StopEditProp();
	virtual void DataFromGizmo();
	virtual void DebugDraw();
	virtual void OnClose();
	virtual void OnStopEdit();
	virtual void CheckHotKeys();
#endif

	bool IsActive() {return active;}
	virtual void SetActive(bool value);

	virtual void Load(IStreamLoader* stream);
	virtual void LoadProperties(IStreamLoader* stream);	
	
	virtual void Save(IStreamSaver* stream);
	virtual void SaveProperties(IStreamSaver* stream);
	
	virtual void Reset();
	virtual void SetTime(float dt);	

	virtual void ReInit();
	virtual void OnPropertiesChanged();

	virtual void PostLoadInit();
	virtual void Release();

	virtual void StartPlay(){};
	virtual void EndPlay(){};

	DataTrack* GetDataTrack( const char* _name );
	
	std::vector<DataTrack*> tracks;	

protected:
	bool  active;
};

ClassDeclarantBegin(TrackPlayerDecl,TrackPlayer)
ClassDeclarantEnd(TrackPlayerDecl,TrackPlayer)

#define TRACKPLAYER(className, name) \
DESC_BEGIN(TrackPlayerDecl,className, name, 0) \
DESC_END_WO_DESC(TrackPlayerDecl,className, name, 0)