#pragma once

#ifdef EDITOR

#include "Platforms/PC/Editor/wxWidgetsConfig.h"
#include "wx/wx.h"


class StringDataTrackWidgets : public wxPanel
{
public:

	StringDataTrackWidgets( wxWindow* parent = NULL);
	char* value;

	wxStaticText* prop_label;	
	wxTextCtrl*   prop_ed;

	void InitControls();
	void StartEditKey(char* vl, const char* name);

private:
			
	void OnValEdited(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif