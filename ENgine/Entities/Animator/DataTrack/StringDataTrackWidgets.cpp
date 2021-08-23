

#ifdef EDITOR

#include "StringDataTrackWidgets.h"
#include "wx/xrc/xmlres.h"

BEGIN_EVENT_TABLE(StringDataTrackWidgets, wxPanel)
	EVT_TEXT_ENTER(XRCID("editString"), StringDataTrackWidgets::OnValEdited)	
END_EVENT_TABLE()

StringDataTrackWidgets::StringDataTrackWidgets(wxWindow* parent)
{	
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("trackString"));	
	value = NULL;
}

void StringDataTrackWidgets::InitControls()
{
	prop_label = (wxStaticText*)FindWindowByName("trackStringName",this);
	prop_ed = (wxTextCtrl*)FindWindowByName("editString", this);	
	prop_ed->SetWindowStyleFlag(wxTE_PROCESS_ENTER);
	SetSize(wxSize(300,150));
}

void StringDataTrackWidgets::StartEditKey(char* vl, const char* name)
{
	value = vl;
	prop_label->SetLabelText(name);
	
	prop_ed->SetValue(vl);

	Show();
}

void StringDataTrackWidgets::OnValEdited(wxCommandEvent& WXUNUSED(event))
{	
	strcpy(value,prop_ed->GetValue().c_str());	
}

#endif