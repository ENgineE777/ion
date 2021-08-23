#pragma once

#include "DataTrack.h"
#include "Utils/math/Vector4.h"

class Vector4DataTrackWidgets;

class Vector4DataTrackPoint : public TrackPoint
{
public:			
	IVector4 m_value;
};

class Vector4DataTrack : public TemplDataTrack<Vector4DataTrackPoint>
{
public:

	IVector4 def_val;

	Vector4DataTrack(TrackPlayer* own, const char* nm,
				   KeyFunc key_Func,
				   KeyFunc changeKey_Func);
	virtual ~Vector4DataTrack();

	virtual void Load(StreamLoader* stream, int num);
	virtual void Save(StreamSaver* stream);
	virtual void CalcValue(int index, float blend);

#ifdef EDITOR

	Vector4DataTrackWidgets* wigets;	
	
	virtual void SetDefaultKey(int index);
	virtual void InitControls(wxWindow* prop_dock);		
	virtual void StartEditKey(int key, Gizmo* gizmo);	
	virtual void StopEditKey();	
	virtual void OnClose();
	
#endif

};

#define AddVector4DataTrack(name, onCalcCallback, onNextCallback, defValue) \
	Vector4DataTrack* name##Vector4DataTrack = new Vector4DataTrack(this,#name,onCalcCallback,onNextCallback);\
name##Vector4DataTrack->def_val = defValue;\
tracks.push_back(name##Vector4DataTrack);


