
#ifdef EDITOR

#include "AnimModelPlayerWidgets.h"
#include "wx/xrc/xmlres.h"

#include "DeepBlack/Cutscene/DataTrack/TransformDataTrack.h"
#include "windows.h"
#include "AnimModelPlayer.h"
#include "Core/Core.h"


BEGIN_EVENT_TABLE(AnimModelPlayerWidgets, wxPanel)
	EVT_BUTTON(XRCID("btnOpenModel"), AnimModelPlayerWidgets::OnBtnOpenModel)	
	EVT_BUTTON(XRCID("btnOpenGraf"), AnimModelPlayerWidgets::OnBtnOpenGraf)	
END_EVENT_TABLE()

AnimModelPlayerWidgets::AnimModelPlayerWidgets(wxWindow* parent)
{	
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("AnimModelPlayerProp"));	


	m_Player = NULL;
	this->SetPosition(wxPoint(0.f, 100.f));
	//track = NULL;	
}

void AnimModelPlayerWidgets::InitControls()
{
	//prop_label = (wxStaticText*)FindWindowByName("trackTransformName");
	SetSize(wxSize(350,GetSize().GetY()));

	m_ModelNameTxt =(wxStaticText*)FindWindowByName("ModelNameTxt", this);
	m_AnimGrafTxt =(wxStaticText*)FindWindowByName("AnimGrafTxt", this);

	//edX = (wxTextCtrl*)FindWindowByName("edX");
	//edY = (wxTextCtrl*)FindWindowByName("edY");
	//edZ = (wxTextCtrl*)FindWindowByName("edZ");
}

void AnimModelPlayerWidgets::StartEditKey(TransformDataTrack* tr, const char* name)
{
	//track = tr;	

	//prop_label->SetLabelText(name);	



	//TransformTrackPoint* point = track->GetPoint();

	//if (point)
	//{
	//	char str[32];

	//	sprintf(str, "%4.1f", point->pos.x);
	//	edX->SetValue(str);

	//	sprintf(str, "%4.1f", point->pos.y);
	//	edY->SetValue(str);

	//	sprintf(str, "%4.1f", point->pos.z);
	//	edZ->SetValue(str);		
	//}

	Show();
}



void AnimModelPlayerWidgets::UpdateVal(wxTextCtrl* ctrl, float* value)
{
	/*float val = *value;
	val = atof(ctrl->GetValue().c_str());

	if (fabs(val - *value)>0.001f)
	{
		*value = val;
	}
	else
	{
		char str[32];
		sprintf(str, "%4.1f", *value);
		ctrl->SetValue(str);
	}

	track->Prepare();
	track->DataToGizmo();*/
}



bool OpenDialogSelectFile(char* file_name, bool full_path, const char* filter);

void AnimModelPlayerWidgets::OnBtnOpenModel(wxCommandEvent& event)
{
	char name[512];
	name[0] = 0;

	if (OpenDialogSelectFile(name, false, "*.mesh"))
	{
		name[strlen(name)-5] = 0;
	}

	m_Player->SetModelName(name);

	//m_ModelNameTxt->SetLabelText(wxString(name));
}

void AnimModelPlayerWidgets::OnBtnOpenGraf(wxCommandEvent& event)
{
	char name[512];
	name[0] = 0;

	if (OpenDialogSelectFile(name, false, "*.lua"))
	{
		name[strlen(name)-4] = 0;
	}

	m_Player->SetAnimGrafName(name);

	//m_AnimGrafTxt->SetLabelText(wxString(name));
}




#endif