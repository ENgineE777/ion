
#ifdef EDITOR

#include "BoolDataTrackWidgets.h"
#include "wx/xrc/xmlres.h"
#include "Platforms/PC/Editor/EditorMain.h"
#include "BoolDataTrack.h"

BEGIN_EVENT_TABLE(BoolDataTrackWidgets, wxPanel)
	EVT_CHECKBOX(XRCID("CheckBtn"), BoolDataTrackWidgets::OnValEdited)	
END_EVENT_TABLE()

BoolDataTrackWidgets::BoolDataTrackWidgets(BoolDataTrack* _track, wxWindow* parent)
{	
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("BoolDataTrackWidget"));	
	m_track = _track;
}

void BoolDataTrackWidgets::InitControls()
{
	m_CheckBtn = (wxCheckBox*)FindWindowByName("CheckBtn",this);
	assert(m_CheckBtn);
	SetSize(wxSize(250,150));
}

void BoolDataTrackWidgets::StartEditKey( const char* name )
{
	static_cast<wxStaticText*>(FindWindowByName("trackName",this))->SetLabelText(name);
	m_CheckBtn->SetValue(m_track->GetEditedPoint()->m_value);
	Show();
	Refresh();
}

void BoolDataTrackWidgets::OnValEdited(wxCommandEvent& WXUNUSED(event))
{			
	m_track->GetEditedPoint()->m_value = m_CheckBtn->GetValue();
	m_track->CallChangeKeyFunc(*m_track->GetEditedPoint());
	Refresh();
}




#endif