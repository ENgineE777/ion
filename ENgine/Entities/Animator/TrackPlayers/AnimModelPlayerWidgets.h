#pragma once

#ifdef EDITOR

#include "Platforms/PC/Editor/wxWidgetsConfig.h"
#include "wx/wx.h"
#include "Utils\IMath.h"

class TransformDataTrack;

class AnimModelPlayerWidgets : public wxPanel
{
public:

	AnimModelPlayerWidgets( wxWindow* parent = NULL);

	//TransformDataTrack* track;

	//wxStaticText* prop_label;	
	//wxTextCtrl*   prop_ed;
	//wxTextCtrl*   edX;
	//wxTextCtrl*   edY;
	//wxTextCtrl*   edZ;


	void InitControls();
	void StartEditKey(TransformDataTrack* tr, const char* name);

	class AnimModelPlayer* m_Player;
	class wxStaticText* m_ModelNameTxt;
	class wxStaticText* m_AnimGrafTxt;

private:

	void UpdateVal(wxTextCtrl* ctrl, float* val);

	void OnBtnOpenModel(wxCommandEvent& event);
	void OnBtnOpenGraf(wxCommandEvent& event);


	DECLARE_EVENT_TABLE()
};

#endif