#pragma once

#ifdef EDITOR

#include "Platforms/PC/Editor/wxWidgetsConfig.h"
#include "wx/wx.h"
#include "Utils/math/IVector4.h"
#include "ColorPanelWidget.h"

#include <memory>

class Vector4DataTrackWidgets : public wxPanel
{
public:

	Vector4DataTrackWidgets( wxWindow* parent = NULL);
	IVector4* value;

	wxStaticText* prop_label;	
	wxTextCtrl*   prop_ed;

	void InitControls();
	void StartEditKey(IVector4* vl, const char* name);
	void OnColorPanelDclick();

	std::auto_ptr<ColorPanelWidget> m_colorPanel;

private:
			
	void OnValEdited(wxCommandEvent& event);
	void OnColorBtn(wxCommandEvent& event);


	DECLARE_EVENT_TABLE()
};

#endif