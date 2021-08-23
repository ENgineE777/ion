
#ifdef EDITOR

#include "ColorPanelWidget.h"
#include "wx/xrc/xmlres.h"
#include "Platforms/PC/Editor/EditorMain.h"
#include "Vector4DataTrackWidgets.h"

BEGIN_EVENT_TABLE(ColorPanelWidget, wxPanel)
	EVT_LEFT_DCLICK (ColorPanelWidget::OnColorPanelDclick)
END_EVENT_TABLE()

ColorPanelWidget::ColorPanelWidget(wxWindow* parent)
{	
	bool succes = wxXmlResource::Get()->LoadPanel(this, parent, wxT("ColorPanelWidget"));	
}

void ColorPanelWidget::OnColorPanelDclick(wxMouseEvent &event)
{
	Vector4DataTrackWidgets* parent = static_cast<Vector4DataTrackWidgets*>(GetParent());
	parent->OnColorPanelDclick();
}


#endif