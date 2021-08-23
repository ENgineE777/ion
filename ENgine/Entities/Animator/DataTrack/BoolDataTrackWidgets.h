#pragma once

#ifdef EDITOR

#include "Platforms\PC\Editor/wxWidgetsConfig.h"
#include "wx/wx.h"
#include "Utils/math/IVector4.h"
#include "ColorPanelWidget.h"


class BoolDataTrackWidgets : public wxPanel
{
public:

	BoolDataTrackWidgets(class BoolDataTrack* _track, wxWindow* parent = NULL);
	class BoolDataTrack* m_track;

	wxCheckBox* m_CheckBtn;	

	void InitControls();
	void StartEditKey( const char* name );

private:
			
	void OnValEdited(wxCommandEvent& event);


	DECLARE_EVENT_TABLE()
};

#endif