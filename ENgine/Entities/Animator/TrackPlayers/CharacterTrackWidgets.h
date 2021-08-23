#pragma once

#ifdef EDITOR

#include "Platforms/PC/Editor/wxWidgetsConfig.h"
#include "wx/wx.h"

class CharacterPlayer;
class CharacterTrackWidgets : public wxPanel
{
public:
	static CharacterPlayer* g_CurPlayer;

	CharacterTrackWidgets( wxWindow* parent = NULL);
	~CharacterTrackWidgets();

	wxStaticText* prop_NameLabel;
	wxStaticText* prop_2DMovementLabel;
	wxStaticText* prop_OrinetBySplineLabel;
	wxStaticText* prop_Name;
	wxCheckBox*   prop_2DMovement;
	wxCheckBox*   prop_OrientByspline;
	wxStaticText* prop_LoadStyleLabel;
	wxCheckBox*   prop_LoadStyle;
	wxButton*     loadButton;
	wxComboBox*   respawnsList;

	bool		  m_LoadFromFile;

	void InitControls();
	void Show();	
	
	
private:
	
	void OnBtnLoad(wxCommandEvent& event);
	void OnMovementFlagCheck(wxCommandEvent& event);
	void OnOrientBySplneFlagCheck(wxCommandEvent& event);
	void OnLoadStyleChange(wxCommandEvent& event);
	void OnRespawnSet(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif