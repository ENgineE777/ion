
#ifdef EDITOR

#include "CharacterTrackWidgets.h"
#include "wx/xrc/xmlres.h"
#include "wx/app.h"  
#include "Platforms/PC/Editor/wxWidgetsConfig.h"
#include "Platforms/PC/Editor/EditorMainWnd.h"

#include "CharacterPlayer.h"
#include "DeepBlack/Game.h"
#include "DeepBlack/World.h"

CharacterPlayer* CharacterTrackWidgets::g_CurPlayer = 0;
//-------------------------------------------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CharacterTrackWidgets, wxPanel)
EVT_COMBOBOX(XRCID("RespawnsList"), CharacterTrackWidgets::OnRespawnSet)
EVT_BUTTON(XRCID("btnLoadCharacter"), CharacterTrackWidgets::OnBtnLoad)
EVT_CHECKBOX(XRCID("ProjectionMoveFlag"), CharacterTrackWidgets::OnMovementFlagCheck)
EVT_CHECKBOX(XRCID("OrientBySpline"), CharacterTrackWidgets::OnOrientBySplneFlagCheck)
EVT_CHECKBOX(XRCID("LoadStyleFlag"), CharacterTrackWidgets::OnLoadStyleChange)
END_EVENT_TABLE()

//-------------------------------------------------------------------------------------------------------------------------
CharacterTrackWidgets::CharacterTrackWidgets(wxWindow* parent)
{	
	m_LoadFromFile = false;
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("CharacterProp"));	
}
//-------------------------------------------------------------------------------------------------------------------------
CharacterTrackWidgets::~CharacterTrackWidgets()
{

}
//-------------------------------------------------------------------------------------------------------------------------

void CharacterTrackWidgets::InitControls()
{
	loadButton = (wxButton*)FindWindowByName("btnLoadCharacter",this);
	prop_NameLabel = (wxStaticText*)FindWindowByName("name_Label",this);
	prop_Name =  (wxStaticText*)FindWindowByName("CharacterName",this);
	prop_2DMovementLabel = (wxStaticText*)FindWindowByName("ProjectionMoveFlagLabel",this);
	prop_2DMovement = (wxCheckBox*)FindWindowByName("ProjectionMoveFlag",this);
	prop_OrientByspline = (wxCheckBox*)FindWindowByName("OrientBySpline",this);
	prop_OrinetBySplineLabel = (wxStaticText*)FindWindowByName("OrientBySplineLabel",this);
	

	respawnsList    =  (wxComboBox*)FindWindowByName("RespawnsList",this);
	prop_LoadStyleLabel = (wxStaticText*)FindWindowByName("LoadStyleFlagLabel",this);
	prop_LoadStyle = (wxCheckBox*)FindWindowByName("LoadStyleFlag",this);

	respawnsList->Clear();

	const Respawns_t&	list = CWorld::instance()->getUpdateScene()->getGameManager()->getRespawns();
	for(Respawns_t::const_iterator it = list.begin(); it != list.end();++it)
	{
		respawnsList->Append(it->first.c_str());
	}


	SetSize(wxSize(235,150));
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterTrackWidgets::Show()
{
	__super::Show(true);
	prop_LoadStyleLabel->Show(true);
	prop_LoadStyle->Show(true);
	if(m_LoadFromFile)
		loadButton->Show(true);	
	else 
		respawnsList->Show(true);
	if(g_CurPlayer)
	{
		const char* name = g_CurPlayer->GetCharacterName();
		if(strlen(name) > 0 )
		{
			prop_NameLabel->Show(true);
			prop_Name->Show(true);
			prop_Name->SetLabelText(name);	
			prop_2DMovementLabel->Show(true);
			prop_2DMovement->Show(true);
			prop_OrientByspline->Show(true);
			prop_OrinetBySplineLabel->Show(true);
			prop_2DMovement->SetValue(g_CurPlayer->isMovement2D());
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterTrackWidgets::OnBtnLoad(wxCommandEvent& event)
{
	char name[512];
	name[0] = 0;

	if (OpenDialogSelectFile(name, false, "Level(*.ai)\0*.ai\0"))
	{
		name[strlen(name)-3] = 0;
		if(g_CurPlayer)
			g_CurPlayer->SetCharacter(name,false);
		Show();
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterTrackWidgets::OnMovementFlagCheck(wxCommandEvent& event)
{
	if(g_CurPlayer)
		g_CurPlayer->setMovementFlag(event.IsChecked());
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterTrackWidgets::OnOrientBySplneFlagCheck(wxCommandEvent& event)
{
	if(g_CurPlayer)
		g_CurPlayer->setOrientBySplineFlag(event.IsChecked());
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterTrackWidgets::OnLoadStyleChange(wxCommandEvent& event)
{
	m_LoadFromFile = event.IsChecked();
	if(m_LoadFromFile)
	{
		loadButton->Show(true);	
		respawnsList->Show(false);
	}
	else 
	{
		respawnsList->Show(true);
		loadButton->Show(false);	
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterTrackWidgets::OnRespawnSet(wxCommandEvent& event)
{
	if(g_CurPlayer)
	{
		int index = respawnsList->GetSelection();
		wxString str = respawnsList->GetString(index);
		g_CurPlayer->SetRespawn(str.c_str(),false);
		Show();
	}

}
#endif