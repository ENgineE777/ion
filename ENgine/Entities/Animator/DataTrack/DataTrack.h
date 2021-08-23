#pragma once

#include "core/Files/StreamLoader.h"
#include "core/Files/StreamSaver.h"

#ifdef EDITOR

#include "Platforms\PC\GUI\Widgets.h"
#include "Editor\Helpers\Gizmo.h"
/*
#include "Platforms\PC\Editor\wxWidgetsConfig.h"
#include "wx/wx.h"
#include "Platforms\PC\Editor\WidgetTimeLine.h"
#include "Platforms\PC\Editor\WidgetCaptions.h"
#include "Utils\Gizmo.h"*/

#endif

class TrackPlayer;
class TrackPoint;
typedef void (TrackPlayer::*KeyFunc)(TrackPoint* key);
typedef void (TrackPlayer::*ChangeKeyFunc)(TrackPoint* key);

class TrackPoint
{
public: 
	bool  needBlend;
	float time;
#ifdef EDITOR
	virtual void OnClose(){}
#endif
};

class DataTrack
{
public:

	virtual ~DataTrack() {};

	virtual const char* GetName() = 0;
	virtual void Load(IStreamLoader* stream, int num) = 0;	
	virtual void Save(IStreamSaver* stream) = 0;
	virtual void Reset() = 0;	
	virtual void CalcValue(int index, float blend) = 0;
	virtual void SetTime(float time) = 0;
	virtual void SetControlledValue(void* value) = 0;
	virtual int  GetKeysCount() = 0;

#ifdef EDITOR
	virtual int  AddKey(float time) = 0;
	virtual void SetDefaultKey(int index) = 0;
	virtual void DelKey(int index) = 0;
	virtual void InitControls(BaseWidget* parent) = 0;
	virtual void PrepareControls(TimeLineWidget* timeline, int line) = 0;	

	virtual void StartEditKey(int key, Gizmo* gizmo) = 0;
	virtual bool UpdateWidgets() = 0;
	virtual void StopEditKey() = 0;
	virtual void StartEditMultiKeys(std::vector<int>& _keys, Gizmo* _gz) = 0;
	virtual void StopEditMultiKeys() = 0;

	virtual void SetEditedKeyTime(float time) = 0;
	virtual void SetKeyTime(int _key, float time) = 0;
	virtual TrackPoint* GetEditedPoint() = 0;
	virtual void SwicthKeyBlend() = 0;
	virtual void SetKeyBlend(bool _value) = 0;
	virtual void DataFromGizmo() = 0;
	
	virtual void DebugDraw() = 0;
	virtual void OnClose() = 0;
	virtual void CopyToBuffer() = 0;
	virtual void PasteFromBuffer() = 0;
	virtual void CheckHotKeys() = 0;
	virtual void SetParent(DataTrack* _parent) = 0;
	virtual DataTrack* GetParent() = 0;
	virtual void AddChild(DataTrack* _child) = 0;
	virtual const std::vector<DataTrack*>& GetChilds() = 0;
	virtual const TrackPoint& GetPoint(int _index) = 0;

	virtual void SetEditorPlayBack(bool _value) = 0;
	virtual void SetEditorPlay(bool _value) = 0;

#endif
};

template<typename DataType>
class TemplDataTrack : public DataTrack
{
protected:
		
	int   cur_index;		
				
	TrackPlayer* owner;

	static DataType ms_BufferValue;
	DataType cur_val;
	std::vector<DataType> points;
	float m_prevTime;
	bool m_needBlendInitValue;

	char name[64];
	KeyFunc keyFunc;
	ChangeKeyFunc changeKeyFunc;

public:

#ifdef EDITOR	
	int edited_key;	
	std::vector<int> m_EditedKeys;
	Gizmo* gizmo;
	bool m_editorPlayBack;
	bool m_editorPlay;
	DataTrack* m_parent;
	std::vector<DataTrack*> m_childs;
#endif

	TemplDataTrack(TrackPlayer* own, const char* nm,
				   KeyFunc key_Func,
				   KeyFunc changeKey_Func,
					 bool _blend = true)
	{
		owner = own;
		strcpy(name, nm);
		keyFunc = key_Func;
		changeKeyFunc = changeKey_Func;
		cur_index = 0;
		m_prevTime = -0.1f;
		m_needBlendInitValue = _blend;

#ifdef EDITOR	
		m_editorPlayBack = false;
		m_parent = NULL;
		edited_key = -1;
		gizmo = null;
#endif
	};

