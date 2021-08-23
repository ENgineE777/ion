#pragma once

#include "BaseWidget.h"
#include "TextBoxWidget.h"
#include "ScrollBarWidget.h"

class Gizmo;

class DEF_EXPORT TimeLineWidget : public BaseWidget
{
public:

	Gizmo* gizmo;

	struct Key
	{
		float time;
		bool needBlend;
	};

	struct Line
	{
		bool isSeparator;
		char name[64];
		std::vector<Key> keys;
		void* ptr;		
	};

	float timeLenght;
	float scale;
	std::vector<Line> lines;

	float* curTime;

	float* fromTime;
	float* toTime;

	int drag_mode;
	int selLine;
	int selKey;	

	bool play;

	std::vector<int> m_SelLines;
	std::vector<int> m_SelKeys;	

	TextBoxWidget* edCurTime;	
	TextBoxWidget* edFromTime;
	TextBoxWidget* edToTime;

	ScrollBarWidget* horz_scroll;
	ScrollBarWidget* vert_scroll;

	float CalcKeyPos(float ps_x);
	float CalcLinePos(int index);
	float PixelToTime(float px);

	bool  IsTimeVisible(float time);
	void  SetTimeVisible(float time);
	void  SetTime(float time);

	void Clear();
	void SetTimeLenght(float time);
	void SetCurTime(float time);
	void SetFromTime(float time);
	void SetToTime(float time);
	void AddLine(const char* name, void* ptr, bool separator = false);
	void InsertLine(int index, const char* name, void* ptr, bool separator = false);
	void DelLine(int index);
	void* GetSelLineOwner();
	void AddKey();
	void AddKey(int line, float time, bool needBlend);	
	void DelKey();		
	void DelKey(int _line, int _key);
	void SwitchKeyCut();

	int  FindLineIndex(void* ptr);

	void Play();
	void Update();

	void SetWidgets(float* cr_time, TextBoxWidget* tbCurTime,
					float* from_time, TextBoxWidget* tbFromTime,
				    float* to_time, TextBoxWidget* tbToTime,
					ScrollBarWidget* horz_scroll, ScrollBarWidget* vert_scroll);

	///TrackPlayer* GetSelTrackPlayer();
	//class DataTrack* GetSelDataTrack();

	void UpdateHorizBar();
	void UpdateVertBar();

	void SetSelection(int line, int key);
	void AddToMultiSelection(int new_line, int new_key);
	void ChangeAcitivity();

	void CheckHotKeys();

	TimeLineWidget(int id, BaseWidget* parent, float x, float y, float w, float h);
	virtual void ProcessWidget(WPARAM wParam, LPARAM lParam);
	virtual void Draw();

	virtual void OnLeftMouseDown(int mx, int my);
	virtual void OnMouseMove(int mx, int my);
	virtual void OnLeftMouseUp(int mx, int my);
};
