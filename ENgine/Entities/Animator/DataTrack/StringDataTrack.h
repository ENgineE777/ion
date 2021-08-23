#pragma once

#include "DataTrack.h"
class StringDataTrackWidgets;

class TrackStringPoint : public TrackPoint
{
public:			
	char val[64];
};

class StringDataTrack : public TemplDataTrack<TrackStringPoint>
{	
public:
	
	StringDataTrack(TrackPlayer* own, const char* nm,
					KeyFunc key_Func,
					ChangeKeyFunc changeKey_Func, bool _blend = true);	
	virtual ~StringDataTrack();

	virtual void Load(StreamLoader* stream, int num);
	virtual void Save(StreamSaver* stream);
	virtual void CalcValue(int index, float blend);

#ifdef EDITOR

	StringDataTrackWidgets* wigets;	

	virtual void SetDefaultKey(int index);
	virtual void InitControls(wxWindow* prop_dock);	
	virtual void StartEditKey(int key, Gizmo* gz);	
	virtual void StopEditKey();	
	virtual void OnClose();

#endif

};

#define AddStringDataTrack(name, onCalcCallback, onNextCallback) \
StringDataTrack* name##StringDataTrack = new StringDataTrack(this,#name,onCalcCallback,onNextCallback);\
tracks.push_back(name##StringDataTrack);

#define AddStringDataTrackNoBlend(name, onCalcCallback, onNextCallback) \
	StringDataTrack* name##StringDataTrack = new StringDataTrack(this,#name,onCalcCallback,onNextCallback, false);\
	tracks.push_back(name##StringDataTrack);