	virtual ~TemplDataTrack()
	{
		points.clear();
	};

	int GetCurIndex() { return cur_index;}
	const std::vector<DataType>& GetPoints() { return points;}
	int AddPoint(const DataType& _point);
	int AddSortedPoint(const DataType& _point);
	void ClearPoints() { points.clear();}
	const DataType& GetCurPoint() {return points[cur_index];}
	const DataType& GetCurValue() {return cur_val;}

	virtual const char* GetName()
	{ 
		return name;
	};
	
	virtual void Reset()
	{ 
		cur_index = 0;
	};
	
	virtual void CalcValue(int index, float blend) = 0;

	virtual void SetTime(float time)
	{	
#ifdef EDITOR
		if (m_parent)
			return;


		dword size = m_childs.size();
		for (dword i = 0; i < size; ++i)
		{
			DataTrack* temp = m_childs[i]->GetParent();
			m_childs[i]->SetParent(NULL);
			m_childs[i]->SetTime(time);
			m_childs[i]->SetParent(temp);
		}
#endif

		cur_val.time = time;
		if ((int)points.size() == 0) return;

		if ((int)points.size() == 1 || points[0].time >= time)
		{
			CalcValue(0, -1.0f);
			if (m_prevTime == 0.f)
			{
				if ( m_prevTime <= points[0].time &&  points[0].time < time && m_prevTime != time) 
				{		
					if (changeKeyFunc)
					{					
						(owner->*changeKeyFunc)(&points[0]);
					}				
				}

			}
			else if ( m_prevTime < points[0].time &&  points[0].time <= time && m_prevTime != time) 
			{		
				if (changeKeyFunc)
				{					
					(owner->*changeKeyFunc)(&points[0]);
				}				
			}	

#ifdef EDITOR
			else if ( m_prevTime > points[0].time &&  points[0].time <= time && m_prevTime > time && !m_editorPlayBack && m_editorPlay ) 
			{		
				if (changeKeyFunc)
				{					
					(owner->*changeKeyFunc)(&points[0]);
				}				
			}
#endif

		}
		else
		{
			if (points[(int)points.size()-1].time<=time)
			{
				if (cur_index<=(int)points.size() - 1)
				{
					while (cur_index<=(int)points.size() - 1)
					{
						if (changeKeyFunc)
						{					
							(owner->*changeKeyFunc)(&points[cur_index]);
						}				

						cur_index++;
					}
				}

				CalcValue((int)points.size() - 1, -1.0f);
			}
			else
			{			
				while (points[cur_index].time <= time)
				{		
					if (changeKeyFunc)
					{					
						(owner->*changeKeyFunc)(&points[cur_index]);
					}				

					cur_index++;
				}					

				if (points[cur_index - 1].needBlend)
				{
					CalcValue(cur_index,
							  (time - points[cur_index - 1].time) /
							  (points[cur_index].time - points[cur_index - 1].time));
				}
				else
				{
					CalcValue(cur_index - 1, -1.0f);
				}
			}
		}

		if (keyFunc) (owner->*keyFunc)(&cur_val);		

		m_prevTime = time;
	}

	virtual void SetControlledValue(void* value)
	{

	}

	virtual int GetKeysCount()
	{
		return (int)points.size();
	}

#ifdef EDITOR

	virtual int AddKey(float time)
	{
		for (int i=0;i<(int)points.size();i++)
		{
			if (fabs(points[i].time - time)<0.01f)
			{
				return -1;
				break;				
			}
		}
		
		points.push_back(DataType());
		
		int index = (int)points.size()-1;
		points[index].time = time;
		points[index].needBlend = m_needBlendInitValue;
		SetDefaultKey(index);
		
		for (int i=(int)points.size()-1; i>0; i--)
		{
			if (points[i].time<points[i-1].time)
			{
				DataType tmp = points[i];
				if(i == edited_key)
					edited_key = i - 1;
				points[i] = points[i-1];
				points[i-1] = tmp;
				index--;

				continue;
			}
		
			break;		
		}	
		
		dword size = m_childs.size();
		for (dword i = 0; i < size; ++i)
		{
			m_childs[i]->AddKey(time);
		}

		return index;
	}	

