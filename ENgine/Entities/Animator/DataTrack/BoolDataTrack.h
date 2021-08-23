#pragma once

#include "DataTrack.h"
#include "Utils/math/IVector4.h"

class BoolDataTrackWidgets;

class BoolDataTrackPoint : public TrackPoint
{
public:			
	bool m_value;
};

class BoolDataTrack : public TemplDataTrack<BoolDataTrackPoint>
{
public:

	bool def_val;

	BoolDataTrack(TrackPlayer* own, const char* nm,
				   KeyFunc key_Func,
				   KeyFunc changeKey_Func, 
					 bool _blend);
	virtual ~BoolDataTrack();

	virtual void Load(StreamLoader* stream, int num);
	virtual void Save(StreamSaver* stream);
	virtual void CalcValue(int index, float blend);

#ifdef EDITOR

	BoolDataTrackWidgets* wigets;	
	
	virtual void SetDefaultKey(int index);
	virtual void InitControls(wxWindow* prop_dock);		
	virtual void StartEditKey(int key, Gizmo* gizmo);	
	virtual void StopEditKey();	
	virtual void OnClose();
	
#endif

};

#define AddBoolDataTrack(name, onCalcCallback, onNextCallback, defValue, blend) \
	BoolDataTrack* name##BoolDataTrack = new BoolDataTrack(this,#name,onCalcCallback,onNextCallback, blend);\
name##BoolDataTrack->def_val = defValue;\
tracks.push_back(name##BoolDataTrack);

