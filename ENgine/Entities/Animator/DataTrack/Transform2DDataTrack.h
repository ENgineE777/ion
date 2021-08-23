#pragma once

#include "Common/Math.h"
#include "DataTrack.h"

class Transform2DDataTrackWidgets;

class Transform2DTrackPoint : public TrackPoint
{
public:				
	Vector2 pos;
	Vector2 size;	
};	

class Transform2DDataTrack : public TemplDataTrack<Transform2DTrackPoint>
{			
public:		

	Transform2DDataTrack(TrackPlayer* own, const char* nm, KeyFunc key_Func, ChangeKeyFunc changeKey_Func, Vector _defaultPos = Vector());
	virtual ~Transform2DDataTrack();

	virtual void Load(IStreamLoader* stream, int num);
	virtual void Save(IStreamSaver* stream);
		
	virtual void CalcValue(int index, float blend);

	virtual void SetControlledValue(void* value);

#ifdef EDITOR

	Transform2DDataTrackWidgets* wigets;	

	virtual void SetDefaultKey(int index);	

	virtual void InitControls(BaseWidget* parent);	
	virtual void StartEditKey(int key, Gizmo* gizmo);		
	virtual bool UpdateWidgets();
	virtual void StopEditMultiKeys();	
	virtual void DataToGizmo();
	void DataToGizmoMultiSelect();
	virtual void DataFromGizmo();
	virtual void StopEditKey();			
	virtual void OnClose();
	virtual void CheckHotKeys();
	Transform2DTrackPoint* GetPoint();
	void UpdateValue(int type, float value);	

#endif

};

#define AddTransformDataTrack(name,r,orient,onCalcCallback, onNextCallback) \
TransformDataTrack* name##TransformDataTrack = new TransformDataTrack(this,#name,r,orient,onCalcCallback,onNextCallback);\
tracks.push_back(name##TransformDataTrack);

#define AddTransformDataTrackWithDefaultKey(name,r,orient,onCalcCallback, onNextCallback, defaultPos) \
	TransformDataTrack* name##TransformDataTrack = new TransformDataTrack(this,#name,r,orient,onCalcCallback,onNextCallback, false, defaultPos);\
	tracks.push_back(name##TransformDataTrack);