	virtual void DelKey(int index)
	{
		if(index == edited_key)
			edited_key = -1;
		if (!points[index].needBlend)
		{
			if (index>0)
			{
				points[index-1].needBlend = false;
			}
		}

		points.erase(points.begin() + index);	

		dword size = m_childs.size();
		for (dword i = 0; i < size; ++i)
		{
			m_childs[i]->DelKey(index);
		}

		cur_index = 0;

	}

	virtual void InitControls(BaseWidget* parent)
	{
		edited_key = -1;		
	}

	virtual void PrepareControls(TimeLineWidget* timeline, int line)
	{
		gizmo = null;		

		//timeline->AddLine(this);
		//dock->AddLine(this, name);

		for (int i=0;i<(int)points.size();i++)
		{
			timeline->AddKey( line, points[i].time, points[i].needBlend);
		}
	}

	virtual void StartEditKey(int key, Gizmo* gz)
	{		
		edited_key = key;
		gizmo = gz;
	}

	virtual void StartEditMultiKeys(std::vector<int>& _keys, Gizmo* gz)
	{
		gizmo = gz;
		m_EditedKeys = _keys;
	}

	virtual void StopEditMultiKeys()
	{
		m_EditedKeys.clear();
	}


	virtual DataType* GetEditedPoint()
	{
		if (edited_key != -1)
		{
			return &points[edited_key];
		}

		return NULL;
	}

	virtual void SetEditedKeyTime(float time)
	{
		if (edited_key!=-1) points[edited_key].time = time;

		dword size = m_childs.size();
		for (dword i = 0; i < size; ++i)
		{
			m_childs[i]->SetKeyTime(edited_key, time);
		}
	}

	virtual void SetKeyTime(int _key, float time)
	{
		if (_key != -1) points[_key].time = time;

		dword size = m_childs.size();
		for (dword i = 0; i < size; ++i)
		{
			m_childs[i]->SetKeyTime(edited_key, time);
		}
	}

	virtual void SwicthKeyBlend()
	{
		if (edited_key!=-1) points[edited_key].needBlend = !points[edited_key].needBlend;
	}

	virtual void SetKeyBlend(bool _value)
	{
		if (edited_key!=-1) points[edited_key].needBlend = _value;
	}

	virtual void DataFromGizmo()
	{

	}

	virtual void StopEditKey()
	{
		edited_key = -1;
		gizmo = null;
	}	

	virtual void DebugDraw()
	{
	}
	virtual void OnClose()
	{
		for (int i=0;i<(int)points.size();i++)
		{
			 points[i].OnClose();
		}
	}

	virtual void CopyToBuffer()
	{
		if(edited_key != -1)
		{
			ms_BufferValue = points[edited_key];
		}
		else
			ms_BufferValue = cur_val;
	}

	virtual void PasteFromBuffer()
	{
		if (edited_key < 0 || edited_key > points.size() - 1) return;

		float temp = points[edited_key].time;

		points[edited_key] = ms_BufferValue;
		points[edited_key].time = temp;
	}

	virtual void CheckHotKeys()
	{

	}

	virtual void SetParent(DataTrack* _parent)
	{
		m_parent = _parent;
	}

	virtual DataTrack* GetParent() { return m_parent;}

	virtual void AddChild(DataTrack* _child)
	{
		m_childs.push_back(_child);
	}

	virtual const std::vector<DataTrack*>& GetChilds()
	{
		return m_childs;
	}

	virtual const TrackPoint& GetPoint(int _index)
	{
		return points[_index];
	}

	void CallChangeKeyFunc( DataType& _value)
	{
		if (changeKeyFunc)
		{					
			(owner->*changeKeyFunc)(&_value);
		}
	}

	void SetEditorPlayBack(bool _value)
	{
		m_editorPlayBack  = _value;
	}

	void SetEditorPlay(bool _value)
	{
		m_editorPlay  = _value;
	}
				

	

#endif

};

template<typename DataType>	DataType TemplDataTrack<DataType>::ms_BufferValue;

template<typename DataType> int TemplDataTrack<DataType>::AddPoint(const DataType& _point)
{
	int index = AddKey(_point.time);
	points[index] = _point;
	return index;
}

template<typename DataType> int TemplDataTrack<DataType>::AddSortedPoint(const DataType& _point)
{
	points.push_back(_point);
	return points.size() - 1;
}
