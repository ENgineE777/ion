#pragma once

#ifdef EDITOR

#include "Platforms/PC/Editor/wxWidgetsConfig.h"
#include "wx/wx.h"
#include "Utils/math/IVector4.h"


class ColorPanelWidget : public wxPanel
{
public:

	ColorPanelWidget( wxWindow* parent = NULL);
	void OnColorPanelDclick(wxMouseEvent &event);
	DECLARE_EVENT_TABLE()
};

#endif