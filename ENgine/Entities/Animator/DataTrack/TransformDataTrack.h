#pragma once

#include "Common/Math.h"
#include "DataTrack.h"

class TransformDataTrackWidgets;

class TransformTrackPoint : public TrackPoint
{
public:			
	Quaternion rot;
	Vector dir;
	Vector a, b, c, d;
	Vector dirAxeX;
	Vector aX, bX, cX, dX;
	Quaternion aQ, bQ, cQ, dQ;

	Vector pos;
	Vector b_pt1;
	Vector b_pt2;
	
	float length;
	float dst[20];

	float prev_speed;		
	float speed;
};	

class TransformDataTrack : public TemplDataTrack<TransformTrackPoint>
{		
	int curve_count;
	Vector* curve;
	float radius;
	bool m_grabCameraOnAddKey;
	Vector m_defaultPos;
	Matrix* target;

public:	

	bool  orient_by_spline;

	TransformDataTrack(TrackPlayer* own, const char* nm,float radius,bool orinet,
					   KeyFunc key_Func, ChangeKeyFunc changeKey_Func, bool _grabCameraOnAddKey = true,  Vector _defaultPos = Vector());
	virtual ~TransformDataTrack();

	virtual void Load(IStreamLoader* stream, int num);
	virtual void Save(IStreamSaver* stream);

	void BezierFunc(Vector& p1, Vector& p2,Vector& p3,Vector& p4, Vector& pos, float t);
	void CubicFunc(int index, Vector& pos, float s);
	void CubicFuncAxeX(int index, Vector& pos, float s);
	void CubicFuncQuat(int index, Quaternion& quat, float s);
	void GenerateKoef(int start_index, int end_index);
	void GenerateKoefAxeX(int start_index, int end_index);
	void GenerateKoefQuat(int start_index, int end_index);
	void Prepare();
	virtual void CalcValue(int index, float blend);

	virtual void SetControlledValue(void* value);

#ifdef EDITOR

	TransformDataTrackWidgets* wigets;
	bool invertSightInGizmo;

	virtual void SetDefaultKey(int index);
	virtual void DelKey(int index);
	virtual int  AddKey(float time);

	virtual void InitControls(BaseWidget* parent);	
	virtual void StartEditKey(int key, Gizmo* gizmo);	
	virtual void StartEditMultiKeys(std::vector<int>& _keys, Gizmo* gz);
	virtual bool UpdateWidgets();
	virtual void StopEditMultiKeys();
	virtual void SwicthKeyBlend();	
	virtual void SetKeyBlend(bool _value);
	virtual void DataToGizmo();
	void DataToGizmoMultiSelect();
	virtual void DataFromGizmo();
	virtual void StopEditKey();	
	virtual void DebugDraw();
	virtual void PasteFromBuffer();
	virtual void OnClose();
	virtual void CheckHotKeys();
	TransformTrackPoint* GetPoint();
	void UpdateValue(int type, float value);
	void GrabCamera(int index);

#endif

};

#define AddTransformDataTrack(name,r,orient,onCalcCallback, onNextCallback) \
TransformDataTrack* name##TransformDataTrack = new TransformDataTrack(this,#name,r,orient,onCalcCallback,onNextCallback);\
tracks.push_back(name##TransformDataTrack);

#define AddTransformDataTrackWithDefaultKey(name,r,orient,onCalcCallback, onNextCallback, defaultPos) \
	TransformDataTrack* name##TransformDataTrack = new TransformDataTrack(this,#name,r,orient,onCalcCallback,onNextCallback, false, defaultPos);\
	tracks.push_back(name##TransformDataTrack);