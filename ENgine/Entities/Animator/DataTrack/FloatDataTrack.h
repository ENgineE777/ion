#pragma once

#include "DataTrack.h"

class FloatDataTrackWidgets;

class TrackFloatPoint : public TrackPoint
{
public:			
	float val;
};

class FloatDataTrack : public TemplDataTrack<TrackFloatPoint>
{
public:

	float def_val;
	float* target;

	FloatDataTrack(TrackPlayer* own, const char* nm,
				   KeyFunc key_Func,
				   KeyFunc changeKey_Func);
	virtual ~FloatDataTrack();

	virtual void Load(IStreamLoader* stream, int num);
	virtual void Save(IStreamSaver* stream);
	virtual void CalcValue(int index, float blend);

	virtual void SetControlledValue(void* value);

#ifdef EDITOR

	FloatDataTrackWidgets* wigets;	
	
	virtual void SetDefaultKey(int index);
	virtual void InitControls(BaseWidget* parent);		
	virtual void StartEditKey(int key, Gizmo* gizmo);	
	virtual bool UpdateWidgets();
	virtual void StopEditKey();	
	virtual void OnClose();
	
#endif

};

#define AddFloatDataTrack(name, onCalcCallback, onNextCallback, defValue) \
	FloatDataTrack* name##FloatDataTrack = new FloatDataTrack(this,#name,onCalcCallback,onNextCallback);\
name##FloatDataTrack->def_val = defValue;\
tracks.push_back(name##FloatDataTrack);

