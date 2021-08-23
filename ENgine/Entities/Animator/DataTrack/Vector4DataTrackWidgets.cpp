
#ifdef EDITOR

#include "Vector4DataTrackWidgets.h"
#include "wx/xrc/xmlres.h"
#include "Platforms/PC/Editor/EditorMain.h"

BEGIN_EVENT_TABLE(Vector4DataTrackWidgets, wxPanel)
	EVT_TEXT_ENTER(XRCID("edit"), Vector4DataTrackWidgets::OnValEdited)	
	//EVT_BUTTON(XRCID("ColorBtn"), Vector4DataTrackWidgets::OnColorBtn)	
END_EVENT_TABLE()

Vector4DataTrackWidgets::Vector4DataTrackWidgets(wxWindow* parent)
{	
	bool succes = wxXmlResource::Get()->LoadPanel(this, parent, wxT("Vector4DataTrackWidget"));	
	m_colorPanel = std::auto_ptr<ColorPanelWidget>(new ColorPanelWidget(this));
	m_colorPanel->SetPosition(wxPoint(20, 25));
	value = NULL;
}

void Vector4DataTrackWidgets::InitControls()
{
	prop_label = (wxStaticText*)FindWindowByName("trackName",this);
	prop_ed = (wxTextCtrl*)FindWindowByName("edit",this);	
	prop_ed->SetWindowStyleFlag(wxTE_PROCESS_ENTER);
	m_colorPanel->SetBackgroundColour(wxColor());
	SetSize(wxSize(250,150));
}

void Vector4DataTrackWidgets::StartEditKey(IVector4* vl, const char* name)
{
	value = vl;
	prop_label->SetLabelText(name);

	char str[32];
	sprintf(str, "%.2f %.2f %.2f %.2f", value->x, value->y, value->z, value->w );
	prop_ed->SetValue(str);

	m_colorPanel->SetBackgroundColour(wxColor(value->x*255, value->y*255, value->z*255, value->w*255 ));
	Show();
	Refresh();
}

void Vector4DataTrackWidgets::OnValEdited(wxCommandEvent& WXUNUSED(event))
{			
	IVector4 temp;
	if (sscanf_s(prop_ed->GetValue().ToAscii(), "%f %f %f %f", &temp.x, &temp.y, &temp.z, &temp.w) == 4)
		*value = temp;
	else
	{
		char str[32];
		sprintf(str, "%.2f %.2f %.2f %.2f",  value->x, value->y, value->z, value->w );
		prop_ed->SetValue(str);
	}

	m_colorPanel->SetBackgroundColour(wxColor(value->x*255, value->y*255, value->z*255, value->w*255 ));
	Refresh();
}

//void Vector4DataTrackWidgets::OnColorBtn(wxCommandEvent& event)
//{
//	dword color;
//	ColorDialog(color);
//	
//	char str[32];
//	sprintf(str, "%.2f %.2f %.2f %.2f",  *(((byte*)&color)+2)/255.f, *(((byte*)&color)+1)/255.f, *((byte*)&color)/255.f, *(((byte*)&color)+3)/255.f );
//	prop_ed->SetValue(str);
//	OnValEdited(wxCommandEvent());
//
//}

void Vector4DataTrackWidgets::OnColorPanelDclick()
{
	dword color;
	ColorDialog(color);

	char str[32];
	sprintf(str, "%.2f %.2f %.2f %.2f",  *(((byte*)&color)+2)/255.f, *(((byte*)&color)+1)/255.f, *((byte*)&color)/255.f, *(((byte*)&color)+3)/255.f );
	prop_ed->SetValue(str);
	OnValEdited(wxCommandEvent());
}


#endif