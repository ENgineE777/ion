#ifdef EDITOR

#include "Platforms/PC/GUI/DockHost/DockHostImpl.h"

#include "Editor.h"
#include "Platforms/pc/EditorViewPort.h"
#include "Common/Utils.h"
#include "Entities/NGUI/NGuiWidget.h"
#include "Entities/NGUI/NGUISymbolInstance.h"
#include "Entities/NGUI/NGUIService.h"
#include "Core/Core.h"
#include "Core/Network/NetworkService.h"

#include "Common/libxml/tinyxml2.h"
#include "Core/Network/NetLogger.h"

#include "Entities/Camera.h"

#include "Platforms/pc/gui/UIBlue.h"
#include "Helpers/DeclarationsDescriptor.h"

CPaintManagerUI paintManagerUI;


#define BEGIN_MENU(Name)                {Editor::MenuDescriptor::eBeginSubMenu,Name,-1,0,NULL},
#define ADD_SEPARATOR                   {Editor::MenuDescriptor::eAddSeparator,"",-1,0,NULL},
#define ADD_MENU_ITEM(Name,Id)          {Editor::MenuDescriptor::eAddItem,Name,Id,0,NULL},
#define ADD_CHECKED_MENU_ITEM(Name,Id)  {Editor::MenuDescriptor::eAddItem,Name,Id,MF_CHECKED,NULL},
#define RUN_CREATOR(Name,Func)          {Editor::MenuDescriptor::eRunCreator,Name,-1,0,Func},
#define END_MENU(Name)                  {Editor::MenuDescriptor::eEndSubMenu,Name,-1,0,NULL},

Editor::MenuDescriptor g_vMenuDeclarator[] = 
{
    BEGIN_MENU("Project")
    ADD_MENU_ITEM("New project...",400)
    ADD_MENU_ITEM("Open project...",401)
    ADD_MENU_ITEM("Open script...",SCRIPT_DOCUMENT_OPEN_ID)

    BEGIN_MENU("Save")
    ADD_MENU_ITEM("Save project...",402)
    ADD_MENU_ITEM("Save project as...",403)
    ADD_SEPARATOR
    ADD_MENU_ITEM("Save scene...",404)
    ADD_MENU_ITEM("Save scene as...",405)
    ADD_MENU_ITEM("Save scene as and duplicate...",406)
    END_MENU("Save")

	ADD_MENU_ITEM("Save all", 402)

    ADD_SEPARATOR

    BEGIN_MENU("Add")
    ADD_MENU_ITEM("Add new scene...",408)
    ADD_MENU_ITEM("Add existing scene...",409)
    ADD_SEPARATOR
    ADD_MENU_ITEM("Add new external script...",413)
    ADD_MENU_ITEM("Add external existing script...",414)    
    END_MENU("Add")

    ADD_SEPARATOR

    ADD_MENU_ITEM("Make scene as start...",410)
    ADD_MENU_ITEM("Delete scene...",411)   

    ADD_SEPARATOR
    RUN_CREATOR("Recent Projects",&Editor::CreateRecentOpenProjects)
    RUN_CREATOR("Recent Files",&Editor::CreateRecentOpenFiles)
    ADD_SEPARATOR

    ADD_MENU_ITEM("Project Settings",805)

    ADD_SEPARATOR

    ADD_MENU_ITEM("Exit",412)

    END_MENU("Project")

    BEGIN_MENU("Edit")

    ADD_MENU_ITEM("Undo",185)	
    ADD_MENU_ITEM("Redo",186)	

    ADD_SEPARATOR

    RUN_CREATOR("Create Entity",&Editor::CreateEntitiesMenu)

    ADD_MENU_ITEM("Copy Entity",187)
    ADD_MENU_ITEM("Delete Entity",188)

	ADD_SEPARATOR

	RUN_CREATOR("Script Editor Color Schemes", &Editor::ScriptEditorColorSchemesMenu)

    END_MENU("Edit")

	BEGIN_MENU("Test")

	ADD_MENU_ITEM("Run",236)
	ADD_MENU_ITEM("Select device",SIMULATION_OPTIONS_ID)

	END_MENU("Test")

    BEGIN_MENU("View")
     ADD_MENU_ITEM("Projects",MENU_SHOW_PROJECTS_ID)
     ADD_MENU_ITEM("Scene",MENU_SHOW_SCENE_ID)
     ADD_MENU_ITEM("Object",MENU_SHOW_OBJECT_ID)
     ADD_MENU_ITEM("Output`",MENU_SHOW_LOGGER_ID)
    END_MENU("View")

    BEGIN_MENU("Export")

    ADD_MENU_ITEM("Generate Apk",222)
    ADD_MENU_ITEM("Generate IPA",226)

    END_MENU("Export")

	ADD_MENU_ITEM("Help",228)
};


Editor* editor_core;

TabSheetWidget* Editor::g_pLogTabs = null;
HashTable<string,TextListWidget> Editor::g_vLoggerWidgets(128);

HINSTANCE gEditorInstance;

Editor core_ed;

void EditorFontEntityEnumerator(ComboBoxWidget* widget)
{
	SceneInstance::EntityGroup* group = core_ed.GetCurrentScene()->GetGroup("NGUIFont");
	
	if (group)
	{
		for (size_t i=0; i<group->entities.size(); i++)
			widget->AddString(group->entities[i]->GetName());
	}
}

void EditorGUITexrtureEnumerator(ComboBoxWidget* widget)
{
	SceneInstance::EntityGroup* group = core_ed.GetCurrentScene()->GetGroup("NGUITexture");
	
	if (group)
	{
		for (size_t i=0; i<group->entities.size(); i++)
			widget->AddString(group->entities[i]->GetName());
	}
}

void Editor::CreateRecentPathsMenu(const TRecentPaths& vPaths,int nBaseId,WindowMenuWidget* pMenu,HMENU* pHandle)
{
    int i = 0;
    for(TRecentPaths::const_iterator it = vPaths.begin() ;it !=  vPaths.end();++it,++i)
    {
         if(pHandle)
             pMenu->AddMenuItem(*pHandle,nBaseId + i,(*it).c_str());
          else
             pMenu->AddMenuItem(nBaseId + i,(*it).c_str());
    }
}

void Editor::TraceCallback(const char* log, const char* text)
{	
	if (main_thread_id != GetCurrentThreadId())
	{
		return;
	}

	if (Editor::g_pLogTabs)
	{
		char buff[128];
		String::ExtractFileName(log,buff);

        if (String::IsEqual(buff,"Status"))
        {
            core_ed.SetStatusText("%s",text);
            return;
        }

        TextListWidget::TextType  eType = TextListWidget::eLogInfo;

        if (String::IsEqual(buff,"ScriptErr") || String::IsEqual(buff,"Script"))
		{
            eType = TextListWidget::eLogError;
		}

        TextListWidget*  pWidget = Editor::g_vLoggerWidgets.Find(buff);

		if (!pWidget)
		{
            g_vLoggerWidgets[buff] = TextListWidget(g_pLogTabs,buff,LOGGER_LIST_ID);
            pWidget = g_vLoggerWidgets.Find(buff);
            g_pLogTabs->AddTabWidget(g_pLogTabs->AddTab(buff),pWidget->Widget());
		}

		pWidget->AddString(text, eType);

		char common_text[1024];
		String::Printf(common_text, 1024, "%s: %s", buff, text);
        pWidget = Editor::g_vLoggerWidgets.Find("Output");
		pWidget->AddString(common_text,eType);		
	}						
}

void Editor::ConverterProgress(const char* file_name, int current, int total)
{
	int pr = (int)((float)current/(float)total * 100.0f);
	editor_core->SetProgressExportText("Progress (%i / %i) ..... %i%s", current, total, pr, "%");
}

void Editor::ProgressExportCallback::SetText(const char *text)
{
	pOwner->SetProgressExportText(text);
}

int Editor::main_thread_id = 0;

Editor::Editor():nSelectedScriptEditorColorScheme(1),last_ed_line(0),last_ed_col(0),in_loading_scene(false),in_canvas_drag(false)
    ,prev_mx(0),prev_my(0),pMainWindow(NULL),pGameWindow(NULL),pExportProgressWindow(NULL),pExportProgressLabel(NULL),pProjectList(NULL),pSceneList(NULL),pGuiList(NULL),pScriptTabs(NULL),pEntityName(NULL)
    ,pEntityClassName(NULL),pEntityProperties(NULL),pEntityPropertiesScroll(NULL),pStatusEditLine(NULL),pStatusEditCol(NULL),pStatus(NULL),pScriptEditor(NULL),pAssetsName(NULL),pActiveButton(NULL),pButtonGrid(NULL)
    ,pVisbleButton(NULL),pViewport(NULL),pSelectedObject(NULL),pGameViewport(NULL),system_on_pause(false),pSceneIncludeWindow(NULL),pDevicesWindow(NULL),pDevicesList(NULL)
    ,pDevicesConnectButton(NULL),nDevicesWindowRefreshTimer(0),pDevicesSendButton(NULL),pDevicesSendMessage(NULL),pEditorDockHost(NULL),pViewPortDockHost(NULL),pProjectTabs(NULL),pWorkSpaceTabs(NULL)
	,pFindResultsTabs(NULL)
{	
    editor_core = this;

	pProject = &editor_project.project;	
	
	progress_callback.pOwner = this;

	editor_project.progress_callback = &progress_callback;

	main_thread_id = GetCurrentThreadId();
}

Editor::~Editor()
{
    if(pSimulationOptions)
        delete pSimulationOptions;
    DELETE(pExportProgressWindow)
    DELETE(pSceneIncludeWindow)
    DELETE(pDevicesWindow)
    DELETE(pMainWindow)
    DELETE(pGameWindow)
    DELETE(pProjectWindow)
}

void Editor::OnDockedWindowDestroy(const string& sName)
{
    DockedWidget::DockedWidgetHolder* pHolder = vDockedWidgets.Find(sName);
    if(pHolder)
    {
        pViewPortDockHost->OnDockedWindowDestroy(pHolder->pWidget);
        pEditorDockHost->OnDockedWindowDestroy(pHolder->pWidget);
        pHolder->OnWidgetDestroy();
    }

	if(sName == "Find Results")
	{
		rFindResultsWidget.OnWidgetDestroy();
	}

    if(sName == "Output") // :((( ToDo:  my be need number types for DockedWindow
    {
        TLoggerWidgets::Iterator it = g_vLoggerWidgets.Begin();
        while(it != g_vLoggerWidgets.End())
        {
            (*it)->OnWidgetDestroy();
            ++it;
        }
    }
}

bool Editor::Run(HINSTANCE instance, int nArgs, char** args)
{				

	gEditorInstance = instance;

	en_core.SetInstance(instance);

	InitGUI();
	
	en_core.SetTraceCallback(TraceCallback);

	en_core.EN_Files()->AddPool("System");
	en_core.EN_Files()->AddPool("Target");

	en_core.EN_Files()->ActivatePool("System");

	if (!core->Init()) return 0;

	en_core.EN_Files()->ActivatePool("");

	pGizmo = new Gizmo();

	core->Scene()->SetSceneLoadType(IStreamLoader::xml);
	

	en_core.SetEditorMode(true);

	SetActiveWindow(pMainWindow->GetHandle());
	
	pGizmo->enabled = false;

	pViewport->SetGizmo(pGizmo);		

	ClearProject();

	pSelectedEntity = null;
    pSelectedObject = null;

	
	view.Pos() = Vector(0.0f, 4.0f, -5.0f);


    pMainWindow->Show(true);
    pMainWindow->Maximaze();


	char scene[256];
	scene[0] = 0;
	ReadArg(nArgs, args, "scene", scene, 256);
    
	if (!String::IsEmpty(scene))
	{
        OpenProject(scene);
	}

    float nSceneInfoTimer = 0;
	while( !en_core.IsExited() )
	{
		core->CountDeltaTime(true);
		
        ProjectInfo::Target* pTarget  = pProject->CurTarget();
		if (pTarget)
		{
			if (pTarget->debug_info_type > 0) en_core.Render()->Debug()->PrintText(10, 10, up_right, COLOR_GREEN, "%i", core->GetFPS());		
			if (pTarget->debug_info_type == 2)
			{
				int tex_total = 0;
				int tex_loading = 0;
				int tex_memory = 0;

				en_core.Render()->GetTextureInfo(tex_total, tex_loading, tex_memory);

				float flt_tex_memory = tex_memory / (1024.0f * 1024.0f);

				en_core.Render()->Debug()->PrintText(10, 35, up_right, COLOR_GREEN, "C: %i L: %i M: %4.1f Mb GU: %i", tex_total, tex_loading, flt_tex_memory, NGUIWidget::num_updates);		
			}
		}

		if (core->IsEditorMode())
		{
			UpdateLoadingElems();
			ProcessEditorControls();
			ProcessEditorGUI();			
		}
		else
		{
			ProcessTestsceneGUI();
		}

		if (EventsWidget::CheckEvent(NETWORK_MENU_INTERNET_FLAG_ID, EventsWidget::menuitem_click))
		{
            if (pMainMenu->IsChecked(NETWORK_MENU_INTERNET_FLAG_ID))
			{
                pMainMenu->ModifyMenuItem(NETWORK_MENU_INTERNET_FLAG_ID, MF_UNCHECKED);
			}
            else
			{
                pMainMenu->ModifyMenuItem(NETWORK_MENU_INTERNET_FLAG_ID, MF_CHECKED);
			}

			core->Network()->SetInternetConnected(pMainMenu->IsChecked(NETWORK_MENU_INTERNET_FLAG_ID));			
		}

		EventsWidget::Update();		
		pGizmo->transform_started = false;

		ProcessMessageLoop();

		if ((en_core.IsEditorMode() && pMainWindow->IsMinimazed()) || (!en_core.IsEditorMode() && pGameWindow->IsMinimazed())) 
		{
			ThreadWorker::Sleep(100);
			continue;
		}

		core->Work();		
	}

 	core->Release();

	return 0;
}

void Editor::CreateMenu(WindowMenuWidget* pParent,MenuDescriptor* vMenuDeclarator,unsigned nCount)
{
    for(unsigned i =0 ;i < nCount;++i)
    {
        switch(vMenuDeclarator[i].eCommand)
        {
            case Editor::MenuDescriptor::eBeginSubMenu:pMainMenu->StartSubMenu(vMenuDeclarator[i].sName);break;
            case Editor::MenuDescriptor::eAddItem:pMainMenu->AddMenuItem(vMenuDeclarator[i].nId,vMenuDeclarator[i].sName,vMenuDeclarator[i].nFlag);break;
            case Editor::MenuDescriptor::eAddSeparator:pMainMenu->AddSeparator();break;
            case Editor::MenuDescriptor::eEndSubMenu:pMainMenu->EndSubMenu(vMenuDeclarator[i].sName);break;
            case Editor::MenuDescriptor::eRunCreator:
                {
                    MenuDescriptor& desc = vMenuDeclarator[i];
                    if(vMenuDeclarator[i].pCreator)
                    {
                        pMainMenu->StartSubMenu(vMenuDeclarator[i].sName);
                        (this->*(desc.pCreator))(pParent);
                        pMainMenu->EndSubMenu(vMenuDeclarator[i].sName);
                    }
                    break;
                }
        }
    }
}

void Editor::ScriptEditorColorSchemesMenu(WindowMenuWidget* pParent)
{
    for(unsigned i = 0 ;i < vScriptEditorColorSchemsNames.size();++i)
    {
        pMainMenu->AddMenuItem(SCRIPT_EDITOR_COLOR_SCHEME_ID_OFFSET + i,vScriptEditorColorSchemsNames[i].c_str());
        if(sScriptEditorInitedColorScheme == vScriptEditorColorSchemsNames[i])
        {
            nSelectedScriptEditorColorScheme = SCRIPT_EDITOR_COLOR_SCHEME_ID_OFFSET + i;
            pMainMenu->ModifyMenuItem(nSelectedScriptEditorColorScheme,MF_CHECKED);
        }
    }
}

void Editor::CreateRecentOpenProjects(WindowMenuWidget* pParent)
{
    CreateRecentPathsMenu(vRecentOpenProjects,RECENT_OPEN_PROJECTS_ID_OFFSET,pParent);
}

void Editor::CreateRecentOpenFiles(WindowMenuWidget* pParent)
{
    CreateRecentPathsMenu(vRecentOpenFiles,RECENT_OPEN_FILES_ID_OFFSET,pParent);
}

void Editor::CreateEntitiesMenu(WindowMenuWidget* pParent)
{
    TDeclarationsDescriptors vDeclarationsDescriptor;

    for (EntityDedeclarator* decl = EntityDedeclarator::GetFirst(); decl != NULL; decl = decl->GetNext())
    {
        if (!String::IsEqual(decl->GetName(), "NGUISymbolInstance") && !String::IsEqual(decl->GetName(), "NGUIWidget"))			
        {
            const char* sSheetName = decl->GetNodePanelSheetName();
            DeclarationsDescriptor* pDesc =  NULL;
            for(unsigned i = 0;i < vDeclarationsDescriptor.size();++i)
            {
                if(String::IsEqual(vDeclarationsDescriptor[i].GetName(),sSheetName))
                {
                    pDesc = &vDeclarationsDescriptor[i];
                    break;
                }
            }
            if(!pDesc)
            {
                vDeclarationsDescriptor.push_back(DeclarationsDescriptor(sSheetName));
                pDesc = &vDeclarationsDescriptor.back();
            }
            pDesc->Add(decl);
        }
    }	
    vEntitiesDeclarations.clear();
    std::sort(vDeclarationsDescriptor.begin(),vDeclarationsDescriptor.end(),DeclarationsDescriptor::Comparator());
    for(unsigned i = 0;i < vDeclarationsDescriptor.size();++i)
    {
        DeclarationsDescriptor& rDesc = vDeclarationsDescriptor[i];
        rDesc.Sort();
        pMainMenu->StartSubMenu(rDesc.GetName());
        for (unsigned j = 0; j < rDesc.Size();++j)
        {
           vEntitiesDeclarations.push_back(rDesc[j]);
           pMainMenu->AddMenuItem(450 + vEntitiesDeclarations.size() - 1, vEntitiesDeclarations.back()->GetNodePanelTexName());
        }
        pMainMenu->EndSubMenu(rDesc.GetName());
    }
}

void Editor::ShowObjectPanel(bool bCreate)
{
    DockedWidget::DockedWidgetHolder& rHolder = vDockedWidgets["Object"];
    if(!rHolder.pWidget)
    {
        DockedWidget::Descriptor desc("Object",RectWidth(rHolder.rSize),RectHeight(rHolder.rSize),240,540 - TabSheetWidget::gY_Offset,DHNS_NO_CLOSE,DH_CRIGHT,2);
        HDHNODE hBase = NULL;
        DockedWidget::DockedWidgetHolder* pScene = vDockedWidgets.Find("Scene");
        if(pScene && pScene->pWidget)
        {
            hBase = NULL;
            desc.nDockLocation = DH_RIGHT;
        }
        else
        {
            DockedWidget::DockedWidgetHolder* pViewPort = vDockedWidgets.Find("ViewPort");
            if(pViewPort && pViewPort->pWidget)
                hBase = pViewPort->pWidget->GetDockHandle();
            else desc.nDockLocation = DH_RIGHT;
        }
        rHolder.pWidget = pViewPortDockHost->AddDockedWidget(&desc,vDockedWidgets,hBase);
    }
    else
        if(!bCreate)
            return;
    if(rHolder.pWidget)
    {
        LabelWidget* pObjectPage = new LabelWidget(150,rHolder.pWidget, "", false,5,0,100,100,BaseWidget::eAutoSizeBoth);

        SizerWidget* pObjectTopSizer = new SizerWidget(1000, pObjectPage, true, 60);
        SizerWidget* pObjectBottomSizer = new SizerWidget(1000, pObjectPage, true, -1);	

        SizerWidget* pObjectTopLeftSizer = new SizerWidget(1000, pObjectTopSizer, false, -1);	
        SizerWidget* pObjectTopRightSizer = new SizerWidget(1000, pObjectTopSizer, false, 64);

        pEntityName = new TextBoxWidget(VIEW_PORT_ENTITY_NAME_ID, pObjectTopLeftSizer, "", 0, 5, 160, 20,BaseWidget::eAutoSizeWidth);
        pEntityName->Enable(false);
        pActiveButton = new ButtonWidget(VIEW_PORT_ENTITY_ACTIVE_BUTTON_ID, pObjectTopRightSizer, true, "active", 0, 0, 32, 32);
        pActiveButton->SetPushable(true);
        pActiveButton->SetImage("Editor/btn_activate.bmp");
        pActiveButton->SetPushedImage("Editor/btn_activate_sel.bmp");
        pActiveButton->SetHoweredImage("Editor/btn_activate_over.bmp");
        pActiveButton->SetHoweredPushedImage("Editor/btn_activate_sel_over.bmp");

        pVisbleButton = new ButtonWidget(VIEW_PORT_ENTITY_VISIBLE_BUTTON_ID, pObjectTopRightSizer, true, "visible", 32, 0, 32, 32);
        pVisbleButton->SetPushable(true);
        pVisbleButton->SetImage("Editor/btn_show.bmp");
        pVisbleButton->SetPushedImage("Editor/btn_show_sel.bmp");
        pVisbleButton->SetHoweredImage("Editor/btn_show_over.bmp");
        pVisbleButton->SetHoweredPushedImage("Editor/btn_show_sel_over.bmp");

        /*	pAssetsName = new TextBoxWidget(119, obj_prop_top_sizer, "", 90, 25, 110, 20, false);
        pAssetsName->Enable(false);
        pAssetsName->Show(false);*/

        pEntityClassName = new TextBoxWidget(VIEW_PORT_ENTITY_CLASS_NAME_ID, pObjectTopLeftSizer, "", 0, 30, 160, 20,BaseWidget::eAutoSizeWidth);	
        pEntityClassName->Enable(false);

        pEntityProperties = new SizerWidget(1000, pObjectBottomSizer, false, -1);	
        SizerWidget* pObjectBottomRightSizer = new SizerWidget(1000, pObjectBottomSizer, false, 17);


        pEntityPropertiesScroll = new ScrollBarWidget(178, pObjectBottomRightSizer, false, 0,0, 10,10,BaseWidget::eAutoSizeBoth);
        pEntityPropertiesScroll->Show(false);
        rHolder.pWidget->Resize();
    }
}

void Editor::ShowScenePanel(bool bCreate)
{
    DockedWidget::DockedWidgetHolder& rHolder = vDockedWidgets["Scene"];
    if(!rHolder.pWidget)
    {
        DockedWidget::Descriptor desc("Scene",RectWidth(rHolder.rSize),RectHeight(rHolder.rSize),240,540 - TabSheetWidget::gY_Offset,DHNS_NO_CLOSE,DH_CRIGHT,3);
        HDHNODE hBase = NULL;
        DockedWidget::DockedWidgetHolder* pViewPort = vDockedWidgets.Find("ViewPort");
        if(pViewPort && pViewPort->pWidget)hBase = pViewPort->pWidget->GetDockHandle();
        else desc.nDockLocation = DH_RIGHT;
        rHolder.pWidget = pViewPortDockHost->AddDockedWidget(&desc,vDockedWidgets,hBase);
    }
    else
        if(!bCreate)
            return;
    if(rHolder.pWidget)
    {
        TabSheetWidget* pTabs = new TabSheetWidget(PROJECTS_TABS_ID,rHolder.pWidget, "",0, 0, 800, 600, BaseWidget::eAutoSizeBoth);

        pTabs->AddTab("Entities");	
        int nFlags = TreeViewWidget::EFlags::eDraggEnable | TreeViewWidget::EFlags::eCreateElementsInTopFolders | TreeViewWidget::EFlags::eAllowRename |  TreeViewWidget::EFlags::eSorted;
        pSceneList = new TreeViewWidget(VIEW_PORT_SCENE_LIST_ID, pTabs, pTabs->GetXOffset(), pTabs->GetYOffset(), 205, 550,nFlags, BaseWidget::eAutoSizeBoth);
        pSceneList->AddImage("Editor/folder.bmp");
        pSceneList->AddImage("Editor/scene_elem.bmp");
        pSceneList->AddImage("Editor/scene_elem_inc.bmp");


        pTabs->AddTabWidget(0, pSceneList);

        pTabs->AddTab("GUI");

        rHolder.pWidget->Resize();

        int w = pTabs->GetWidth() - pTabs->GetXOffset();
        int h = pTabs->GetHeight() - pTabs->GetYOffset();
        DockedPanelWidget* pGuiDockPanel = new DockedPanelWidget(pTabs, pTabs->GetXOffset(), pTabs->GetYOffset(), w, h, BaseWidget::eAutoSizeBoth);
        DockedWidget* pGuiTopDock = new DockedWidget(pGuiDockPanel,&DockedWidget::Descriptor(w/2,h/2,DHNS_NO_TEAROFF,DH_TOP,2));
        DockedWidget* pGuiBottomDock = new DockedWidget(pGuiDockPanel,&DockedWidget::Descriptor(w/2,h/2,DHNS_NO_TEAROFF,DH_CBOTTOM,2),pGuiTopDock->GetDockHandle());
        pTabs->AddTabWidget(1, pGuiDockPanel);	
        nFlags = TreeViewWidget::EFlags::eCreateInstances | TreeViewWidget::EFlags::eDraggEnable | TreeViewWidget::EFlags::eCreateElementsInTopFolders | TreeViewWidget::EFlags::eAllowRename ;
        pGuiList = new TreeViewWidget(VIEW_PORT_GUI_LIST_ID, pGuiTopDock, 0, 0, 205, 200,nFlags, BaseWidget::eAutoSizeBoth);
        pGuiList->AddImage("Editor/folder.bmp");
        pGuiList->AddImage("Editor/scene_elem.bmp");
        pGuiList->AddImage("Editor/scene_elem_inc.bmp");


        nFlags = TreeViewWidget::EFlags::eCreateInstances | TreeViewWidget::EFlags::eDraggEnable | TreeViewWidget::EFlags::eAllowRename;
        pGuiSymbolsList = new TreeViewWidget(VIEW_PORT_SYMBOLS_LIST_ID, pGuiBottomDock, 0, 0, 205, 200,nFlags, BaseWidget::eAutoSizeBoth);
        pGuiSymbolsList->AddImage("Editor/folder.bmp");
        pGuiSymbolsList->AddImage("Editor/scene_elem.bmp");
        pGuiSymbolsList->AddImage("Editor/scene_elem_inc.bmp");
        pGuiTopDock->Resize();
        pGuiBottomDock->Resize();

        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            pScene->SetGuiTreeView(pGuiList);
            pScene->SetSceneTreeView (pSceneList);
            pScene->SetSymbolsTreeView(pGuiSymbolsList);
        }

    }
}

void Editor::ShowFindResults(TMatchesMap& vMatshes,const char* sSerchText)
{
	ShowFindResultsWindow(true);
	char sBuffer[256];
	if(!rFindResultsWidget.Widget())
	{
		String::Printf(sBuffer,256,"Find Results of % \"%s\"",sSerchText);
		pFindResultsTabs->Resize();
		pFindResultsTabs->AddTabWidget(pFindResultsTabs->AddTab(sBuffer),rFindResultsWidget.CreateWidget(pFindResultsTabs,sBuffer,FIND_RESULTS_LIST_ID));
	}
	TMatchesMap::Iterator it = vMatshes.Begin();
	rFindResultsWidget.Clear();
	while(it != vMatshes.End())
	{
		string sPath = it.GetKey();
		if(sPath.find(':') == string::npos )
			String::GetAbsalutPath(sPath,core->GetProjectPath());
		TMatches* pList = (*it);
		for(unsigned i = 0; i < pList->size();++i)
		{
			const SearchMatch& rMatcher = pList->at(i);
			String::Printf(sBuffer,256,"%s(%d,%d): %s",sPath.c_str(),rMatcher.nRow,rMatcher.nCol,rMatcher.sLine.c_str());
			rFindResultsWidget.AddString(sBuffer,TextListWidget::eFindResult,it.GetKey());
		}
		++it;
	}
}

void Editor::ShowFindResultsWindow(bool bCreate)
{
	 DockedWidget::DockedWidgetHolder& rHolder =  vDockedWidgets["Find Results"];
	 if(!rHolder.pWidget)
	 {
		 DockedWidget::Descriptor desc("Find Results",RectWidth(rHolder.rSize),RectHeight(rHolder.rSize),200,190,0,DH_BOTTOM,1);
		 HDHNODE hBase = NULL;
		 DockedWidget::DockedWidgetHolder* pOutput = vDockedWidgets.Find("Output");
		 if(pOutput && pOutput->pWidget)
		 {
			 hBase = pOutput->pWidget->GetDockHandle();
			 desc.nDockLocation = DH_CRIGHT;
			 desc.nHeigth = RectHeight(pOutput->rSize);
			 desc.nWidth = RectWidth(pOutput->rSize) / 2;
			 
		 }
		 rHolder.pWidget = pEditorDockHost->AddDockedWidget(&desc,vDockedWidgets,hBase);
	 }
	 else
		 if(!bCreate)
			 return;
	 if(rHolder.pWidget)
		 pFindResultsTabs = new TabSheetWidget(FIND_RESULTS_TABS_ID,rHolder.pWidget, "",0, 0,500,200,BaseWidget::eAutoSizeBoth);
}

void Editor::ShowLoggerWindow(bool bCreate)
{
    DockedWidget::DockedWidgetHolder& rHolder =  vDockedWidgets["Output"];
    if(!rHolder.pWidget)
        rHolder.pWidget = pEditorDockHost->AddDockedWidget(&DockedWidget::Descriptor("Output",RectWidth(rHolder.rSize),RectHeight(rHolder.rSize),200,190,0,DH_BOTTOM,2),vDockedWidgets);
    else
        if(!bCreate)
            return;
    if(rHolder.pWidget)
    {
        g_pLogTabs = new TabSheetWidget(LOGS_TABS_ID,rHolder.pWidget, "",0, 0,500,200,BaseWidget::eAutoSizeBoth);
        if(g_vLoggerWidgets.IsEmpty())
        {
            g_vLoggerWidgets["Output"] = TextListWidget(g_pLogTabs,"Output",LOGGER_LIST_ID);
            g_pLogTabs->AddTab("Output");
            g_pLogTabs->AddTabWidget(0, g_vLoggerWidgets["Output"].Widget());
        }
        else
        {
            TextListWidget* pOutput = Editor::g_vLoggerWidgets.Find("Output");
            g_pLogTabs->AddTabWidget(g_pLogTabs->AddTab("Output"),pOutput->CreateWidget(g_pLogTabs,"Output",LOGGER_LIST_ID));
            TLoggerWidgets::Iterator it = g_vLoggerWidgets.Begin();
            while(it != g_vLoggerWidgets.End())
            {
                if(pOutput != *it)
                {
                    string sName = it.GetKey();
                    g_pLogTabs->AddTabWidget(g_pLogTabs->AddTab(sName.c_str()),(*it)->CreateWidget(g_pLogTabs,sName.c_str(),LOGGER_LIST_ID));
                }
                ++it;
            }
        }
        rHolder.pWidget->Resize();
    }
}

void Editor::ShowProjectPanel(bool bCreate)
{
    DockedWidget::DockedWidgetHolder& rHolder = vDockedWidgets["Projects"];
    if(!rHolder.pWidget)
        rHolder.pWidget = pEditorDockHost->AddDockedWidget(&DockedWidget::Descriptor("Projects",RectWidth(rHolder.rSize),RectHeight(rHolder.rSize),190,550,DHNS_NO_CLOSE,DH_LEFT,3),vDockedWidgets);
    else
        if(!bCreate)
            return;
    if(rHolder.pWidget)
    {
        pProjectTabs = new TabSheetWidget(PROJECTS_TABS_ID, rHolder.pWidget, "", 0, 0, 800, 550, BaseWidget::eAutoSizeBoth);
        pProjectTabs->AddTab("Project");
        int nFlags = TreeViewWidget::EFlags::eDraggEnable | TreeViewWidget::EFlags::eSorted | TreeViewWidget::EFlags::eAllowRename;
        pProjectList = new TreeViewWidget(PROJECT_LIST_ID, pProjectTabs,pProjectTabs->GetXOffset(),pProjectTabs->GetYOffset(), 205, 550, nFlags, BaseWidget::eAutoSizeBoth);
        pProjectList->AddImage("Editor/folder.bmp");
        pProjectList->AddImage("Editor/scene_image.bmp");
        pProjectList->AddImage("Editor/scene_start_image.bmp");
        pProjectList->AddImage("Editor/scene_elem.bmp");
        pProjectList->AddImage("Editor/inc_scene_image.bmp");
        pProjectTabs->AddTabWidget(0,pProjectList);
        rHolder.pWidget->Resize();
        ProjectInfo::Target* pTarget = pProject->CurTarget();
        if(pTarget)
           pTarget->SetTreeView(pProjectList);
    }
}

void Editor::ParseRecentPaths(const string& sBuffer,TRecentPaths& vList)
{
    string sPath;

    for(unsigned i =0;i < sBuffer.size();++i)
    { 
        if(sBuffer[i] == '{')return;
        if(sBuffer[i] == ';' && !sPath.empty())
        {
            vList.push_back(sPath);
            sPath.clear();
        }
        else sPath += sBuffer[i];
    }
}

void Editor::AddRecentPath(const string& sPath,TRecentPaths& vList,const string& sType)
{
    TRecentPaths::iterator it = vList.begin();
    it = vList.begin();
    for(;it !=  vList.end();++it)
    {
        if(sPath == (*it))
          break;
    }
    if(it != vList.end())
        vList.erase(it);

    if(vList.size() == 10)
        vList.pop_back();
    vList.push_front(sPath);

    string sBuffer;
    for(TRecentPaths::iterator it = vList.begin() ;it !=  vList.end();++it)
    {
        sBuffer += *it;
        sBuffer += ';';
    }
    WriteINI("settings.ini",sBuffer.c_str(),"GUI",sType.c_str());

}

void Editor::AddRecentProject(const string& sPath)
{
    string sBuffer = sPath;
    String::FixSlashes(&sBuffer[0]);
    AddRecentPath(sBuffer,vRecentOpenProjects,"recent_projects");
    pMainMenu->EnableSubMenu("Recent Projects",true);
    
	for (unsigned i = 0;i < 10;++i)
	{
        pMainMenu->Delete(RECENT_OPEN_PROJECTS_ID_OFFSET + i);
	}

    HMENU* pHandle = pMainMenu->GetSubMemu("Recent Projects");
    
	if (pHandle)
	{
        CreateRecentPathsMenu(vRecentOpenProjects,RECENT_OPEN_PROJECTS_ID_OFFSET,pMainMenu,pHandle);
	}
}

void Editor::AddRecentFile(const string& sPath)
{
    AddRecentPath(sPath,vRecentOpenFiles,"recent_files");
    pMainMenu->EnableSubMenu("Recent Files",true);
    for(unsigned i = 0;i < 10;++i)
        pMainMenu->Delete(RECENT_OPEN_FILES_ID_OFFSET + i);

    HMENU* pHandle = pMainMenu->GetSubMemu("Recent Files");
    if(pHandle)
        CreateRecentPathsMenu(vRecentOpenFiles,RECENT_OPEN_FILES_ID_OFFSET,pMainMenu,pHandle);
}

void Editor::InitGUI()
{
    GetScriptEditorColorSchems();

    sScriptEditorInitedColorScheme = ReadINI("settings.ini", "default", "GUI", "script_editor_color_scheme");
    string sEditorWindowsLocation = ReadINI("settings.ini", "", "GUI", "editor_windows_location");
    int width = ReadINI("settings.ini", 1024, "GUI", "width");
    int height = ReadINI("settings.ini", 768, "GUI", "height");

    ParseRecentPaths(ReadINI("settings.ini", "", "GUI", "recent_projects"),vRecentOpenProjects);
    ParseRecentPaths(ReadINI("settings.ini", "", "GUI", "recent_files"),vRecentOpenFiles);


    pMainWindow = new EditorWindow(MAIN_WINDOW_ID, "Editor", false, 0, 0, width, height);		
    pMainMenu = new WindowMenuWidget(pMainWindow);
    CreateMenu(pMainMenu,g_vMenuDeclarator,sizeof(g_vMenuDeclarator)/sizeof(Editor::MenuDescriptor));

    pMainMenu->EnableSubMenu("Recent Projects",!vRecentOpenProjects.empty());
    pMainMenu->EnableSubMenu("Recent Files",!vRecentOpenFiles.empty());

    paintManagerUI.Init(pMainWindow->GetHandle());
    EventsWidget::SetMainWindow(pMainWindow);

    ///CreateControlBar(new SizerWidget(1000, pMainWindow, true, 40));
    TDockedWindowDescriptors vList;
    if(sEditorWindowsLocation.empty())
    {
       pEditorDockHost = new DockedPanelWidget(new SizerWidget(1000, pMainWindow, true, -1),4,0,1020,768, BaseWidget::eAutoSizeBoth);
       DockedWidget::Descriptor* pProjectsDoc = new  DockedWidget::Descriptor("Projects",200,568,190,550,DHNS_NO_CLOSE,DH_CLEFT,3);
       vList.push_back(new  DockedWidget::Descriptor("WorkSpace",810,568,800,550,DHNS_NO_TEAROFF,DH_CENTER,4,pProjectsDoc));
       vList.push_back(new  DockedWidget::Descriptor("Output",500,200,200,190,0,DH_BOTTOM,2));
       pEditorDockHost->CreateDockedWidgets(vList,vDockedWidgets);
    }
    else
        pEditorDockHost = new DockedPanelWidget(new SizerWidget(1000, pMainWindow, true, -1),4,0,sEditorWindowsLocation,vDockedWidgets,BaseWidget::eAutoSizeBoth);

    CreateStatusBar(new SizerWidget(1000, pMainWindow, true, 20));

    if(vDockedWidgets.Find("Projects"))
        ShowProjectPanel(true);

    CreateWorkSpace(vDockedWidgets.Find("WorkSpace")->pWidget);

    if(vDockedWidgets.Find("Output"))
        ShowLoggerWindow(true);


    if(vDockedWidgets.Find("Scene"))
        ShowScenePanel(true);

    if(vDockedWidgets.Find("Object"))
        ShowObjectPanel(true);

	if(vDockedWidgets.Find("FindResults"))
		ShowFindResultsWindow(true);

	en_core.SetHWND(pViewport->GetHandle());			

	pExportProgressWindow = new WindowWidget(444, "Export", true, true, 100, 100, 650,50);	

	pExportProgressLabel = new LabelWidget(445, pExportProgressWindow, "Label", false, 5, 5, 500, 25);		

	CreateSettingsWindow();	
    CreateDevicesWindow();	
	CreateSymbolImport();

    pScriptEditor->SetColorScheme(vScriptEditorColorSchems.Find(sScriptEditorInitedColorScheme));

    pSimulationOptions = new SimulationOtions;
}

void Editor::CreateWorkSpace(DockedWidget* pParent)
{
    pWorkSpaceTabs = new TabSheetWidget(WORK_SPACE_TABS_ID, pParent, "", 0, 0, 800, 550, BaseWidget::eAutoSizeBoth);

    pWorkSpaceTabs->AddTab("Viewport");
    pWorkSpaceTabs->AddTab("Script");

    string sWindowsLocation = ReadINI("settings.ini", "", "GUI", "viewport_windows_location");
    TDockedWindowDescriptors vList;
    if(sWindowsLocation.empty())
    {
        pViewPortDockHost = new DockedPanelWidget(pWorkSpaceTabs, pWorkSpaceTabs->GetXOffset(), pWorkSpaceTabs->GetYOffset(), 800, 550, BaseWidget::eAutoSizeBoth);
        vList.push_back(new  DockedWidget::Descriptor("ViewPort",300,550,290,540 - TabSheetWidget::gY_Offset,DHNS_NO_TEAROFF,DH_LEFT,4));
        vList.push_back(new  DockedWidget::Descriptor("Scene",250,550,240,540 - TabSheetWidget::gY_Offset,DHNS_NO_CLOSE,DH_RIGHT,3));
        vList.push_back(new  DockedWidget::Descriptor("Object",250,550,240,540 - TabSheetWidget::gY_Offset,DHNS_NO_CLOSE,DH_RIGHT,2));
        pViewPortDockHost->CreateDockedWidgets(vList,vDockedWidgets);
    }
    else
        pViewPortDockHost = new DockedPanelWidget(pWorkSpaceTabs, pWorkSpaceTabs->GetXOffset(), pWorkSpaceTabs->GetYOffset(),sWindowsLocation,vDockedWidgets, BaseWidget::eAutoSizeBoth);
   
    pWorkSpaceTabs->AddTabWidget(0, pViewPortDockHost);	



    pViewport = new EditorViewPort(VIEW_PORT_ID,vDockedWidgets.Find("ViewPort")->pWidget, "",0,0, 800, 600);
    pViewport->core_ed = this;

    pScriptTabs = new TabSheetWidget(SCRIPT_FILES_TABS_ID, pWorkSpaceTabs, "",pWorkSpaceTabs->GetXOffset(), pWorkSpaceTabs->GetYOffset(), 225, 590,BaseWidget::eAutoSizeBoth);
    pWorkSpaceTabs->AddTabWidget(1,pScriptTabs);
    pScriptEditor = new TextEditorWidget(pScriptTabs,0);
}

void Editor::CreateControlBar(SizerWidget* pSizer)
{
    ButtonWidget* btn_open = new ButtonWidget(102, pSizer, true, "open", 5, 5, 32, 32);
    btn_open->SetImage("Editor/btn_open.bmp");
    btn_open->SetHoweredImage("Editor/btn_open_over.bmp");



    ButtonWidget* btn_save = new ButtonWidget(103, pSizer, true, "save",37, 5, 32, 32);
    btn_save->SetImage("Editor/btn_save.bmp");
    btn_save->SetHoweredImage("Editor/btn_save_over.bmp");

    ButtonWidget* btn_save_as = new ButtonWidget(110, pSizer, true, "save all",69, 5, 32, 32);	
    btn_save_as->SetImage("Editor/btn_save_all.bmp");
    btn_save_as->SetHoweredImage("Editor/btn_save_all_over.bmp");	

    /*   pButtonGrid = new ComboBoxWidget(114, pSizer, "grid", 40, 10, 40, 150, false, false);

    {
    pButtonGrid->AddString("off");
    pButtonGrid->AddString("0.1");
    pButtonGrid->AddString("0.25");
    pButtonGrid->AddString("0.5");
    pButtonGrid->AddString("1");
    pButtonGrid->AddString("2");
    pButtonGrid->AddString("3");
    pButtonGrid->SetCurString(0);
    }

    pButtonGrid->Show(false);*/

    ButtonWidget* btn_move = new ButtonWidget(111, pSizer, true, "move", 115, 5, 32, 32);
    btn_move->SetPushable(true);
    btn_move->SetImage("Editor/btn_move.bmp");
    btn_move->SetPushedImage("Editor/btn_move_sel.bmp");
    btn_move->SetHoweredImage("Editor/btn_move_over.bmp");
    btn_move->SetHoweredPushedImage("Editor/btn_move_sel_over.bmp");

    ButtonWidget* btn_rot = new ButtonWidget(112, pSizer, true, "rot", 147, 5, 32, 32);
    btn_rot->SetPushable(true);
    btn_rot->SetImage("Editor/btn_rotate.bmp");
    btn_rot->SetPushedImage("Editor/btn_rotate_sel.bmp");
    btn_rot->SetHoweredImage("Editor/btn_rotate_over.bmp");
    btn_rot->SetHoweredPushedImage("Editor/btn_rotate_sel_over.bmp");

    ButtonWidget* btn_scale = new ButtonWidget(113, pSizer, true, "scale", 179, 5, 32, 32);
    btn_scale->SetPushable(true);
    btn_scale->SetImage("Editor/btn_scale.bmp");
    btn_scale->SetPushedImage("Editor/btn_scale_sel.bmp");
    btn_scale->SetHoweredImage("Editor/btn_scale_over.bmp");
    btn_scale->SetHoweredPushedImage("Editor/btn_scale_sel_over.bmp");

    ButtonWidget* btn_play = new ButtonWidget(109, pSizer, true, "play", 220, 5, 32, 32);	
    btn_play->SetImage("Editor/btn_start.bmp");	
    btn_play->SetHoweredImage("Editor/btn_start_over.bmp");

    ButtonWidget* btn_goto = new ButtonWidget(PROJECT_GOTO_FOLDER_ID, pSizer, true, "open project folder", 252, 5, 32, 32);	
    btn_goto->SetImage("Editor/btn_goto_prj.bmp");	
    btn_goto->SetHoweredImage("Editor/btn_goto_prj_over.bmp");
}

void Editor::CreateStatusBar(SizerWidget* pSizer)
{
    SizerWidget* status_left_sizer = new SizerWidget(1000, pSizer, false, -1);
    SizerWidget* status_right_sizer = new SizerWidget(1000, pSizer, false, 150);		

    pStatusEditLine = new LabelWidget(1890, status_right_sizer, "Line 0", false, 10, 4, 50,100);
    pStatusEditCol = new LabelWidget(1891, status_right_sizer, "Col 0", false, 60, 4, 50,100);
    pStatus = new LabelWidget(1892, status_left_sizer, "", false, 6, 4, 50,100,BaseWidget::eAutoSizeBoth);
}

void Editor::CreateSymbolImport()
{
	pSymbolImport = new WindowWidget(950, "Symbols import", false, true, 200, 200, 320, 310);
	
	pSymbolList = new ListBoxWidget(951, pSymbolImport, "list", 10, 10, 300, 250, true, true, BaseWidget::eAutoSizeNone);	

	ButtonWidget* prj_btn_ok = new ButtonWidget(952, pSymbolImport, false, "Ok", 150, 265, 70, 25);
	ButtonWidget* prj_btn_cancel = new ButtonWidget(953, pSymbolImport, false, "Cancel", 240, 265, 70, 25);
}

void Editor::CreateSettingsWindow()
{
	pProjectWindow = new WindowWidget(800, "Project Settings", false, true, 100, 100, 630, 590);

	TreeViewWidget* target_list = new TreeViewWidget(130, pProjectWindow, 10, 40, 170, 500);

	target_list->AddImage("Editor/folder.bmp");
	target_list->AddImage("Editor/scene_elem.bmp");
	target_list->AddImage("Editor/scene_elem_inc.bmp");

	
	ComboBoxWidget* target_type = new ComboBoxWidget(841, pProjectWindow, "ornt", 10, 12, 100, 150, false);		

	target_type->AddString("Todat Ext");
	target_type->AddString("Android Widget");

	ButtonWidget* target_add_btn = new ButtonWidget(809, pProjectWindow, false, "Add", 115, 12, 30, 20);
	ButtonWidget* target_del_btn = new ButtonWidget(809, pProjectWindow, false, "Del", 150, 12, 30, 20);

	TabSheetWidget* tabs = new TabSheetWidget(112, pProjectWindow, "", 190, 10, 430, 530);
		
	tabs->AddTab("Common");
	
	LabelWidget* prj_name_label = new LabelWidget(801, tabs, "Name", false, tabs->GetXOffset(), tabs->GetYOffset() + 4, 85, 25);		
	pProjectName = new TextBoxWidget(806, tabs, "path", tabs->GetXOffset() + 90, tabs->GetYOffset(), 200, 25);
		
	tabs->AddTabWidget(0, prj_name_label);
	tabs->AddTabWidget(0, pProjectName);

	LabelWidget* prj_icon_label = new LabelWidget(802, tabs, "Icon", false, tabs->GetXOffset(), tabs->GetYOffset() + 34, 85, 25);
	pProjectButtonPath = new ButtonWidget(807, tabs, false, "path", tabs->GetXOffset() + 90, tabs->GetYOffset() + 30, 200, 25);

	tabs->AddTabWidget(0, prj_icon_label);
	tabs->AddTabWidget(0, pProjectButtonPath);

	LabelWidget* prj_ornt_label = new LabelWidget(803, tabs, "Orientation", false, tabs->GetXOffset(), tabs->GetYOffset() + 64, 85, 25);		
	pProjectOrientation = new ComboBoxWidget(808, tabs, "ornt", tabs->GetXOffset() + 90, tabs->GetYOffset() + 60, 200, 150, false);
		
	pProjectOrientation->AddString("landscape");
	pProjectOrientation->AddString("vertical");
	pProjectOrientation->AddString("booth");	

	tabs->AddTabWidget(0, prj_ornt_label);
	tabs->AddTabWidget(0, pProjectOrientation);

	LabelWidget* prj_ref_width_name = new LabelWidget(826, tabs, "Reference Width", false, tabs->GetXOffset(), tabs->GetYOffset() + 94, 85, 25);		
	pProjectWidthRef = new TextBoxWidget(869, tabs, "path", tabs->GetXOffset() + 90, tabs->GetYOffset() + 90, 200, 25);

	tabs->AddTabWidget(0, prj_ref_width_name);
	tabs->AddTabWidget(0, pProjectWidthRef);

	LabelWidget* prj_ref_height_name = new LabelWidget(826, tabs, "Reference Height", false, tabs->GetXOffset(), tabs->GetYOffset() + 124, 85, 25);		
	pProjectHeightRef = new TextBoxWidget(847, tabs, "path", tabs->GetXOffset() + 90, tabs->GetYOffset() + 120, 200, 25);

	tabs->AddTabWidget(0, prj_ref_height_name);
	tabs->AddTabWidget(0, pProjectHeightRef);

	LabelWidget* prj_debug_infolabel = new LabelWidget(840, tabs, "DebugInfo", false, tabs->GetXOffset(), tabs->GetYOffset() + 154, 85, 25);		
	pProjectDebugInfo = new ComboBoxWidget(841, tabs, "ornt", tabs->GetXOffset() + 90, tabs->GetYOffset() + 150, 200, 150, false);		

	pProjectDebugInfo->AddString("None");
	pProjectDebugInfo->AddString("FPS");
	pProjectDebugInfo->AddString("FPS & Video");		

	tabs->AddTabWidget(0, prj_debug_infolabel);
	tabs->AddTabWidget(0, pProjectDebugInfo);

	LabelWidget* prj_ornt_fb_id = new LabelWidget(816, tabs, "facebook app id", false, tabs->GetXOffset(), tabs->GetYOffset() + 184, 85, 25);		
	pProjectFBid = new TextBoxWidget(817, tabs, "path", tabs->GetXOffset() + 90, tabs->GetYOffset() + 180, 200, 25);

	tabs->AddTabWidget(0, prj_ornt_fb_id);
	tabs->AddTabWidget(0, pProjectFBid);

	LabelWidget* prj_ornt_vk_id = new LabelWidget(818, tabs, "vk app id", false, tabs->GetXOffset(), tabs->GetYOffset() + 214, 85, 25);		
	pProjectVKid = new TextBoxWidget(819, tabs, "path", tabs->GetXOffset() + 90, tabs->GetYOffset() + 210, 200, 25);

	tabs->AddTabWidget(0, prj_ornt_vk_id);
	tabs->AddTabWidget(0, pProjectVKid);	
	
	LabelWidget* prj_need_isi = new LabelWidget(848, tabs, "Use Isi", false, tabs->GetXOffset(), tabs->GetYOffset() + 244, 85, 25);		
	pProjectNeedIsi = new CheckBoxWidget(849, tabs, "", tabs->GetXOffset() + 90, tabs->GetYOffset() + 240, 200, 25);

	tabs->AddTabWidget(0, prj_need_isi);
	tabs->AddTabWidget(0, pProjectNeedIsi);	

	tabs->AddTab("Android");
	
	LabelWidget* prj_ornt_pkg_name = new LabelWidget(810, tabs, "Package Name", false, tabs->GetXOffset(), tabs->GetYOffset() + 4, 85, 25);		
	pProjectPackegName = new TextBoxWidget(811, tabs, "path", tabs->GetXOffset() + 90, tabs->GetYOffset(), 200, 25);
	tabs->AddTabWidget(1, prj_ornt_pkg_name);
	tabs->AddTabWidget(1, pProjectPackegName);	

	LabelWidget* prj_ornt_ver_name = new LabelWidget(812, tabs, "versionName", false, tabs->GetXOffset(), tabs->GetYOffset() + 34, 85, 25);		
	pProjectVersionName = new TextBoxWidget(813, tabs, "path", tabs->GetXOffset() + 90, tabs->GetYOffset() + 30, 200, 25);
	tabs->AddTabWidget(1, prj_ornt_ver_name);
	tabs->AddTabWidget(1, pProjectVersionName);	

	LabelWidget* prj_ornt_ver_code = new LabelWidget(814, tabs, "versionCode", false, tabs->GetXOffset(), tabs->GetYOffset() + 64, 85, 25);		
	pProjectVersionCode = new TextBoxWidget(815, tabs, "path", tabs->GetXOffset() + 90, tabs->GetYOffset() + 60, 200, 25);
	tabs->AddTabWidget(1, prj_ornt_ver_code);
	tabs->AddTabWidget(1, pProjectVersionCode);	
		
	LabelWidget* prj_droid_theme_style_label = new LabelWidget(826, tabs, "Theme", false, tabs->GetXOffset(), tabs->GetYOffset() + 94, 85, 25);		
	pProjectThemeStyle_DROID = new ComboBoxWidget(845, tabs, "ornt", tabs->GetXOffset() + 90, tabs->GetYOffset() + 90, 200, 150, false);		
	tabs->AddTabWidget(1, prj_droid_theme_style_label);
	tabs->AddTabWidget(1, pProjectThemeStyle_DROID);	

	pProjectThemeStyle_DROID->AddString("Fullscreen");
	pProjectThemeStyle_DROID->AddString("NoTitleBar");
	pProjectThemeStyle_DROID->AddString("WithTitleBar");

	LabelWidget* prj_add_permissions_module_label = new LabelWidget(838, tabs, "Add permitions", false, tabs->GetXOffset(), tabs->GetYOffset() + 124, 85, 25);		
	pProjectAddPermissionsModule = new CheckBoxWidget(839, tabs, "", tabs->GetXOffset() + 90, tabs->GetYOffset() + 120, 200, 25);		
	tabs->AddTabWidget(1, prj_add_permissions_module_label);
	tabs->AddTabWidget(1, pProjectAddPermissionsModule);	

	tabs->AddTab("IOS");		

	LabelWidget* prj_ornt_bundle_name = new LabelWidget(820, tabs, "bundle name", false, tabs->GetXOffset(), tabs->GetYOffset() + 4, 85, 25);		
	pProjectBundleName = new TextBoxWidget(821, tabs, "path", tabs->GetXOffset() + 90, tabs->GetYOffset(), 200, 25);
	tabs->AddTabWidget(2, prj_ornt_bundle_name);
	tabs->AddTabWidget(2, pProjectBundleName);	

	LabelWidget* prj_ornt_bundle_short_name = new LabelWidget(824, tabs, "bundle short name", false, tabs->GetXOffset(), tabs->GetYOffset() + 34, 85, 25);		
	pProjectBundleShortName = new TextBoxWidget(825, tabs, "path", tabs->GetXOffset() + 90, tabs->GetYOffset() + 30, 200, 25);	
	tabs->AddTabWidget(2, prj_ornt_bundle_short_name);
	tabs->AddTabWidget(2, pProjectBundleShortName);	

	LabelWidget* prj_ornt_bundle_version = new LabelWidget(822, tabs, "bundle version", false, tabs->GetXOffset(), tabs->GetYOffset() + 64, 85, 25);		
	pProjectBundleVersion = new TextBoxWidget(823, tabs, "path", tabs->GetXOffset() + 90, tabs->GetYOffset() + 60, 200, 25);
	tabs->AddTabWidget(2, prj_ornt_bundle_version);
	tabs->AddTabWidget(2, pProjectBundleVersion);	

	LabelWidget* prj_ios_theme_style_label = new LabelWidget(826, tabs, "Theme", false, tabs->GetXOffset(), tabs->GetYOffset() + 94, 85, 25);		
	pProjectThemeStyle_IOS = new ComboBoxWidget(827, tabs, "ornt", tabs->GetXOffset() + 90, tabs->GetYOffset() + 90, 200, 150, false);		
	tabs->AddTabWidget(2, prj_ios_theme_style_label);
	tabs->AddTabWidget(2, pProjectThemeStyle_IOS);	

	pProjectThemeStyle_IOS->AddString("No Title bar");
	pProjectThemeStyle_IOS->AddString("Title bar ios 6");
	pProjectThemeStyle_IOS->AddString("Title bar ios 7");
				
	LabelWidget* prj_sb_is_white_label = new LabelWidget(850, tabs, "Is SBar White", false, tabs->GetXOffset(), tabs->GetYOffset() + 124, 85, 25);		
	pProjectIsWhite_SB = new CheckBoxWidget(851, tabs, "", tabs->GetXOffset() + 90, tabs->GetYOffset() + 120, 200, 25);	
	tabs->AddTabWidget(2, prj_sb_is_white_label);
	tabs->AddTabWidget(2, pProjectIsWhite_SB);			

	LabelWidget* prj_force_ios_bitmap_label = new LabelWidget(842, tabs, "Force bitmap", false, tabs->GetXOffset(), tabs->GetYOffset() + 154, 85, 25);		
	pProjectForceBitmap_IOS = new CheckBoxWidget(843, tabs, "", tabs->GetXOffset() + 90, tabs->GetYOffset() + 150, 200, 25);	
	tabs->AddTabWidget(2, prj_force_ios_bitmap_label);
	tabs->AddTabWidget(2, pProjectForceBitmap_IOS);	

	pProjectHackExtSceneButton = new ButtonWidget(852, tabs, false, "", tabs->GetXOffset(), tabs->GetYOffset() + 180, 70, 25);
	ButtonWidget* prj_btn_hack_ext_scn_del = new ButtonWidget(853, tabs, false, "Delete", tabs->GetXOffset() + 90, tabs->GetYOffset() + 180, 70, 25);
	tabs->AddTabWidget(2, pProjectHackExtSceneButton);
	tabs->AddTabWidget(2, prj_btn_hack_ext_scn_del);	

	ButtonWidget* prj_btn_ok = new ButtonWidget(809, pProjectWindow, false, "Ok", 550, 550, 70, 25);
}

void Editor::CreateDevicesWindow()
{
    DELETE(pDevicesWindow)
    pDevicesWindow = new WindowWidget(NETWORK_DEVICES_WINDOW_ID, "Mobile Devices", false, true, 100, 100, 400, 240);
    {		
        SizerWidget* gui_top_sizer = new SizerWidget(1000, pDevicesWindow, true, -1);			
        SizerWidget* gui_bottom_sizerH = new SizerWidget(1000, pDevicesWindow, true, 40);
        SizerWidget* gui_bottom_sizer_send = new SizerWidget(1000, gui_bottom_sizerH, false, 325);
        new SizerWidget(1000, gui_bottom_sizerH, false, -1);
        SizerWidget* gui_bottom_sizer_connect = new SizerWidget(1000, gui_bottom_sizerH, false, 75);

        pDevicesList = new ListBoxWidget(NETWORK_DEVICES_WINDOW_LIST_ID, gui_top_sizer, "list", 0,0, 260, 210, false, false, BaseWidget::eAutoSizeBoth);

        pDevicesConnectButton = new ButtonWidget(NETWORK_DEVICES_WINDOW_CONNECT_ID, gui_bottom_sizer_connect, false, "Connect",0, 10, 70, 20);
        pDevicesSendMessage = new TextBoxWidget(NETWORK_DEVICES_WINDOW_MESSAGE_ID, gui_bottom_sizer_send, "", 5, 10, 250, 20);
        pDevicesSendButton = new ButtonWidget(NETWORK_DEVICES_WINDOW_SEND_ID, gui_bottom_sizer_send, false, "Send", 260, 10, 50, 20);
        pDevicesSendMessage->Show(false);
        pDevicesSendButton->Show(false);
    }
}

void Editor::UpdateLoadingElems()
{
	if (pCurrentScene)
	{
		if (in_loading_scene && !pCurrentScene->IsLoadingPending() && !pCurrentScene->IsLoading())
		{			
            pCurrentScene->SetTaskHoldersOwnerActive(true);
            ProjectInfo::Scene* pScene = GetSceneInfo();
            if(pScene)
                pScene->LoadSceneTrees();
			UpdateSceneLists();		
			in_loading_scene = false;
		}
	}
}

void Editor::ProcessEditorControls()
{		
	if (pScriptTabs->GetCurrentTab() == 0)
	{
		if ((core->Controls()->ControlState(DIK_LCONTROL, ControlsService::Active) && core->Controls()->ControlState(DIK_Z) && pScriptTabs->GetCurrentTab() == 0) ||
			(EventsWidget::CheckEvent(185, EventsWidget::menuitem_click)))
		{
			HistoryBuffer::GetInstance()->MoveBackward();
		}
	
		if ((core->Controls()->ControlState(DIK_LCONTROL, ControlsService::Active) && core->Controls()->ControlState(DIK_Y) && pScriptTabs->GetCurrentTab() == 0) ||
			(EventsWidget::CheckEvent(186, EventsWidget::menuitem_click)))
		{
			HistoryBuffer::GetInstance()->MoveForward();				
		}
	}

	if (pViewport->IsFocused())
	{
		if (core->Controls()->ControlState(DIK_1))
			pGizmo->useLocalSpace = !pGizmo->useLocalSpace;

		if (core->Controls()->ControlState(DIK_2))
			pGizmo->mode = 0;

		if (core->Controls()->ControlState(DIK_3))
			pGizmo->mode = 1;

		if (core->Controls()->ControlState(DIK_4))
			pGizmo->mode = 2;

		float speed = 5.0f;

		if (core->Controls()->ControlState(DIK_LSHIFT, ControlsService::Active))
			speed = 1.0f;

		if (core->Controls()->ControlState(DIK_W, ControlsService::Active))
			view.Pos() += view.Vz() * core->GetDeltaTime() * speed;
			
		if (core->Controls()->ControlState(DIK_S, ControlsService::Active))
			view.Pos() -= view.Vz() * core->GetDeltaTime() * speed;

		if (core->Controls()->ControlState(DIK_A, ControlsService::Active))
			view.Pos() -= view.Vx() * core->GetDeltaTime() * speed;

		if (core->Controls()->ControlState(DIK_D, ControlsService::Active))
			view.Pos() += view.Vx() * core->GetDeltaTime() * speed;

		if (core->Controls()->ControlState(DIK_T, ControlsService::Active))
			view.Pos() += view.Vy() * core->GetDeltaTime() * speed;

		if (core->Controls()->ControlState(DIK_G, ControlsService::Active))
			view.Pos() -= view.Vy() * core->GetDeltaTime() * speed;
	}					

	if (!pMainWindow->IsMinimazed())
	{
		pViewport->Update();						
		pGizmo->Render();
	}

	if (pGizmo->transform_started && pSelectedEntity)
	{
		if (core->Controls()->ControlState(DIK_LCONTROL, ControlsService::Active))
			CopySelectedEntity();					
	}
			
	Matrix inv = view;
	inv.Inverse();

	en_core.Render()->Display()->SetView(inv);

	Matrix proj;
	proj.BuildProjection(45.0f, 1.0f/en_core.Render()->Display()->GetAspectRatio(), 0.01f, 5000.0f);
	en_core.Render()->Display()->SetProj(proj);
			
	if (pSelectedEntity)
	{			
		if (EventsWidget::CheckEvent(VIEW_PORT_ENTITY_ACTIVE_BUTTON_ID, EventsWidget::button_click))
			pSelectedEntity->activeOnStart = !pSelectedEntity->activeOnStart;

		if (EventsWidget::CheckEvent(VIEW_PORT_ENTITY_VISIBLE_BUTTON_ID, EventsWidget::button_click))
		{
			pSelectedEntity->showOnStart = !pSelectedEntity->showOnStart;
			pSelectedEntity->Show(pSelectedEntity->showOnStart);
            ProjectInfo::Scene* pScene = GetSceneInfo();
            if(pScene)
                pScene->OnEntityVisibleChange(pSelectedEntity);
		}

		if (EventsWidget::CheckEvent(VIEW_PORT_ENTITY_NAME_ID, EventsWidget::textbox_change))
		{				
			if (!String::IsEqual(pSelectedEntity->GetName(), pEntityName->GetText()))
            {
                ProjectInfo::Scene* pScene = GetSceneInfo();
                if(pScene)
                {
                    ProjectInfo::ISceneTree* pTree = pScene->GetEntityTree(pSelectedEntity);
                    if(pTree && pTree->Rename(pSelectedEntity->GetName(),pEntityName->GetText()))
                       pEntityName->SetText(pSelectedEntity->GetName());
                }
            }
		}			

        if(pGizmo->bViewPortResized)
        {
            pGizmo->bViewPortResized = false;
            pSelectedEntity->DataToGizmo();
        }
        else
		    pSelectedEntity->DataFromGizmo();
		pSelectedEntity->UpdateEditorWidgets();			

		Camera* cam = (Camera*)pSelectedEntity;

		UpdateEntitityScroll();

		BoundingBox box;
		pSelectedEntity->GetBoundingBox(box);

		Matrix tr;
		pSelectedEntity->GetTransform(tr);

		Vector center = (box.minV + box.maxV) * 0.5f;		

		tr.Pos() += center;								
		en_core.Render()->Debug()->DrawBox((box.maxV - box.minV), tr, COLOR_WHITE);
	}
    else
        if(pSelectedObject)
        {
            if (EventsWidget::CheckEvent(VIEW_PORT_ENTITY_NAME_ID, EventsWidget::textbox_change))
            {
                TreeViewWidget* pList = NULL;
                if(pSceneList->GetSelectedItem() == pSelectedObject)pList = pSceneList;
                else 
                    if(pGuiSymbolsList->GetSelectedItem() == pSelectedObject)pList = pGuiSymbolsList;
                if(pList)
                {
                    const char* name = pEntityName->GetText();
                    if(pList->CheckFolderName(pList->GetParent(pSelectedObject),name))
                        pList->SetItemText(pSelectedObject, pEntityName->GetText());
                    else
                    {
                        pEntityName->SetText(pList->GetItemText(pSelectedObject));
                    }
                }
            }
        }
}

void Editor::ClearProject()
{
	core->UserStats()->SetNamespace("");

    ProjectInfo::Target* pTarget = pProject->CurTarget();
    if(pTarget)
        pTarget->Clear();

    pScriptEditor->ClearDocuments();
    NGUIService::instance->ClearWidgetsLists();
	NGUIService::instance->symbol_widget = null;

	pProject->targets.resize(1);
	pProject->sel_target = 0;
    pTarget = pProject->CurTarget();
	if (!pTarget)
		return;
	pTarget->SetTreeView(pProjectList);
	SelectEntity(null);
	pCurrentScene = null;	

	editor_project.ResetProject();

	en_core.Render()->Display()->SetReferenceHeight(pTarget->ref_height);

    char tmp[512];
    String::Printf(tmp,512,"%s - Unsaved project",g_sEditorName.c_str());		
	pMainWindow->SetText(tmp);

    pProjectList->DeleteItems();
	UpdateSceneLists();
}

void Editor::ProcessEditorGUI()
{
    if(pSimulationOptions)
        if(pSimulationOptions->ProcessEvents())
            return;
    if(pDevicesList->IsVisible())
    {
        nDevicesWindowRefreshTimer += core->GetDeltaTime();
        if(nDevicesWindowRefreshTimer > NetLogger::g_nBroadcastTime)
        {
            GetDevicesList();
            nDevicesWindowRefreshTimer = 0;
        }
    }

    ProjectInfo::Target* pProjectTarget  = pProject->CurTarget();

	if ((pProject->orintation == 0 && pProjectTarget->ref_width > pProjectTarget->ref_height) ||
		(pProject->orintation != 0 && pProjectTarget->ref_width < pProjectTarget->ref_height))
	{
		NGUIService::instance->display_width = pProjectTarget->ref_width;
		NGUIService::instance->display_height = pProjectTarget->ref_height;
	}
	else
	{
		NGUIService::instance->display_width = pProjectTarget->ref_height;
		NGUIService::instance->display_height = pProjectTarget->ref_width;	
	}	

	core->Render()->Display()->SetReferenceHeight(NGUIService::instance->display_height);


    if (((ControlsService*)core->Controls())->ControlState(DIK_LCONTROL, ControlsService::Active) &&
		((ControlsService*)core->Controls())->ControlState(DIK_0, ControlsService::Activated))
	{
		NGUIWidget::editor_offset_x = 0;
		NGUIWidget::editor_offset_y = 0;
	}

	if (((ControlsService*)core->Controls())->MouseState(2, ControlsService::Activated))
	{
		prev_mx = core->Controls()->GetTouchX(0);
		prev_my = core->Controls()->GetTouchY(0);
		in_canvas_drag = true;
	}
	else
	{
		if (!((ControlsService*)core->Controls())->MouseState(2, ControlsService::Active))
			in_canvas_drag = false;

		if (in_canvas_drag)
		{			
			NGUIWidget::editor_offset_x += core->Controls()->GetTouchX(0) - prev_mx;
			NGUIWidget::editor_offset_y += core->Controls()->GetTouchY(0) - prev_my;

			prev_mx = core->Controls()->GetTouchX(0);
			prev_my = core->Controls()->GetTouchY(0);			
		}				
	}

    if (pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::treelist_labeledited))
    {
        ProjectInfo::Scene* pScene = static_cast<ProjectInfo::Scene*>(pProjectList->GetHandleData(pProjectList->GetSelectedItem()));
        if(pScene)
        {
            if(!IsSceneExists(pProjectList->GetEditedText()))
            {
                pProjectTarget->Rename(pScene,pProjectList->GetEditedText());
                pScene->Instance()->Save(pScene->Path(), IStreamSaver::xml);					
                pScriptEditor->SetSceneName(pScene->Path());
                editor_project.SaveProject();
            }
        }
        else
            pProjectTarget->rTree.Rename(pProjectList->GetSelectedItem(),pProjectList->GetEditedText());

    }

	if (pSceneList && EventsWidget::CheckEvent(pSceneList->GetID(), EventsWidget::treelist_labeledited))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            pScene->SceneTree()->Rename(pSceneList->GetSelectedItem());
            pEntityName->SetText(pSceneList->GetItemText(pSceneList->GetSelectedItem()));
        }
    }


    if (pGuiList &&  EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::treelist_labeledited))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            pScene->GuiTree()->Rename(pGuiList->GetSelectedItem());
            pEntityName->SetText(pGuiList->GetItemText(pGuiList->GetSelectedItem()));
        }

    }

    if (pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::treelist_labeledited))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            pScene->SymbolsTree()->Rename(pGuiSymbolsList->GetSelectedItem());
            pEntityName->SetText(pGuiSymbolsList->GetItemText(pGuiSymbolsList->GetSelectedItem()));
        }
    }

	if (EventsWidget::CheckEvent(800, EventsWidget::window_close))
	{
		CreateSettingsWindow();
	}

	if (EventsWidget::CheckEvent(950, EventsWidget::window_close))
	{
		RELEASE(imported_scene)
		imported_symbol_names.clear();	

		CreateSymbolImport();
	}

	if (EventsWidget::CheckEvent(952, EventsWidget::button_click))
	{
		MoveImortedSymbolsToCurScene();

		ProjectInfo::Scene* pScene = GetSceneInfo();

		if (pScene)
		{
			pScene->ReLoad();
		}

		pSymbolImport->Show(false);
	}	

	if (EventsWidget::CheckEvent(952, EventsWidget::button_click))
	{
		RELEASE(imported_scene)
		imported_symbol_names.clear();	

		pSymbolImport->Show(false);
	}

	if (EventsWidget::CheckEvent(806, EventsWidget::textbox_change))	
	{
		String::Copy(pProjectTarget->name, 128, pProjectName->GetText());
	}

	if (EventsWidget::CheckEvent(807, EventsWidget::button_click))
	{
		char name[512];
		name[0] = 0;
		if (DialogSelectFile(true, name, true, "Graphinc files(*.*)\0*.*\0"))
		{				
			char icon_path[512];
			String::Copy(icon_path, 512, name);

			en_core.EN_Files()->RelativePath(icon_path);
			String::FixSlashes(icon_path);

			String::Copy(pProjectTarget->icon_path, 128, icon_path);		
			pProjectButtonPath->SetText(pProjectTarget->icon_path);
		}
	}

	if (EventsWidget::CheckEvent(852, EventsWidget::button_click))
	{
		char name[512];
		name[0] = 0;
		if (DialogSelectFile(true, name, true, "Scene file(*.isx)\0*.isx\0"))
		{
			String::Copy(pProject->hack_ios_ext, 256, name);		
			pProjectHackExtSceneButton->SetText(pProject->hack_ios_ext);
		}
	}


     if(EventsWidget::CheckEvent(MENU_SHOW_LOGGER_ID, EventsWidget::menuitem_click))
         ShowLoggerWindow();
     if(EventsWidget::CheckEvent(MENU_SHOW_PROJECTS_ID, EventsWidget::menuitem_click))
         ShowProjectPanel();
     if(EventsWidget::CheckEvent(MENU_SHOW_SCENE_ID, EventsWidget::menuitem_click))
         ShowScenePanel();
     if(EventsWidget::CheckEvent(MENU_SHOW_OBJECT_ID, EventsWidget::menuitem_click))
         ShowObjectPanel();
  
    for(unsigned i = 0;i < vScriptEditorColorSchemsNames.size();++i)
    {
        if (EventsWidget::CheckEvent(SCRIPT_EDITOR_COLOR_SCHEME_ID_OFFSET + i, EventsWidget::menuitem_click))
        {
            pScriptEditor->SetColorScheme(vScriptEditorColorSchems.Find(vScriptEditorColorSchemsNames[i].c_str()));
            if(nSelectedScriptEditorColorScheme != -1)
                pMainMenu->ModifyMenuItem(nSelectedScriptEditorColorScheme,MF_UNCHECKED);
            nSelectedScriptEditorColorScheme = SCRIPT_EDITOR_COLOR_SCHEME_ID_OFFSET + i;
            pMainMenu->ModifyMenuItem(nSelectedScriptEditorColorScheme,MF_CHECKED);
            WriteINI("settings.ini",vScriptEditorColorSchemsNames[i].c_str(), "GUI", "script_editor_color_scheme");
            break;
        }
    }

    if(EventsWidget::CheckEvent(FIND_RESULTS_LIST_ID, EventsWidget::listbox_double_click))
    {
       ListBoxWidget* pLogger = static_cast<ListBoxWidget*>(EventsWidget::Sender());
	   string sText;
	   TextListWidget::TextType eType;
	   int index = pLogger->GetSelectedItem(0, sText, (int&)eType);
	   pScriptEditor->GoToFindResult(index,rFindResultsWidget.GetData(index));
    }

	if(EventsWidget::CheckEvent(LOGGER_LIST_ID, EventsWidget::listbox_double_click))
	{
		ListBoxWidget* pLogger = static_cast<ListBoxWidget*>(EventsWidget::Sender());
		string sName = pLogger->GetText();
		if(sName == "Output" || sName == "ScriptErr" || sName == "Script")
		{
			string sText;
			TextListWidget::TextType eType;
			pLogger->GetSelectedItem(0, sText, (int&)eType);

			if (eType == TextListWidget::eLogError && pScriptEditor->GoToScriptError(sText))
			{
				if (pWorkSpaceTabs->GetCurrentTab() == 0)
				{
					pWorkSpaceTabs->SetCurrentTab(1);
				}
			}
		}
	}

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_CLOSE_BUTTON_ID, EventsWidget::button_click))
    {
        int index  = pScriptEditor->GetCurDocumentSceneId();
        if(index != -1)
            pProjectTarget->vScenes[index]->Close();
        pScriptEditor->CloseScript();
    }

	if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_REDO_BUTTON_ID, EventsWidget::button_click))
		pScriptEditor->Redo();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_OPEN_ID, EventsWidget::button_click))
        pScriptEditor->ReOpenCurrentDocument();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_SAVE_ID, EventsWidget::button_click))
        pScriptEditor->SaveCurrentDocument();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_UNDO_BUTTON_ID, EventsWidget::button_click))
        pScriptEditor->Undo();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_GOTO_WINDOW_ID, EventsWidget::textbox_enter_press))
        pScriptEditor->GoToLine();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_SEARCH_TEXT_ID, EventsWidget::combobox_change))
        pScriptEditor->ReFind();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_SEARCH_MATCH_CASE_ID, EventsWidget::button_click))
        pScriptEditor->OnChangeFindFlags();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_SEARCH_MATCH_WORD_ID, EventsWidget::button_click))
        pScriptEditor->OnChangeFindFlags();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_SEARCH_NEXT_ID, EventsWidget::button_click))
        pScriptEditor->NextSearch();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_SEARCH_PREV_ID, EventsWidget::button_click))
        pScriptEditor->PrevSearch();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_SEARCH_TARGET_ID, EventsWidget::combobox_change))
        pScriptEditor->OnChangeFindTarget();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_CLASSES_LIST_ID, EventsWidget::combobox_change))
        pScriptEditor->OnClassesChange();

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_MEMBERS_LIST_ID, EventsWidget::combobox_change))
        pScriptEditor->OnMembersChange();



    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_SEARCH_REPLACE_ID, EventsWidget::button_click))
        pScriptEditor->Replace();
    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_SEARCH_REPLACEAll_ID, EventsWidget::button_click))
        pScriptEditor->ReplaceAll();

    if (EventsWidget::CheckEvent(853, EventsWidget::button_click))
        pProject->hack_ios_ext[0] = 0;	

	if (EventsWidget::CheckEvent(811, EventsWidget::textbox_change))
		String::Copy(pProject->package_name, 128, pProjectPackegName->GetText());

	if (EventsWidget::CheckEvent(813, EventsWidget::textbox_change))
		String::Copy(pProject->package_version_name, 128, pProjectVersionName->GetText());

	if (EventsWidget::CheckEvent(815, EventsWidget::textbox_change))
		String::Copy(pProject->package_code_name, 128, pProjectVersionCode->GetText());

	if (EventsWidget::CheckEvent(817, EventsWidget::textbox_change))
		String::Copy(pProject->fb_id, 128, pProjectFBid->GetText());

	if (EventsWidget::CheckEvent(819, EventsWidget::textbox_change))
		String::Copy(pProject->vk_id, 128, pProjectVKid->GetText());


	if (EventsWidget::CheckEvent(849, EventsWidget::checkbox_click))
	{
		if (pProjectNeedIsi->IsChecked())
		{
			pProject->need_isi = 1;
		}
		else
		{
			pProject->need_isi = 0;
		}		

		en_core.EN_Scene()->SetIsNeedIsi(pProject->need_isi);
	}


	if (EventsWidget::CheckEvent(821, EventsWidget::textbox_change))
		String::Copy(pProject->bundle_name, 128, pProjectBundleName->GetText());

	if (EventsWidget::CheckEvent(823, EventsWidget::textbox_change))
		String::Copy(pProject->bundle_version, 128, pProjectBundleVersion->GetText());


	if (EventsWidget::CheckEvent(825, EventsWidget::textbox_change))
		String::Copy(pProject->bundle_short_name, 128, pProjectBundleShortName->GetText());


	if (EventsWidget::CheckEvent(869, EventsWidget::textbox_change))
	{		
		pProjectTarget->ref_width = atoi(pProjectWidthRef->GetText());		
	}	

	if (EventsWidget::CheckEvent(847, EventsWidget::textbox_change))
	{		
		pProjectTarget->ref_height = atoi(pProjectHeightRef->GetText());
		en_core.Render()->Display()->SetReferenceHeight(pProjectTarget->ref_height);
	}		

	if (EventsWidget::CheckEvent(808, EventsWidget::combobox_change))
	{
		pProject->orintation = pProjectOrientation->GetCurString();
	}

	if (EventsWidget::CheckEvent(827, EventsWidget::combobox_change))
		String::Copy(pProject->ios_theme, 128, pProjectThemeStyle_IOS->GetText());
	
	if (EventsWidget::CheckEvent(851, EventsWidget::checkbox_click))
	{
		if (pProjectIsWhite_SB->IsChecked())
			pProject->status_bar_is_white = 1;
		else
			pProject->status_bar_is_white = 0;
	}

	if (EventsWidget::CheckEvent(845, EventsWidget::combobox_change))
		String::Copy(pProject->droid_theme, 128, pProjectThemeStyle_DROID->GetText());

	
	if (EventsWidget::CheckEvent(839, EventsWidget::checkbox_click))
	{
		if (pProjectAddPermissionsModule->IsChecked())
			pProject->additional_permissions = 1;
		else
			pProject->additional_permissions = 0;
	}	

	if (EventsWidget::CheckEvent(841, EventsWidget::combobox_change))
		pProjectTarget->debug_info_type = pProjectDebugInfo->GetCurString();

	if (EventsWidget::CheckEvent(843, EventsWidget::checkbox_click))
	{
		if (pProjectAddPermissionsModule->IsChecked())
			pProjectTarget->force_ios_bitmap = 1;
		else
			pProjectTarget->force_ios_bitmap = 0;
	}	

	if (EventsWidget::CheckEvent(809, EventsWidget::button_click))
	{
		pProjectWindow->Show(false);
		SaveProject();
	}

    if (EventsWidget::CheckEvent(NETWORK_DEVICES_WINDOW_ID, EventsWidget::window_close))
            CreateDevicesWindow();

    if (EventsWidget::CheckEvent(NETWORK_DEVICES_WINDOW_ID, EventsWidget::window_show))
        GetDevicesList();

    if (EventsWidget::CheckEvent(NETWORK_DEVICES_WINDOW_LIST_ID, EventsWidget::listbox_sel_change))
    {
        NetLogger* pNetLogger =  en_core.GetNetLogger();
        if(pNetLogger)
        {
            string sSelected;
            pDevicesList->GetSelectedItem(0, sSelected);
            int nPos = sSelected.find_last_of(":");
            if(nPos != -1 && nPos + 1 < sSelected.size())
            {
                string sAddress;
                sAddress.assign(&sSelected[nPos + 1],sSelected.size() - (nPos + 1));
                if(pNetLogger->IsConnectedTo(sAddress))DevicesListSelectedConnected(true);
                else  DevicesListSelectedConnected(false);
            }
            else  DevicesListSelectedConnected(false);
        }
    }
    
    if (EventsWidget::CheckEvent(NETWORK_DEVICES_WINDOW_SEND_ID, EventsWidget::button_click))
    {
        string sMessage  = pDevicesSendMessage->GetText();
        NetLogger* pNetLogger =  en_core.GetNetLogger();
        if(pNetLogger && !sMessage.empty())
            pNetLogger->SendCommand(sMessage);
    }
    if (EventsWidget::CheckEvent(NETWORK_DEVICES_WINDOW_CONNECT_ID, EventsWidget::button_click))
    {
        NetLogger* pNetLogger =  en_core.GetNetLogger();
        if(pNetLogger)
        {
            if(pDevicesConnectButton->GetFlag())
            {
                pNetLogger->Disconnect();
                DevicesListSelectedConnected(false);
            }
            else
            {
                string sSelected;
                pDevicesList->GetSelectedItem(0, sSelected);
                int nPos = sSelected.find_last_of(":");
                if(nPos != -1 && nPos + 1 < sSelected.size())
                {
                    string sAddress;
                    sAddress.assign(&sSelected[nPos + 1],sSelected.size() - (nPos + 1));
                    if(pNetLogger->ConnectTo(sAddress))DevicesListSelectedConnected(true);
                }
            }
        }
    }

	if (EventsWidget::CheckEvent(901, EventsWidget::button_click))
		pSceneIncludeWindow->Show(false);

    if (pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::popupmenu_include_scene))
	{
		char name[512];
		name[0] = 0;
		if (DialogSelectFile(true, name, true, "Scene file(*.isx)\0*.isx\0"))
		{			
			en_core.EN_Files()->RelativePath(name);
			String::FixSlashes(name);
			ProjectInfo::Scene* pScene = static_cast<ProjectInfo::Scene*>(pProjectList->GetHandleData(pProjectList->GetPopUpItem()));
			bool exists = false;
			for (size_t i=0; i<pScene->vIncludes.size(); i++)
			{
				if (String::IsEqual(pScene->vIncludes[i].c_str(), name))
				{
					exists = true;
					//FIX ME!!! need show MSG warning
				}
			}
			if (!exists)
			{				
                pProjectTarget->rTree.AddIncludeScene(pScene,name);
                if(pScene->Instance() && !pScene->Instance()->IsLoading())
				    ReloadScene(pScene);
			}
		}
	}

    if (pGuiList && EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::popupmenu_get_full_whidget_name))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            ProjectInfo::Entity* pEntity =   pScene->GuiTree()->GetEntity(pGuiList->GetPopUpItem()); 
            if(pEntity && pEntity->IsWidget() && pEntity->GetEngineEntity())
            {
                NGUIWidget* pWidget = static_cast<NGUIWidget*>(pEntity->GetEngineEntity());
                core->Mobile()->SetTextToClipboard(pWidget->GetFullName());
            }
        }
    }
    if (pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::popupmenu_get_full_whidget_name))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            ProjectInfo::Entity* pEntity =   pScene->SymbolsTree()->GetEntity(pGuiSymbolsList->GetPopUpItem()); 
            if(pEntity && pEntity->IsWidget() && pEntity->GetEngineEntity())
            {
                NGUIWidget* pWidget = static_cast<NGUIWidget*>(pEntity->GetEngineEntity());
                core->Mobile()->SetTextToClipboard(pWidget->GetFullName());
            }
        }
    }

    if (pGuiList && EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::popupmenu_paste))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            SelectEntity(NULL);
            ProjectInfo::ISceneTree* pSourceTree = static_cast< ProjectInfo::ISceneTree*>(pGuiList->GetPopUpCutTreeWidget()->GetTreeLogic());
            ProjectInfo::Entity* pEntity = pScene->GuiTree()->MoveItem(pSourceTree, pGuiList->GetPopUpCutTreeWidget()->GetPopUpCutItem(),pGuiList->GetPopUpItem());
            if(pEntity)
                SelectEntity(pEntity->GetEngineEntity());
            pGuiList->ClearCutData();
        }

    }

    if (pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::popupmenu_paste))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            SelectEntity(NULL);
            ProjectInfo::ISceneTree* pSourceTree = static_cast< ProjectInfo::ISceneTree*>(pGuiSymbolsList->GetPopUpCutTreeWidget()->GetTreeLogic());
            ProjectInfo::Entity* pEntity = pScene->SymbolsTree()->MoveItem(pSourceTree, pGuiSymbolsList->GetPopUpCutTreeWidget()->GetPopUpCutItem(),pGuiSymbolsList->GetPopUpItem());
            if(pEntity)
                SelectEntity(pEntity->GetEngineEntity());
            pGuiSymbolsList->ClearCutData();
        }

    }


    if (pSceneList && EventsWidget::CheckEvent(pSceneList->GetID(), EventsWidget::popupmenu_paste))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            SelectEntity(NULL);
            ProjectInfo::ISceneTree* pSourceTree = static_cast< ProjectInfo::ISceneTree*>(pSceneList->GetPopUpCutTreeWidget()->GetTreeLogic());
            ProjectInfo::Entity* pEntity = pScene->SceneTree()->MoveItem(pSourceTree, pSceneList->GetPopUpCutTreeWidget()->GetPopUpCutItem(),pSceneList->GetPopUpItem());
            if(pEntity)
                SelectEntity(pEntity->GetEngineEntity());
            pSceneList->ClearCutData();
        }

    }


    if (pSceneList && EventsWidget::CheckEvent(pSceneList->GetID(), EventsWidget::treelist_drag_ended))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            ProjectInfo::ISceneTree* pSourceTree = static_cast< ProjectInfo::ISceneTree*>(pSceneList->GetDragSourceWidget()->GetTreeLogic());
            ProjectInfo::Entity* pEntity = pScene->SceneTree()->MoveItem(pSourceTree, pSceneList->GetDragSourceWidget()->GetDragegItem(),pSceneList->GetDragTargetItem());
            if(pEntity)
                SelectEntity(pEntity->GetEngineEntity());
        }
    }

    if (pGuiList && EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::treelist_drag_ended))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            ProjectInfo::ISceneTree* pSourceTree = static_cast< ProjectInfo::ISceneTree*>(pGuiList->GetDragSourceWidget()->GetTreeLogic());
            ProjectInfo::Entity* pEntity = pScene->GuiTree()->MoveItem(pSourceTree, pGuiList->GetDragSourceWidget()->GetDragegItem(),pGuiList->GetDragTargetItem());
            if(pEntity)
                SelectEntity(pEntity->GetEngineEntity());
        }
    }

    if (pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::treelist_drag_ended))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            ProjectInfo::ISceneTree* pSourceTree = static_cast< ProjectInfo::ISceneTree*>(pGuiSymbolsList->GetDragSourceWidget()->GetTreeLogic());
            ProjectInfo::Entity* pEntity = pScene->SymbolsTree()->MoveItem(pSourceTree, pGuiSymbolsList->GetDragSourceWidget()->GetDragegItem(),pGuiSymbolsList->GetDragTargetItem());
            if(pEntity)
                SelectEntity(pEntity->GetEngineEntity());
        }

    }

    if (pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::treelist_drag_ended))
    {
        ProjectInfo::Target* pTarget  = pProject->CurTarget();
        if(pTarget)
        {
            pTarget->rTree.MoveItem(pProjectList->GetDragegItem(),pProjectList->GetDragTargetItem());
        }
    }

    if (pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::popupmenu_create_folder))
    {
        pProjectList->CreateFolder(NULL,pProjectList->GetPopUpItem(),NULL);
    }

    if (pSceneList && EventsWidget::CheckEvent(pSceneList->GetID(), EventsWidget::popupmenu_create_folder))
    {
        pSceneList->CreateFolder(NULL,pSceneList->GetPopUpItem(),NULL);
    }

    if (pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::popupmenu_create_folder))
    {
        pGuiSymbolsList->CreateFolder(NULL,pGuiSymbolsList->GetPopUpItem(),NULL);
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
            pScene->SymbolsTree()->Sort(pGuiSymbolsList->GetPopUpItem());
    }

    if (pGuiList && EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::popupmenu_copy))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
           ProjectInfo::Entity* pEntity = pScene->GuiTree()->CopyEntity(pGuiList->GetPopUpItem());
           if(pEntity)
               SelectEntity(pEntity->GetEngineEntity());
        }
        pGuiList->ClearPopUpItem();
    }
    if(pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::popupmenu_copy))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            ProjectInfo::Entity* pEntity = pScene->SymbolsTree()->CopyEntity(pGuiSymbolsList->GetPopUpItem());
            if(pEntity)
                SelectEntity(pEntity->GetEngineEntity());
        }
        pGuiSymbolsList->ClearPopUpItem();
    }
    if(pSceneList && EventsWidget::CheckEvent(pSceneList->GetID(), EventsWidget::popupmenu_copy) )
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            ProjectInfo::Entity* pEntity = pScene->SceneTree()->CopyEntity(pSceneList->GetPopUpItem());
            if(pEntity)
                SelectEntity(pEntity->GetEngineEntity());
        }
        pSceneList->ClearPopUpItem();
    }

    if(pGuiList && EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::popupmenu_convert_to_symbol))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            if(pSelectedObject == pGuiList->GetPopUpItem())
                SelectEntity(NULL);
            ProjectInfo::Entity* pEntity = pScene->GuiTree()->ConvertToSymbol(pGuiList->GetPopUpItem(), pScene->SymbolsTree());
            if(pEntity)
                SelectEntity(pEntity->GetEngineEntity());

        }
        pGuiList->ClearPopUpItem();

    }

    if (pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::popupmenu_convert_to_symbol))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            if(pSelectedObject == pGuiSymbolsList->GetPopUpItem())
                SelectEntity(NULL);

           ProjectInfo::Entity* pEntity = pScene->SymbolsTree()->ConvertToSymbol(pGuiSymbolsList->GetPopUpItem(), pScene->SymbolsTree());
           if(pEntity)
               SelectEntity(pEntity->GetEngineEntity());
        }
        pGuiSymbolsList->ClearPopUpItem();
    }

    if (pGuiList && EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::popupmenu_delete))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            if(pSelectedObject == pGuiList->GetPopUpItem())
            {
                SelectEntity(NULL);
            }
            else
            {
                if(pScene->GuiTree()->IsDeletedSelected(pGuiList->GetPopUpItem(),pSelectedObject))
                    SelectEntity(NULL);
            }

            pScene->GuiTree()->Delete(pGuiList->GetPopUpItem());
        }
        pGuiList->ClearPopUpItem();
    }

	if (pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::popupmenu_rollback_changes))
	{
		ProjectInfo::Entity* pEntity = static_cast<ProjectInfo::Entity*>(pGuiSymbolsList->GetHandleData(pGuiSymbolsList->GetPopUpItem()));
		if (pEntity)
        {
            SelectEntity(pEntity->GetEngineEntity());
			pEntity->RollBackLocalChanges();
        }
	}

	if (pSceneList && EventsWidget::CheckEvent(pSceneList->GetID(), EventsWidget::popupmenu_rollback_changes))
	{
		ProjectInfo::Entity* pEntity = static_cast<ProjectInfo::Entity*>(pSceneList->GetHandleData(pSceneList->GetPopUpItem()));
        if (pEntity)
        {
            SelectEntity(pEntity->GetEngineEntity());
            pEntity->RollBackLocalChanges();
        }

	}

    if (pGuiList && EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::popupmenu_rollback_changes))
    {
        ProjectInfo::Entity* pEntity = static_cast<ProjectInfo::Entity*>(pGuiList->GetHandleData(pGuiList->GetPopUpItem()));
        if (pEntity)
        {
            SelectEntity(pEntity->GetEngineEntity());
            pEntity->RollBackLocalChanges();
        }

    }

    if(pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::popupmenu_delete))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            if(pSelectedObject == pGuiSymbolsList->GetPopUpItem())
            {
                SelectEntity(NULL);
            }
            else
            {
                if(pScene->SymbolsTree()->IsDeletedSelected(pGuiSymbolsList->GetPopUpItem(),pSelectedObject))
                     SelectEntity(NULL);
            }
            pScene->SymbolsTree()->Delete(pGuiSymbolsList->GetPopUpItem());
        }
        pGuiSymbolsList->ClearPopUpItem();
    }
    if(pSceneList && EventsWidget::CheckEvent(pSceneList->GetID(), EventsWidget::popupmenu_delete) )
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            if(pSelectedObject == pSceneList->GetPopUpItem())
            {
                SelectEntity(NULL);
            }
            else
            {
                if(pScene->SceneTree()->IsDeletedSelected(pSceneList->GetPopUpItem(),pSelectedObject))
                    SelectEntity(NULL);
            }     
            pScene->SceneTree()->Delete(pSceneList->GetPopUpItem());
        }
        pSceneList->ClearPopUpItem();
    }

    if(pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::popupmenu_create_widget))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
            CreateWidgetEntity( pGuiSymbolsList->GetPopUpCreatingWidgetName().c_str(),pScene->SymbolsTree(),pGuiSymbolsList->GetPopUpItem());
         pGuiSymbolsList->ClearPopUpItem();
    }
    if(pGuiList && EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::popupmenu_create_widget) )
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
            CreateWidgetEntity( pGuiList->GetPopUpCreatingWidgetName().c_str(),pScene->GuiTree(),pGuiList->GetPopUpItem(),NULL,NGUIService::instance->TopWidgetsCount());
        pGuiList->ClearPopUpItem();
    }
    if(pSceneList && EventsWidget::CheckEvent(pSceneList->GetID(), EventsWidget::popupmenu_create_widget) )
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
            CreateWidgetEntity( pSceneList->GetPopUpCreatingWidgetName().c_str(),pScene->SceneTree(),pSceneList->GetPopUpItem());
         pSceneList->ClearPopUpItem();
    }

    if(pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::popupmenu_create_instance))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            ProjectInfo::Entity* pEntity = pScene->Find(pGuiSymbolsList->GetPopUpCreatingWidgetName());
            if(pEntity && pEntity->Type() == ProjectInfo::Entity::eSymbol)
            {
                CreateWidgetEntity("NGUISymbolInstance",pScene->SymbolsTree(),pGuiSymbolsList->GetPopUpItem(),pEntity->GetEngineEntity());
            }

        }
        pGuiSymbolsList->ClearPopUpItem();
    }
    if(pGuiList && EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::popupmenu_create_instance) )
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            ProjectInfo::Entity* pEntity = pScene->Find(pGuiList->GetPopUpCreatingWidgetName());
            if(pEntity && pEntity->Type() == ProjectInfo::Entity::eSymbol)
            {
                CreateWidgetEntity("NGUISymbolInstance",pScene->GuiTree(),pGuiList->GetPopUpItem(),pEntity->GetEngineEntity(),NGUIService::instance->TopWidgetsCount());
            }

        }
        pGuiList->ClearPopUpItem();
    }

    pScriptEditor->Update();

	if (!pMainWindow->IsActive()) return;
    
	{
		int cur_script_line = 0;
		int cur_script_col = 0;

		pScriptEditor->GetCursorState(cur_script_line, cur_script_col);

		char str[64];

		if (last_ed_line != cur_script_line)
		{
			last_ed_line = cur_script_line;				
			String::Printf(str, 64, "Line %i", cur_script_line);
			pStatusEditLine->SetText(str);
		}
				
		if (last_ed_col != cur_script_col)
		{
			last_ed_col = cur_script_col;
			String::Printf(str, 64, "Col %i", cur_script_col);
			pStatusEditCol->SetText(str);
		}
	}	

    if (EventsWidget::CheckEvent(SCRIPT_DOCUMENT_OPEN_ID, EventsWidget::menuitem_click) )
    {
        char name[512];
        name[0] = 0;
        if (DialogSelectFile(true, name, true, "Script file(*.srt)\0*.srt\0"))
            pScriptEditor->AddScript(name,NULL);
    }


    if(EventsWidget::CheckEvent(NETWORK_MENU_DEVICES_LIST_ID, EventsWidget::menuitem_click))
    {
       GetDevicesList();
       pDevicesWindow->Show(true);
       nDevicesWindowRefreshTimer = 0;
    }

    if(EventsWidget::CheckEvent(SIMULATION_OPTIONS_ID, EventsWidget::menuitem_click))
    {
        if(pSimulationOptions)
            pSimulationOptions->Show(true);
    }


	if (EventsWidget::CheckEvent(400, EventsWidget::menuitem_click))
	{
		SaveProject();
		ClearProject();
        SaveProjectAs();
	}
    if (EventsWidget::CheckEvent(PROJECT_GOTO_FOLDER_ID, EventsWidget::button_click))
    {
        if (editor_project.project_path[0])
        {
            char project_path[512];
            String::ExtractPath(editor_project.project_path,project_path,false);
            ShellExecute(NULL,NULL,"explorer.exe",(LPSTR)project_path,NULL,SW_SHOWNORMAL);
        }
    }


	if (EventsWidget::CheckEvent(401, EventsWidget::menuitem_click) ||
		EventsWidget::CheckEvent(102, EventsWidget::button_click))
	{
		char name[512];
		name[0] = 0;

		if (DialogSelectFile(true, name, true, "Project file(*.prx)\0*.prx\0"))
			OpenProject(name);
	}

	if (EventsWidget::CheckEvent(402, EventsWidget::menuitem_click))
	{
		SaveProject();		
	}

	if (EventsWidget::CheckEvent(403, EventsWidget::menuitem_click) ||
		(EventsWidget::CheckEvent(402, EventsWidget::menuitem_click) && !editor_project.project_path[0]))
	{
        SaveProjectAs();
	}	

	if (EventsWidget::CheckEvent(404, EventsWidget::menuitem_click) || 	EventsWidget::CheckEvent(103, EventsWidget::button_click))
	{
		if (pCurrentScene)
		{
			pCurrentScene->Save(pProject->CurScene()->Path(), IStreamSaver::xml);
			pScriptEditor->SaveScene(pProject->CurScene());
		}
	}

    if (EventsWidget::CheckEvent(405, EventsWidget::menuitem_click) )
    {
        if (pCurrentScene)
            SaveSceneAs(pProject->CurScene());
    }

    if (EventsWidget::CheckEvent(406, EventsWidget::menuitem_click) )
    {
        if (pCurrentScene)
            SaveSceneAsAndDuplicate(pProject->CurScene());
    }

	if ( EventsWidget::CheckEvent(110, EventsWidget::button_click))
	{
		if (editor_project.project_path[0])
			SaveProject();
        else
            SaveProjectAs();
	}

    if (EventsWidget::CheckEvent(408, EventsWidget::menuitem_click))
    {
        ShowProjectPanel();
        AddNewScene(false);
    }

	if (pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::popupmenu_add_new_scene))
        AddNewScene(false,pProjectList->GetPopUpItem());

    if (EventsWidget::CheckEvent(409, EventsWidget::menuitem_click))
    {
        ShowProjectPanel();
        AddExistingScene(false);
    }

	if (pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::popupmenu_add_existing_scene))
        AddExistingScene(false,pProjectList->GetPopUpItem());


    if( !in_loading_scene)
    {
        unsigned i = 0;
        for(TRecentPaths::iterator it = vRecentOpenProjects.begin();it != vRecentOpenProjects.end();++i,++it)
        {
            if (EventsWidget::CheckEvent(RECENT_OPEN_PROJECTS_ID_OFFSET + i, EventsWidget::menuitem_click))
            {
                OpenProject((*it).c_str());
                break;
            }
        }
        i = 0;
        for(TRecentPaths::iterator it = vRecentOpenFiles.begin();it != vRecentOpenFiles.end();++i,++it)
        {
            if (EventsWidget::CheckEvent(RECENT_OPEN_FILES_ID_OFFSET + i, EventsWidget::menuitem_click))
            {
                ProjectInfo::Scene* pScene = NULL;
                for (size_t i=0; i<pProjectTarget->vScenes.size(); i++)
                {
                    if(String::IsSubstr((*it).c_str(),pProjectTarget->vScenes[i]->Path()))
                    {
                        pScene = pProjectTarget->vScenes[i];
                        break;
                    }
                }
                if(pScene)
                {
                    if(!pScene->IsScript())
                        SetEdScene(pScene->Index());
                    else
                    {
                        if(pWorkSpaceTabs->GetCurrentTab() == 0)pWorkSpaceTabs->SetCurrentTab(1);
                        pScriptEditor->AddScript((*it).c_str(),pScene);
                    }
                }
                else
                {
                    string sPath = *it;
                    if(!sPath.empty())
                    {
                        if(sPath[sPath.size() - 1] == 't')
                        {
                            if(pWorkSpaceTabs->GetCurrentTab() == 0)pWorkSpaceTabs->SetCurrentTab(1);
                            pScriptEditor->AddScript((*it).c_str(),pScene);
                        }
                    }
                }
                break;
            }
        }

        if(pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::popupmenu_set_as_startup_scene))
        {
             ProjectInfo::Scene* pScene = static_cast<ProjectInfo::Scene*>(pProjectList->GetHandleData(pProjectList->GetPopUpItem()));
             pProjectTarget->SetStartScene(pScene);
        }

        if (EventsWidget::CheckEvent(410, EventsWidget::menuitem_click))
        {
            if (pCurrentScene)
            {
                pProjectTarget->SetStartScene(pProjectTarget->vScenes[pProjectTarget->cur_ed_scene]);
            }
        }

        if (EventsWidget::CheckEvent(411, EventsWidget::menuitem_click) )
            DeleteScene(pProjectTarget->cur_ed_file);

        if (pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::popupmenu_delete))
        {
            if(pProjectTarget->rTree.IsIncludedeSceneItem(pProjectList->GetPopUpItem()))
            {
                void* pSceneHandle = pProjectList->GetParent(pProjectList->GetPopUpItem());
                if(pSceneHandle)
                {
                    ProjectInfo::Scene* pScene = static_cast<ProjectInfo::Scene*>(pProjectList->GetHandleData(pSceneHandle));
                    pProjectTarget->rTree.DeleteIncludeScene(pScene,pProjectList->GetPopUpItem(),pProjectList->GetHandleData(pProjectList->GetPopUpItem()));
                    if(pScene->Instance() && !pScene->Instance()->IsLoading())
                        ReloadScene(pScene,true);
                }
            }
            else
                DeleteProjectObject(pProjectList->GetPopUpItem());
        }


        if (pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::popupmenu_save_scene))
        {
            ProjectInfo::Scene* pScene = static_cast<ProjectInfo::Scene*>(pProjectList->GetHandleData(pProjectList->GetPopUpItem()));
            if(pScene && pScene->Instance())
            {
                pScene->Instance()->Save(pScene->Path(), IStreamSaver::xml);
                pScriptEditor->SaveScene(pScene);
            }
        }


        if (pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::popupmenu_save_scene_as))
        {
            ProjectInfo::Scene* pScene = static_cast<ProjectInfo::Scene*>(pProjectList->GetHandleData(pProjectList->GetPopUpItem()));
            if (pScene->Instance())
                SaveSceneAs(pScene);
        }

        if (pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::popupmenu_save_scene_as_duplicate))
        {
            ProjectInfo::Scene* pScene = static_cast<ProjectInfo::Scene*>(pProjectList->GetHandleData(pProjectList->GetPopUpItem()));
            if (pScene->Instance())
                SaveSceneAsAndDuplicate(pScene);
        }

        if (pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::treelist_click))
        {
            ProjectInfo::Scene* pScene = NULL;
            if(pProjectTarget->rTree.IsIncludedeSceneItem(pProjectList->GetSelectedItem()))
            {
                void* pSceneHandle = pProjectList->GetParent(pProjectList->GetSelectedItem());
                if(pSceneHandle)
                    pScene = static_cast<ProjectInfo::Scene*>(pProjectList->GetHandleData(pSceneHandle));
            }
            else
              pScene = static_cast<ProjectInfo::Scene*>(pProjectList->GetHandleData(pProjectList->GetSelectedItem()));
            if(pScene)
                SetEdScene(pScene->Index());	
        }	
    }

	
	if (EventsWidget::CheckEvent(412, EventsWidget::menuitem_click))
	{
		core->Exit();
	}
		
	if (EventsWidget::CheckEvent(413, EventsWidget::menuitem_click))
    {
        ShowProjectPanel();
        AddNewScene(true);
    }
        
        
    if(pProjectList &&  EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::popupmenu_add_new_script))
        AddNewScene(true,pProjectList->GetPopUpItem());
        

	if (EventsWidget::CheckEvent(414, EventsWidget::menuitem_click))
    {
        ShowProjectPanel();
        AddExistingScene(true);
    }

    if(pProjectList && EventsWidget::CheckEvent(pProjectList->GetID(), EventsWidget::popupmenu_add_existing_script))
        AddExistingScene(true,pProjectList->GetPopUpItem());

	if (EventsWidget::CheckEvent(187, EventsWidget::menuitem_click))
		CopySelectedEntity();

	if (EventsWidget::CheckEvent(188, EventsWidget::menuitem_click) || (core->Controls()->ControlState(DIK_LSHIFT, ControlsService::Active) && core->Controls()->ControlState(DIK_DELETE)))
	{
		ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
        {
            ProjectInfo::ISceneTree* pTree = pScene->FindTree(pSelectedObject);
            if(pTree)
            {
                void* pOldSelectedObject = pSelectedObject;
                SelectEntity(NULL);
                pTree->Delete(pOldSelectedObject);
            }
        }
	}	
		
	if (EventsWidget::CheckEvent(189, EventsWidget::menuitem_click))
	{
		ExportResources(platform_undefined, false);		
	}
			
	if (EventsWidget::CheckEvent(190, EventsWidget::menuitem_click))
	{
		ExportResources(platform_pc, false);				
	}
		
	if (EventsWidget::CheckEvent(191, EventsWidget::menuitem_click))
	{
		ExportResources(platform_osx, false);						
	}

	if (EventsWidget::CheckEvent(192, EventsWidget::menuitem_click))
	{
		ExportResources(platform_ios, false);						
	}
		
	if (EventsWidget::CheckEvent(193, EventsWidget::menuitem_click))
	{		
		editor_project.android_tex_format = 0;
		ExportResources(platform_android, false);								
	}

	if (EventsWidget::CheckEvent(194, EventsWidget::menuitem_click))
	{
		editor_project.android_tex_format = 1;
		ExportResources(platform_android, false);								
	}

	if (EventsWidget::CheckEvent(195, EventsWidget::menuitem_click))
	{
		editor_project.android_tex_format = 2;
		ExportResources(platform_android, false);								
	}

	if (EventsWidget::CheckEvent(196, EventsWidget::menuitem_click))
	{
		editor_project.android_tex_format = 3;
		ExportResources(platform_android, false);								
	}

	if (EventsWidget::CheckEvent(197, EventsWidget::menuitem_click))
	{
		editor_project.android_tex_format = 4;
		ExportResources(platform_android, false);								
	}	

	if (EventsWidget::CheckEvent(260, EventsWidget::menuitem_click))
	{		
		if (pProject->CurScene()->Instance())
		{
			char path[64];

			if (DialogSelectFile(true, path, true, "Scene file(*.isx)\0*.isx\0"))
			{	
				imported_scene = new SceneInstance(en_core.EN_Scene(), "imported");

				imported_scene->Load(path, IStreamLoader::xml);		
				imported_scene->LoadImpl();
				imported_scene->ApplyEntityTaskList();

				int count = imported_scene->entities.size();

				pSymbolList->ClearList();

				for (int i=0; i<count; i++)
				{
					if (imported_scene->entities[i]->CheckClassName("NGUISymbol"))
					{
						pSymbolList->AddString(imported_scene->entities[i]->GetName(), 0);
					}
				}

				pSymbolImport->Show(true);
			}
		}
	}		

	if (EventsWidget::CheckEvent(222, EventsWidget::menuitem_click))
	{		
		editor_project.android_tex_format = 4;

		editor_project.PrepereExpParametrs(platform_android);		

		ExportResources(platform_android, true);								

		const char* tex_name = "";
        switch(editor_project.android_tex_format)
        {
			case 0:tex_name = "_bitmap";break;
			case 1:tex_name = "_atc";break;
			case 2:tex_name = "_pvr";break;
			case 3:tex_name = "_dds";break;
			case 4:tex_name = "_etc1";break;
        }

		char src[256];		
		String::Printf(src, 256, "%s/Export/android/%s%s/", en_core.EN_Files()->GetAppDirectory(), pProjectTarget->ex_name, tex_name);

		//MessageBox(main_window->GetHandle(), src, "Apk lies in follow path", 0);
		ShellExecute(null, "open", src, NULL, NULL, SW_SHOWNORMAL);
	}		
		
	if (EventsWidget::CheckEvent(223, EventsWidget::menuitem_click))
	{			
		char command[1024];			

		/*sprintf(command, "%sbin/adb.exe push %sExport/android/%s/Resource.pack /sdcard/Android/data/ion/Resource.pack", en_core.EN_Files()->GetAppDirectory(), en_core.EN_Files()->GetAppDirectory(), project->name);
		int ret = system(command);

		sprintf(command, "%sbin/adb.exe shell am start -a android.intent.action.MAIN -n com.android.ion/com.android.ion.IonActivity", en_core.EN_Files()->GetAppDirectory());
		ret = system(command);*/
	}	

	if (EventsWidget::CheckEvent(226, EventsWidget::menuitem_click))
	{	
		editor_project.PrepereExpParametrs(platform_ios);	
		
		ExportResources(platform_ios, true);		
		
		char src[256];		
		String::Printf(src, 256, "%s/Export/ios/%s/", en_core.EN_Files()->GetAppDirectory(), pProjectTarget->ex_name);

		ShellExecute(null, "open", src, NULL, NULL, SW_SHOWNORMAL);
	}		

	if (EventsWidget::CheckEvent(227, EventsWidget::menuitem_click))
	{	
		char src[256];		
		String::Printf(src, 256, "%s/Help/editor.docx", en_core.EN_Files()->GetAppDirectory());

		ShellExecute(null, "open", src, NULL, NULL, SW_SHOWNORMAL);
	}
	
	if (EventsWidget::CheckEvent(228, EventsWidget::menuitem_click))
	{	
		char src[256];		
		String::Printf(src, 256, "%s/Help/script_html/html/index.html", en_core.EN_Files()->GetAppDirectory());

		ShellExecute(null, "open", src, NULL, NULL, SW_SHOWNORMAL);
	}

	if (EventsWidget::CheckEvent(805, EventsWidget::menuitem_click))
	{
		pProjectWindow->Show(true);		

		pProjectName->SetText(pProjectTarget->name);
		pProjectButtonPath->SetText(pProjectTarget->icon_path);
		pProjectOrientation->SetCurString(pProject->orintation);	
		pProjectThemeStyle_IOS->SetCurString(pProject->ios_theme);	
		pProjectThemeStyle_DROID->SetCurString(pProject->droid_theme);	
		
		char str[32];
		String::Printf(str,32, "%i", pProjectTarget->ref_height);
		pProjectHeightRef->SetText(str);

		String::Printf(str,32, "%i", pProjectTarget->ref_width);
		pProjectWidthRef->SetText(str);

		pProjectPackegName->SetText(pProject->package_name);
		pProjectVersionName->SetText(pProject->package_version_name);
		pProjectVersionCode->SetText(pProject->package_code_name);

		pProjectBundleName->SetText(pProject->bundle_name);
		pProjectBundleShortName->SetText(pProject->bundle_short_name);
		pProjectBundleVersion->SetText(pProject->bundle_version);		

		pProjectFBid->SetText(pProject->fb_id);
		pProjectVKid->SetText(pProject->vk_id);	

		pProjectNeedIsi->SetChecked(pProject->need_isi);

		pProjectIsWhite_SB->SetChecked(pProject->status_bar_is_white>0);		

		pProjectAddPermissionsModule->SetChecked(pProject->additional_permissions>0);		
		pProjectForceBitmap_IOS->SetChecked(pProjectTarget->force_ios_bitmap>0);

		pProjectHackExtSceneButton->SetText(pProject->hack_ios_ext);

		pProjectDebugInfo->SetCurString(pProjectTarget->debug_info_type);		
	}

	if (pProjectTarget && pProjectTarget->start_scene < (int)pProjectTarget->vScenes.size())
	{
		if (!in_loading_scene)
		{
			if (EventsWidget::CheckEvent(109, EventsWidget::button_click) || core->Controls()->ControlState(DIK_F5)
				|| EventsWidget::CheckEvent(236, EventsWidget::menuitem_click))
			{
				pProjectWindow->Show(false);

                ShowLoggerWindow();

                TLoggerWidgets::Iterator it = g_vLoggerWidgets.Begin();
                while(it != g_vLoggerWidgets.End())
                {
                    (*it)->Clear();
                    ++it;
                }
				
				system_on_pause = false;

				NGUIWidget::focusedWidget = null;
				NGUIWidget::mscover_widget = null;
				NGUIWidget::mscover_widget_not_covered = null;				



				en_core.ResetLogs();

				core->Script()->ReInit();

				en_core.SetUseFakeExit(true);

				if (pCurrentScene)
				{
					pCurrentScene->SetTaskHoldersOwnerActive(false);					
				}

				SaveProject();

				en_core.SetEditorMode(false);				

				if (pSelectedEntity)
                    SelectEntity(NULL);

                InitSimulationWindow();

                NGUIService::instance->ClearWidgetsLists();

				en_core.SetHWND(pGameWindow->GetHandle());				

				pGameWindow->Show(true);

				pGameViewport->SetFocused();
				pGameViewport->Reset();

				pGameViewport->Update();			

				core->Controls()->Release();
				core->Controls()->Init();			

				en_core.EN_Files()->RegFile("editor/system_declations.srt");
				en_core.EN_Scene()->RegisterSceneName("editor/system_declations.srt");

				core->UserStats()->Reset();

				for (size_t i=0;i<pProjectTarget->vScenes.size();i++)
				{					
					en_core.EN_Scene()->RegisterSceneName(pProjectTarget->vScenes[i]->Path());

					if (!pProjectTarget->vScenes[i]->IsScript())
					{
						char name[128];
						String::Copy(name, 128, pProjectTarget->vScenes[i]->Name());
						name[strlen(name) - 3] = 0;

						for (size_t j=0;j<pProjectTarget->vScenes[i]->vIncludes.size();j++)
						{							
							en_core.EN_Scene()->RegisterIncludeInScene(name, pProjectTarget->vScenes[i]->vIncludes[j].c_str());
						}
					}
				}						

                if(pProjectTarget->start_scene != -1)
                {
                    char name[128];
                    String::Copy(name, 128, pProjectTarget->vScenes[pProjectTarget->start_scene]->Name());
                    name[strlen(name) - 3] = 0;				

                    SceneInstance* scene = en_core.EN_Scene()->Load(name);								

                }

				/*for (int i=0;i<project->scenes[project->start_scene].included_scenes.size();i++)
				{
					scene->IncludeScene(project->scenes[project->start_scene].included_scenes[i].data);
				}*/
			}
		}		
	}
	
	if (EventsWidget::CheckEvent(111, EventsWidget::button_click))pGizmo->mode = 0;

	if (EventsWidget::CheckEvent(112, EventsWidget::button_click))pGizmo->mode = 1;

	if (EventsWidget::CheckEvent(113, EventsWidget::button_click))pGizmo->mode = 2;


/*	if (EventsWidget::CheckEvent(114, EventsWidget::combobox_change))
	{
		int index = pButtonGrid->GetCurString();

		switch (index)
		{
			case 0:
				gizmo.grid_align = 0.0f;
			break;
			case 1:
				gizmo.grid_align = 0.1f;
			break;
			case 2:
				gizmo.grid_align = 0.25f;
			break;
			case 3:
				gizmo.grid_align = 0.5f;
			break;
			case 4:
				gizmo.grid_align = 1.0f;
			break;
			case 5:
				gizmo.grid_align = 2.0f;
			break;
			case 6:
				gizmo.grid_align = 3.0f;
			break;
		}
	}*/					
		
	if (pSceneList && EventsWidget::CheckEvent(pSceneList->GetID(), EventsWidget::treelist_click))
    {
	    ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
            OnSceneTreeClick(pScene->SceneTree());
    }

	if (pGuiList && EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::treelist_click))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
            OnSceneTreeClick(pScene->GuiTree());
    }

	if (pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::treelist_click))
    {
        ProjectInfo::Scene* pScene = GetSceneInfo();
        if(pScene)
            OnSceneTreeClick(pScene->SymbolsTree());
    }



	if ((pGuiList && EventsWidget::CheckEvent(pGuiList->GetID(), EventsWidget::treelist_double_click)) || (pGuiSymbolsList && EventsWidget::CheckEvent(pGuiSymbolsList->GetID(), EventsWidget::treelist_double_click)))
	{
		if (pSelectedEntity && String::IsEqual(pSelectedEntity->Desc()->GetName(), "NGUISymbolInstance"))
		{
			NGUISymbolInstance* inst = (NGUISymbolInstance*)pSelectedEntity;

			pGuiSymbolsList->SetFocused();

			SelectEntity(inst->wgt_symbol);
		}
	}
     


	if (pProjectTarget && pProjectTarget->cur_ed_scene != -1 && !pProjectTarget->vScenes[pProjectTarget->cur_ed_scene]->IsScript())
	{
		for (int i=0;i < vEntitiesDeclarations.size();i++)
		{		
			if (EventsWidget::CheckEvent(450 + i, EventsWidget::menuitem_click))
			{
				CreateWidgetEntity(vEntitiesDeclarations[i]->GetName());
				break;
			}				
		}
	
	}	




	if (core->Controls()->ControlState(DIK_LCONTROL, IControlsService::Active) && core->Controls()->ControlState(DIK_R))
	{
		if (pProjectTarget->cur_ed_scene != -1 && !pProjectTarget->vScenes[pProjectTarget->cur_ed_scene]->IsScript())
		{
			if (pProjectTarget->vScenes[pProjectTarget->cur_ed_scene]->Instance())
			{
				char path[512];
				String::Copy(path, 512, pProjectTarget->vScenes[pProjectTarget->cur_ed_scene]->Path());
				String::RemoveExctention(path);
				String::Cat(path, 512, "_copy.isx");
				pProjectTarget->vScenes[pProjectTarget->cur_ed_scene]->Instance()->Save(path, IStreamSaver::xml);			
			}
		}
	}
}

void Editor::ProcessTestsceneGUI()
{
	if (!pGameWindow->IsMinimazed()) 
	{
		pGameViewport->Update();
	}
	
	if (core->Controls()->ControlState(DIK_F9))
	{
		string empty;
		en_core.Render()->MakeScreenshot(empty);
	}	

	if (core->Controls()->ControlState(DIK_LSHIFT, ControlsService::Active) && core->Controls()->ControlState(DIK_F5))
	{
		system_on_pause = !system_on_pause;
		en_core.Mobile()->SetOnPause(system_on_pause);
	}

	/*if (core->Controls()->ControlState(DIK_F5))
	{		
		for (int i=0; i<en_core.EN_Scene()->scenes.size(); i++)
		{
			bool need_delete = true;

			for (int j=0;j<project->scenes.size();j++)
			{
				if (project->scenes[j].scene == en_core.EN_Scene()->scenes[i])
				{
					need_delete = false;
					break;
				}
			}

			if (need_delete)
			{
				en_core.EN_Scene()->scenes[i]->Finish();				
			}
		}

		en_core.OnlineUserStats()->Reset();
		en_core.Ads()->Reset();
		en_core.Mobile()->Reset();
		core->Network()->Reset();

		char name[128];
		String::Copy(name, 128, project->scenes[project->start_scene].name);
		name[strlen(name) - 3] = 0;				

		SceneInstance* scene = en_core.EN_Scene()->Load(name);
	}*/


	if (core->Controls()->ControlState(DIK_F4) || en_core.IsFakeExit() || EventsWidget::CheckEvent(777, EventsWidget::window_close))
	{	
        ProjectInfo::Target* pTarget  = pProject->CurTarget();
		en_core.SetUseFakeExit(false);
		en_core.SetEditorMode(true);		

		en_core.EN_Scene()->UnregisterSceneNames();

        NGUIService::instance->ClearWidgetsLists();
        pTarget->UpdateEngineTopEntities();

		en_core.SetHWND(pViewport->GetHandle());				

		NGUIWidget::focusedWidget = null;
		NGUIWidget::mscover_widget = null;
		NGUIWidget::mscover_widget_not_covered = null;		

		pViewport->SetFocused();
		pViewport->Reset();
					
		if (EventsWidget::CheckEvent(777, EventsWidget::window_close))
		{
            InitSimulationWindow();
		}
		
		pGameWindow->Show(false);				
		
		core->Controls()->Release();
		core->Controls()->Init();		
		for (size_t i=0; i<en_core.EN_Scene()->scenes.size(); i++)
		{
			bool need_delete = true;
            
			for (size_t j=0;j<pTarget->vScenes.size();j++)
			{
				if (pTarget->vScenes[j]->Instance() == en_core.EN_Scene()->scenes[i])
				{
					need_delete = false;
					break;
				}
			}

			if (need_delete)
			{
				en_core.EN_Scene()->scenes[i]->Finish();
			}
		}

		for (size_t i=0; i<en_core.EN_Scene()->scenes.size(); i++)
		{
			bool need_delete = true;
			for (size_t j=0;j<pTarget->vScenes.size();j++)
			{
				if (pTarget->vScenes[j]->Instance() == en_core.EN_Scene()->scenes[i])
				{
					need_delete = false;
					break;
				}
			}

			if (need_delete) en_core.EN_Scene()->Unload(en_core.EN_Scene()->scenes[i]->GetName());				
		}

		if (pCurrentScene)
		{
			if (!pCurrentScene->IsLoading()) pCurrentScene->SetTaskHoldersOwnerActive(true);
		}

		en_core.OnlineUserStats()->Reset();		
		en_core.Mobile()->Reset();
		core->Network()->Reset();		
	}
}

void Editor::MoveImortedSymbolToCurScene(NGUIWidget* widget, bool add_to_scene)
{
	bool is_inst = false;

	if (String::IsEqual(widget->Desc()->GetName(), "NGUISymbolInstance"))
	{
		imported_symbol_names.push_back( ((NGUISymbolInstance*)widget)->symbol_name );
		is_inst = true;
	}

	int count = widget->childs.size();

	int child_inst_count =  (int)widget->childs.size() - (int)widget->childs_names.size();

	for (int i=0; i<count;i++)
	{
		MoveImortedSymbolToCurScene(widget->childs[i], !is_inst || i>child_inst_count);		
	}

	if (add_to_scene)
	{
		pProject->CurScene()->Instance()->AddEntity(widget);
	}
	else
	{
		widget->scene = pProject->CurScene()->Instance();
	}
}

void Editor::MoveImortedSymbolsToCurScene()
{
	int count = pSymbolList->GetSelectedItemsCount();

	for (int i=0; i<count; i++)
	{
		string str;

		pSymbolList->GetSelectedItem(i, str);
		imported_symbol_names.push_back(str);
	}

	for (int i=0; i<imported_symbol_names.size(); i++)
	{
		NGUIWidget* symbol = (NGUIWidget*)imported_scene->Find(imported_symbol_names[i].c_str());

		if (symbol)
		{
			MoveImortedSymbolToCurScene(symbol, true);
		}
	}

	RELEASE(imported_scene)
	imported_symbol_names.clear();	
}

void Editor::SetEdScene(int index)
{	
    ProjectInfo::Target* pTarget  = pProject->CurTarget();

	if (index == -1)
	{
		return;
	}

    if (pTarget->vScenes[index]->IsScript() && pWorkSpaceTabs->GetCurrentTab() == 0)
	{
		pWorkSpaceTabs->SetCurrentTab(1);
	}

	if (index != -1 && pCurrentScene && !pTarget->vScenes[index]->IsScript())
	{
        pTarget->vScenes[index]->Unselect();
        pCurrentScene->SetTaskHoldersOwnerActive(false);
		pScriptEditor->SaveScene(pProject->CurScene());
	}
	pTarget->SelectScene(index);
	if (index != -1)
	{		
		if (!pTarget->vScenes[index]->IsScript())
		{
            ShowScenePanel();
            int old_index =  pScriptEditor->GetDocumentSceneId(0);
			if (old_index != -1)
			{
				pTarget->vScenes[old_index]->Close();
				pTarget->vScenes[old_index]->Unselect();
			}
			if (!pTarget->vScenes[index]->Instance())
			{			
				pTarget->vScenes[index]->SetInstance(en_core.EN_Scene()->Load(pTarget->vScenes[index]->Path()));						
				pTarget->vScenes[index]->Instance()->SetName(pTarget->vScenes[index]->Name());

				for (size_t i=0;i<pTarget->vScenes[index]->vIncludes.size();i++)
				{
					pTarget->vScenes[index]->Instance()->IncludeScene(pTarget->vScenes[index]->vIncludes[i].c_str());
				}
				in_loading_scene = true;
			}
            pScriptEditor->LoadScene(pTarget->vScenes[index]->Path(),pTarget->vScenes[index],!in_loading_scene);
			pCurrentScene = pTarget->vScenes[index]->Instance();
			if (!pCurrentScene->IsLoading()) 
            {
                pCurrentScene->SetTaskHoldersOwnerActive(true);
                UpdateSceneLists();
            }
			SelectEntity(pTarget->vScenes[index]->SelectedEntity());	
		}
        else 
            pScriptEditor->AddScript(pTarget->vScenes[index]->Path(),pTarget->vScenes[index]);
        pTarget->vScenes[index]->Open();
	}
	else
	{		
		pCurrentScene = null;
	}
    
}

void Editor::OpenProject(const char* name)
{
    SaveProject();
    ClearProject();
    AddRecentProject(name);
    ShowProjectPanel();
	ProjectInfo::Target* pTarget = pProject->CurTarget();
	if (pTarget)
		pTarget->SetTreeView(NULL);
    HistoryBuffer::GetInstance()->Clear();
    editor_project.SetProjectPath(name);
	editor_project.OpenProject(editor_project.project, name);

	char ns[512];
	String::ExtractFileName(name, ns);
	String::RemoveExctention(ns);	

	core->UserStats()->SetNamespace(ns);

	en_core.EN_Scene()->SetIsNeedIsi(editor_project.project.need_isi);

	if (pMainWindow)
	{
		char tmp[512];
        ProjectInfo::Target* pTarget = pProject->CurTarget();

		String::Printf(tmp,512,"%s - %s",g_sEditorName.c_str(),pTarget->name);		

		pMainWindow->SetText(tmp);
        pTarget->SetTreeView(pProjectList);
        pTarget->LoadProjectInfo(name,IStreamLoader::xml);

        en_core.EN_Files()->RegFile("editor/system_declations.srt");
        en_core.EN_Scene()->RegisterSceneName("editor/system_declations.srt");
        for (size_t i=0;i<pTarget->vScenes.size();i++)
        {
            en_core.EN_Scene()->RegisterSceneName(pTarget->vScenes[i]->Path());
		}

        char sName[512];

        bool bParseCode =  !core->Script()->LoadParseData(core->GetProjectPath());
        for (size_t i=0;i<pTarget->vScenes.size();i++)
        {
            ProjectInfo::Scene* pScene =  pTarget->vScenes[i];
            if(pScene->IsOpen())
            {
                if(pScene->IsScript())
                    pScriptEditor->AddScript(pScene->Path(),pScene);
                else
                    if (i == pTarget->start_scene)
                    {
                        if(pTarget->cur_ed_scene != i)
                        {
                            pTarget->cur_ed_scene = i;
                            pScriptEditor->LoadScene(pScene->Path(),pScene,false);
                        }
                    }
            }

            sName[0] = 0;
            String::ExtractFileName(pScene->Path(),sName);
            String::RemoveExctention(sName);
            if(!pScene->IsScript() && bParseCode)
            {
                  core->Script()->ParseCode(sName);
                  core->Script()->SaveParseData(core->GetProjectPath());
            }
        }

		SetEdScene(pTarget->cur_ed_scene);
        pScriptEditor->OpenScriptTab(pTarget->cur_ed_file);
	}
}

void Editor::SaveProjectAs()
{
    char name[512];
    name[0] = 0;

    if (DialogSelectFile(false, name, true, "Project file(*.prx)\0*.prx\0"))
    {							
        if (string(name).find(".") == string::npos)
            String::Cat(name, 512, ".prx");
        ProjectInfo::Target* pProjectTarget  = pProject->CurTarget();
        String::ExtractFileName(name, pProjectTarget->name);

        char tmp[512];
        String::Printf(tmp,512,"%s - %s",g_sEditorName.c_str(), pProjectTarget->name);
        pMainWindow->SetText(tmp);			

        editor_project.SetProjectPath(name);

        SaveProject();			

		char ns[512];
		String::ExtractFileName(name, ns);
		String::RemoveExctention(ns);		

		core->UserStats()->SetNamespace(ns);
    }
}

void Editor::SaveDockedWindowsLocation()
{
    string sText;
    
	/*if(pViewPortDockHost && pEditorDockHost)
    {
        pEditorDockHost->DockedHostToString(sText);
        WriteINI("settings.ini", sText.c_str(), "GUI", "editor_windows_location");
        pViewPortDockHost->DockedHostToString(sText);
        WriteINI("settings.ini", sText.c_str(), "GUI", "viewport_windows_location");
        WriteINI("settings.ini",pMainWindow->GetWidth(), "GUI", "width");
        WriteINI("settings.ini",pMainWindow->GetHeight(), "GUI", "height");
    }*/
}

void Editor::SaveProject()
{
    SaveDockedWindowsLocation();
    pScriptEditor->SaveAll();
    core->Script()->SaveParseData(core->GetProjectPath());
	editor_project.SaveProject();

}

void Editor::UpdateSceneLists()
{	
    pViewport->SetView(&view);
    ProjectInfo::Scene* pScene = GetSceneInfo();
    
	if (pScene)
    {
        pScene->SetGuiTreeView(pGuiList);
        pScene->SetSceneTreeView(pSceneList);
        pScene->SetSymbolsTreeView(pGuiSymbolsList);		
    }
    else
    {
        pGuiList->DeleteItems();
        pSceneList->DeleteItems();
        pGuiSymbolsList->DeleteItems();
    }
}

void Editor::SelectEntity(SceneEntity* entity)
{
    if (pSelectedEntity == entity) return;

    ShowObjectPanel();

    if (pSelectedEntity)
    {
        pSelectedEntity->SetEditMode(false);		
        pSelectedEntity->SetGizmo(null);

        pGizmo->enabled = false;
        //sel_entity->ShowWidgets(false);		
        pSelectedEntity->ShowEditorWidgets(false);		

        //pAssetsName->SetText("");
        pEntityName->SetText("");		
        pEntityName->Enable(false);
        pEntityClassName->SetText("");
        pEntityPropertiesScroll->Show(false);
    }

    pSelectedEntity = entity;

    ProjectInfo::Target* pTarget  = pProject->CurTarget();
    pTarget->vScenes[pTarget->cur_ed_scene]->SetSelectedEntity(entity);

    if (pSelectedEntity)
    {		
        ProjectInfo::Scene* pScene =  GetSceneInfo();
        if(pScene)
        {
            ProjectInfo::Entity* pEntity = pScene->Find(pSelectedEntity->GetName());
            if(pEntity)
            {
                pEntity->Select();
                pSelectedObject = pEntity->Handle();
            }
        }

        pEntityName->SetText(pSelectedEntity->GetName());
        if(String::IsEqual(pSelectedEntity->Desc()->GetName(),"NGUISymbolInstance"))
        {
            string sName = string(static_cast<NGUISymbolInstance*>(pSelectedEntity)->symbol_name) + string("Instance");
            pEntityClassName->SetText(sName.c_str());
        }
        else
            pEntityClassName->SetText(pSelectedEntity->Desc()->GetName());


        //pAssetsName->SetText(pSelectedEntity->assetName);

        pSelectedEntity->CreateEditorWidgets(pEntityProperties, pSelectedEntity->Desc()->GetName());		
        pSelectedEntity->SetEditMode(true);	
        pSelectedEntity->SetGizmo(pGizmo);
        pSelectedEntity->DataToGizmo();

        pActiveButton->SetPushed(pSelectedEntity->activeOnStart);
        pVisbleButton->SetPushed(pSelectedEntity->showOnStart);

        bool active = !((SceneInstance*)pSelectedEntity->Scene())->IsIncludedEntity(pSelectedEntity);

        pEntityName->Enable(active);
        //	pActiveButton->Enable(active);
        //	pVisbleButton->Enable(active);
        //	pSelectedEntity->ed_rollout->Enable(active);

        pGizmo->enabled = true;

        UpdateEntitityScroll();		

        if (String::IsEmpty(pSelectedEntity->assetName))
            pSelectedEntity->ShowEditorWidgets(true);
    }
    else 
    {
        pSelectedObject = NULL;
        NGUIService::instance->symbol_widget = NULL;
    }
}

void Editor::CreateWidgetEntity(const char* sName)
{
    if (pProject->CurTarget()->vScenes.size() == 0) return;
    ProjectInfo::Scene* pScene = GetSceneInfo();
    if(pScene)
    {
        if (String::IsSubstr(sName,"NGUI") && !String::IsEqual(sName,"NGUIFont") && !String::IsEqual(sName,"NGUITexture"))
        {
            if(sName == "NGUISymbol")
            {
                CreateWidgetEntity(sName, pScene->SymbolsTree(),NULL);
            }
            else
            {
                if(pSelectedObject)
                {
                    ProjectInfo::ISceneTree* pTree = NULL;
                    if(pSelectedObject == pGuiList->GetSelectedItem())
                        pTree = pScene->GuiTree();
                    else
                        if(pSelectedObject == pGuiSymbolsList->GetSelectedItem())
                        {
                            void* pIter = pGuiSymbolsList->GetParent(pSelectedObject);
                            while(pIter && pGuiSymbolsList->IsFolder(pIter))
                            {
                                pIter = pGuiSymbolsList->GetParent(pIter);
                            }
                            if(pIter)
                                pTree = pScene->SymbolsTree();
                        }
                        if(pTree)
                            CreateWidgetEntity(sName,pTree,pSelectedObject);
                }
                else
                    CreateWidgetEntity(sName,pScene->GuiTree(),NULL,NULL,NGUIService::instance->TopWidgetsCount());
            }
        }
        else
            CreateWidgetEntity(sName,pScene->SceneTree(),pSelectedObject);
    }
}

void  Editor::CreateWidgetEntity(const char* sName, ProjectInfo::ISceneTree* pTree,void* pHandel,SceneEntity* pDesc,int weight)
{
    if (pProject->CurTarget()->vScenes.size() == 0) return;
    ProjectInfo::Scene* pScene = GetSceneInfo();
    if(pScene)
    {
        Matrix trans;
        trans.Pos() = view.Pos() + view.Vz() * 5.0f;
        ProjectInfo::Entity* pEntity = pScene->CreateEntity(sName,trans,pDesc);
        if(pEntity)
        {
            int nWeigth = -1;
            if(pHandel)
            {
                const char* sName = pTree->View()->GetItemText(pHandel);
                if(!String::IsEqual(sName,"childs"))
                {
                    ProjectInfo::Entity* pNeighbor = pTree->GetEntity(pHandel);
                    if(pNeighbor)
                    {
                        nWeigth = pNeighbor->Weight() + 1;
                        if(pNeighbor->Parent())
                            pHandel = pNeighbor->Parent()->Handle();
                        else
                            pHandel = NULL;
                    }
                }
            }
            pTree->InsertEntity(pHandel,pEntity,nWeigth);
            SelectEntity(pEntity->GetEngineEntity());
        }
    }
}

void Editor::CopySelectedEntity()
{	
	if (!pSelectedEntity) return;
    ProjectInfo::Scene*  pScene = GetSceneInfo();
    if(pScene)
    {
      ProjectInfo::Entity*  pEntity = pScene->Find(pSelectedEntity->GetName());
      if(pEntity && pEntity->Tree() && pEntity->Handle())
      {
          ProjectInfo::Entity* pCopy = pEntity->Tree()->CopyEntity(pEntity->Handle());
          if(pCopy)
            SelectEntity(pCopy->GetEngineEntity());
      }
    }
}

void Editor::OnSceneTreeClick(ProjectInfo::ISceneTree* pTree)
{
    pSelectedObject = pTree->View()->GetSelectedItem();
    ProjectInfo::Entity* pEntity = pTree->GetEntity(pSelectedObject);
    if(pEntity)
        SelectEntity(pEntity->GetEngineEntity());
    else
    {
        SelectEntity(NULL);
        if(pSelectedObject && pTree->IsFolder(pSelectedObject))
        {
            pEntityName->SetText(pSceneList->GetItemText(pSelectedObject));		
            pEntityName->Enable(true);
        }
    }
}

void Editor::ExportResources(Platform platform, bool gen_package)
{
	if (pExportProgressWindow) pExportProgressWindow->Show(true);

	editor_project.ExportResources(platform, gen_package);

	if (pExportProgressWindow) pExportProgressWindow->Show(false);
}

ProjectInfo::Scene* Editor::GetSceneInfo(const char* sName)
{
    string sBuff;
    ProjectInfo::Target* pTarget = pProject->CurTarget();
    if(pTarget)
    {
        if(sName)
        {
            return pTarget->Find(sName);
        }
        else
        {
            if( pTarget->cur_ed_scene >= 0)
                return pTarget->vScenes[pTarget->cur_ed_scene];
        }

    }
    return NULL;
}



void Editor::ReloadScene(ProjectInfo::Scene* pScene,bool bClear /* = false */)
{
	SelectEntity(null);
    pScene->ReLoadInstance(bClear);
}

void Editor::MakeSelection(int mx, int my)
{
	if (!core->Controls()->ControlState(DIK_LSHIFT, ControlsService::Active)) return;

	SceneEntity* entity = null;

	if (NGUIWidget::mscover_widget)
	{
		entity = NGUIWidget::mscover_widget;
	}	

	SelectEntity(entity);	
}

void Editor::UpdateEntitityScroll()
{
	if (!pSelectedEntity) return;			
	{
		int las_pos = 0;
	
		float sz = pSelectedEntity->ed_rollout->GetHeight();

		if (sz > pEntityPropertiesScroll->GetHeight())
		{
			pEntityPropertiesScroll->Show(true);
			float pg_sz = ((pEntityPropertiesScroll->GetHeight() - 20) / sz) * (pEntityPropertiesScroll->GetHeight() - 20);
			
			pEntityPropertiesScroll->SetLimit(50,(int)(sz - pEntityPropertiesScroll->GetHeight() + 20) );
		}
		else
		{
			pEntityPropertiesScroll->Show(false);
		}
	}

	if (EventsWidget::CheckEvent(pEntityPropertiesScroll->GetID(), EventsWidget::scrollbar_change))
	{		
		pSelectedEntity->ed_rollout->SetPos(pSelectedEntity->ed_rollout->GetX(),(float)-pEntityPropertiesScroll->GetPosition());		
	}
		
}

void Editor::ProcessMessageLoop()
{
	MSG msg;

	while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	} 
}

void Editor::SetProgressExportText( const char* format, ... )
{		
	char buf[1024];

	buf[0] = 0;

	va_list	args;

	va_start( args, format );
	vsnprintf( buf, sizeof( buf ), format, args );	

	if (!pExportProgressWindow)
	{
		core->TraceTo("Export", buf);
	}
	else
	{
		pExportProgressLabel->SetText(buf);
		ProcessMessageLoop();
	}
}

void Editor::ScanDir(const char* sPath,const char* sFileType,std::vector<string>& vFiles)
{
    char cur_dir[521];

#ifdef PC
    String::Printf(cur_dir, 512, "%s/*.%s", sPath, sFileType);
    WIN32_FIND_DATA data;
	HANDLE h = FindFirstFile(cur_dir, &data);
	if( h!=INVALID_HANDLE_VALUE ) 
	{
		do
		{
			if (!String::IsEqual(data.cFileName, ".") && !String::IsEqual(data.cFileName, ".."))
			{
				if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    vFiles.push_back(string(sPath) +"/"+string(data.cFileName));
			}			
		}
		while(FindNextFile(h,&data));
	} 
	FindClose(h);
#endif

#ifdef OSUNIX
    String::Printf(cur_dir, 512, "%s",sPath);
	struct dirent *de=NULL;
	DIR *d=opendir(cur_dir);
	if(d == NULL)return;
	while(de = readdir(d))
	{
		if (!String::IsEqual(".", de->d_name) && !String::IsEqual("..", de->d_name))
		{		
			if (de->d_type != DT_DIR)
            {   
                char ext[10];
			    String::ExtractExctention(de->d_name,ext,10);
                if(String::IsEqual(ext,sFileType))
                    vFiles.push_back(string(sPath) +"/"+string(de->d_name));
            }
		}
	}
	closedir(d);
#endif
}

void Editor::GetScriptEditorColorSchems()
{
    vScriptEditorColorSchemsNames.clear();
    ScriptDocumentWidget::ColorScheme default;
    default.background = RGB(0xFF, 0xFF, 0xFF);
    default.text = RGB(0x00, 0x00, 0x00);
    default.caret = RGB(0x00, 0x00, 0x00);
    default.number = RGB(0x00, 0x00, 0x00);
    default.preprocessor = RGB(0x34, 0x00, 0xff);
    default.preprocessor_comment = RGB(0x34, 0x00, 0xff);
    default.operators = RGB(0x00, 0x00, 0x00);
    default.keywords =  RGB(0x34, 0x00, 0xff);
    default.types = RGB(0x34, 0x00, 0xff);
    default.comment = RGB(0x00, 0x80, 0x00);
    default.strings = RGB(0x88, 0x00, 0x00);
    default.chars = RGB(0x88, 0x00, 0x00);
    default.search_selection_text =  RGB(0,0,0); 
    default.search_selection_back =  RGB(255,255,0);

    vScriptEditorColorSchems["default"] = default;
    vScriptEditorColorSchemsNames.push_back("default");
    std::vector<string> vFiles;
    ScanDir("Editor/ScriptEditorColorSchems","sch",vFiles);
    if(vFiles.size() > 0)
    {
        for(unsigned i =0;i < vFiles.size();++i)
        {
            IStreamLoader* loader = core->Files()->CreateStreamLoader(vFiles[i].c_str(), "", IStreamLoader::json);	
            if (loader)
            {
                ScriptDocumentWidget::ColorScheme scheme;
                scheme.background = GetScriptEditorColor(loader,"background",default.background);
                scheme.text = GetScriptEditorColor(loader,"text",default.text);
                scheme.number = GetScriptEditorColor(loader,"number",default.number);
                scheme.caret = GetScriptEditorColor(loader,"caret",default.caret);
                scheme.preprocessor = GetScriptEditorColor(loader,"preprocessor",default.preprocessor);
                scheme.preprocessor_comment = GetScriptEditorColor(loader,"preprocessor_comment",default.preprocessor_comment);
                scheme.operators = GetScriptEditorColor(loader,"operators",default.operators);
                scheme.keywords = GetScriptEditorColor(loader,"keywords",default.keywords);
                scheme.types = GetScriptEditorColor(loader,"types",default.types);
                scheme.comment = GetScriptEditorColor(loader,"comment",default.comment);
                scheme.strings = GetScriptEditorColor(loader,"strings",default.strings);
                scheme.chars = GetScriptEditorColor(loader,"chars",default.chars);
                scheme.search_selection_text = GetScriptEditorColor(loader,"search_selection_text",default.search_selection_text);
                scheme.search_selection_back = GetScriptEditorColor(loader,"search_selection_back",default.search_selection_back);
                char sKey[128];
                String::ExtractFileName(vFiles[i].c_str(),sKey);
                String::RemoveExctention(sKey);
                vScriptEditorColorSchems[sKey] = scheme;
                vScriptEditorColorSchemsNames.push_back(sKey);
                loader->Release();
            }
        }

    }
}

int  Editor::GetScriptEditorColor(IStreamLoader* loader,const char* sName,int default)
{
    if (loader->EnterBlock("palette"))
    {
        if(loader->EnterBlock(sName))
        {
            int r,g,b;
            loader->Read("r",r);
            loader->Read("r",g);
            loader->Read("r",b);
            loader->LeaveBlock();
            loader->LeaveBlock();
            return RGB(r,g,b);
        }
        loader->LeaveBlock();
    }
    return default;
}

void Editor::DeleteProjectObject(void* pHandle)
{
    if (pHandle)
    {							
        if (pProjectList->IsFolder(pHandle))
        {
            void* pChild = pProjectList->GetFirstItem(pHandle);
            while(pChild != NULL)
            {
                void* pIter = pChild; 
                pChild = pProjectList->GetNextItem(pChild);
                DeleteProjectObject(pIter);
            }
            pProjectList->DeleteItem(pHandle);
        }
        else
        {
            ProjectInfo::Scene* pScene = static_cast<ProjectInfo::Scene*>(pProjectList->GetHandleData(pProjectList->GetPopUpItem()));
            if(pScene)
                DeleteScene(pScene->Index());
        }
    }
}

void Editor::DeleteScene(int nIndex)
{
    if (nIndex != -1)
    {
        ProjectInfo::Target* pTarget  = pProject->CurTarget();
        if (!pTarget->vScenes[nIndex]->IsScript())
        {
            en_core.EN_Scene()->Unload(pTarget->vScenes[nIndex]->Name());		
            if(pTarget->cur_ed_scene == nIndex)
            {
                pTarget->cur_ed_scene = -1;
                pCurrentScene = null;
                SelectEntity(null);
                pSceneList->DeleteItems();
                pGuiList->DeleteItems();
                pGuiSymbolsList->DeleteItems();
            }
        }
        pScriptEditor->CloseScript(pTarget->vScenes[nIndex]);
        pTarget->DeleteScene(nIndex);
    }
}

void Editor::SaveSceneAs(ProjectInfo::Scene* pSceneSrc,ProjectInfo::Scene* pNewScene,const char* sName,char* sPath)
{
    if (string(sPath).find(".") == string::npos)
        String::Cat(sPath, 512, ".isx");			

    en_core.EN_Files()->RelativePath(sPath);
    String::FixSlashes(sPath);

    pNewScene->SetName(sName);
    pNewScene->SetPath(sPath);
    
    pSceneSrc->Instance()->Save(pNewScene->Path(), IStreamSaver::xml);					
    pScriptEditor->SaveScene(pNewScene->Path());
}

void Editor::SaveSceneAs(ProjectInfo::Scene* pSceneSrc)
{
    char sPath[512];
    sPath[0] = 0;
    if (DialogSelectFile(false, sPath, true, "Scene file(*.isx)\0*.isx\0"))
    {	
        char sName[64];
        String::ExtractFileName(sPath, sName);
        String::RemoveExctention(sName);
        String::Cat(sName, 64, "_ed");
        if(!IsSceneExists(sName))
        {
            SaveSceneAs(pSceneSrc,pSceneSrc,sName,sPath);
            pScriptEditor->SetSceneName(sPath);
        }

    }
}

void Editor::SaveSceneAsAndDuplicate(ProjectInfo::Scene* pSceneSrc)
{
    char sPath[512];
    sPath[0] = 0;

    if (DialogSelectFile(false, sPath, true, "Scene file(*.isx)\0*.isx\0"))
    {	
        char sName[64];
        String::ExtractFileName(sPath, sName);
        String::RemoveExctention(sName);
        String::Cat(sName, 64, "_ed");
        if(!IsSceneExists(sName))
        {
            ProjectInfo::Target* pTarget  = pProject->CurTarget();
            ProjectInfo::Scene* pScene = pTarget->AddScene(sName,pSceneSrc->IsScript());
            SaveSceneAs(pSceneSrc,pScene,sName,sPath);
            SetEdScene(pScene->Index());
        }
    }
}

void Editor::AddNewScene(bool bScript,void* pParent)
{
    char name[512],sFiles[64];
    sFiles[0] = name[0] = 0;
    string sExt;
    if(bScript)
    {
        String::Copy(sFiles,64,"Script file(*.srt)\0");
        int offset  = strlen(sFiles) + 1;
        memcpy_s(sFiles + offset,64 - offset ,"*.srt\0",64 - offset);
        sExt = ".srt";
    }
    else
    {
        String::Copy(sFiles,64,"Scene file(*.isx)\0");
        int offset  = strlen(sFiles) + 1;
        memcpy_s(sFiles + offset,64 - offset ,"*.isx\0",64 - offset);
        sExt = ".isx";
    }
    if (DialogSelectFile(false, name, true,sFiles))
    {
        char scene_path[512];
        String::Copy(scene_path, 512, name);

        en_core.EN_Files()->RelativePath(scene_path);
        String::FixSlashes(scene_path);
        int len = strlen(scene_path);
        if (len > 4)
        {
            if (editor_project.project_path[len - 4] != '.')
                String::Cat(scene_path, 512,sExt.c_str());
        }
        else
            String::Cat(scene_path, 512,sExt.c_str());

        String::ExtractFileName(scene_path, name);
        String::RemoveExctention(name);
        String::Cat(name, 512, "_ed");

        if (!IsSceneExists(name))
        {
            ProjectInfo::Target* pTarget  = pProject->CurTarget();
            ProjectInfo::Scene* scene_src = pTarget->AddScene(name,bScript,pParent);
            scene_src->SetPath(scene_path);

            if(!bScript)
            {
                scene_src->Load(scene_path,IStreamLoader::FileType::xml);
            }

            SetEdScene(scene_src->Index());
            pScriptEditor->SaveCurrentDocument();
            editor_project.SaveProject();
        }
    }
}

void Editor::AddExistingScene(bool bScript,void* pParent)
{
    char name[512],sFiles[64];
    sFiles[0] = name[0] = 0;
    if(bScript)
    {
        String::Copy(sFiles,64,"Script file(*.srt)\0");
        int offset  = strlen(sFiles) + 1;
        memcpy_s(sFiles + offset,64 - offset ,"*.srt\0",64 - offset);
    }
    else
    {
        String::Copy(sFiles,64,"Scene file(*.isx)\0");
        int offset  = strlen(sFiles) + 1;
        memcpy_s(sFiles + offset,64 - offset ,"*.isx\0",64 - offset);
    }

    if (DialogSelectFile(true, name, true, sFiles))
    {
        char tmp[64];
        String::ExtractFileName(name, tmp);
        String::RemoveExctention(tmp);
        String::Cat(tmp, 64, "_ed");
        en_core.EN_Files()->RelativePath(name);
        String::FixSlashes(name);
        if (!IsSceneExists(tmp))
        {
            ProjectInfo::Target* pTarget  = pProject->CurTarget();
            ProjectInfo::Scene* scene_src = pTarget->AddScene(tmp,bScript,pParent);
            scene_src->SetPath((const char*)name);
			en_core.EN_Scene()->RegisterSceneName(name);
            if(!bScript)
            {
                scene_src->Load(name,IStreamLoader::FileType::xml);
                SetEdScene(scene_src->Index());
            }
			else
				core->Script()->AddCodeToNameSpaces(scene_src->ScreenName());
            editor_project.SaveProject();
        }
    }
}

bool Editor::IsSceneExists(const char* sName)
{
    ProjectInfo::Target* pTarget  = pProject->CurTarget();
    if(pTarget)
        return pTarget->Find(sName) != NULL;
    return false;
}

void Editor::OnSelectSceneTab(int sceneId)
{
   if(pProject->CurTarget())
      pProject->CurTarget()->SelectScene(sceneId);
}

void Editor::SetStatusText(const char* format, ...)
{
    char buf[8192];
    buf[0] = 0;
    va_list	args;
    va_start( args, format );
    vsnprintf(buf, 8192, format, args );	
    va_end( args );
    pStatus->SetText(buf);
#ifdef LOG_SCRIPT_PARSER
    core->TraceTo("Script","%s",buf);
#endif

}
void Editor::OpenScriptFile(const string& sFile)
{
	string sPath = sFile;
	bool bAbsalutPath = sPath.find(':') != string::npos;
	if(!bAbsalutPath)
		bAbsalutPath = String::GetAbsalutPath(sPath,core->GetProjectPath());
	if(bAbsalutPath)
	{
		if(sPath.size() > 4 && sPath[sPath.size() - 4] != '.')
			sPath += ".srt";
		if(pWorkSpaceTabs->GetCurrentTab() == 0)pWorkSpaceTabs->SetCurrentTab(1);
		pScriptEditor->AddScript(sPath.c_str(),NULL);
	}
}
void Editor::GoToScript(const string& sFile,int nPos)
{
   OpenScriptFile(sFile);
   pScriptEditor->GoToLine(-1,nPos);
}
void Editor::GoToScript(const string& sFile,int nLine,int nCol)
{
	OpenScriptFile(sFile);
	pScriptEditor->GoTo(nLine,nCol);
}
void Editor::GetDevicesList()
{
    NetLogger* pNetLogger =  en_core.GetNetLogger();
    if(pNetLogger)
    {
        std::vector<string> vList;
        pNetLogger->GetDevicesNames(vList); 
        string sSelected;

        pDevicesList->GetSelectedItem(0, sSelected);
        pDevicesList->ClearList();
        
		for (unsigned i = 0;i < vList.size();++i)
		{
            pDevicesList->AddString(vList[i].c_str(), 0);
		}
        
		if (!sSelected.empty())
		{
            pDevicesList->SelectText(sSelected);
		}
    }
}

void Editor::DevicesListSelectedConnected(bool bFlag)
{
    if(bFlag)
    {
        pDevicesConnectButton->SetText("Disconnect");
        pDevicesConnectButton->SetFlag(1);
         pDevicesSendMessage->SetText("");
    }
    else
    {
        pDevicesConnectButton->SetText("Connect");
        pDevicesConnectButton->SetFlag(0);
    }
    pDevicesSendMessage->Show(bFlag);
    pDevicesSendButton->Show(bFlag);
}

void Editor::GetSignatureInScene(const std::vector<string>& vPath,std::vector<string>& vSignatures)
{
    ProjectInfo::Target* pTarget = pProject->CurTarget();
    if(pTarget)
    {
        ProjectInfo::Scene* pScene =  pTarget->Find(vPath[0].c_str());
        if(pScene)
            pScene->GetSignatureInScene(vPath,vSignatures);
        else 
            core->Script()->Machine()->GetSignature(vPath,vSignatures);
    }
}

void Editor::GetMembersInScene(const std::vector<string>& vPath,std::vector<string>& vMembers)
{
    ProjectInfo::Target* pTarget = pProject->CurTarget();
    if(pTarget)
    {
        ProjectInfo::Scene* pScene =  pTarget->Find(vPath[0].c_str());
        if(pScene)
            pScene->GetMembersInScene(vPath,vMembers);
        else
            core->Script()->Machine()->GetMembers(vPath,vMembers);
    }
}

void Editor::GetMembers(int nLine,const char* sFileName,std::vector<string>& vPath,std::vector<string>& vMembers,bool bNameSpace)
{
    vMembers.clear();
    if(!vPath.empty())
    { 
        ScriptCodeParserData::ScriptNameSpace* pSpace = core->Script()->GetNameSpace(nLine,sFileName,vPath,bNameSpace);
        if(pSpace)
        {
            if(String::IsEqual(sFileName,pSpace->File().c_str()))
                pSpace->GetMembers(vMembers,nLine,true);
            else
                pSpace->GetMembers(vMembers,-1,true);
            if(bNameSpace)
                GetMembersInScene(vPath,vMembers);
            else
                core->Script()->Machine()->GetMembers(vPath,vMembers);
        }
        else
        {
            if(bNameSpace)
                GetMembersInScene(vPath,vMembers);
            else
                core->Script()->Machine()->GetMembers(vPath,vMembers);

        }
    }
}

void Editor::GetSignature(int nLine,const char* sFileName,std::vector<string>& vPath,std::vector<string>& vSignatures,bool bNameSpace)
{
    if(!vPath.empty())
    {
        ScriptCodeParserData::ScriptNameSpace* pSpace = core->Script()->GetNameSpace(nLine,sFileName,vPath,bNameSpace);
        if(pSpace)
        {
            if(String::IsEqual(sFileName,pSpace->File().c_str()))
                pSpace->GetSignatures(vSignatures,nLine,vPath.back(),true);
            else
                pSpace->GetSignatures(vSignatures,-1,vPath.back(),true);
            if(bNameSpace)
                GetSignatureInScene(vPath,vSignatures);
            else
                core->Script()->Machine()->GetSignature(vPath,vSignatures);
        }
        else
        {
            if(bNameSpace)
                GetSignatureInScene(vPath,vSignatures);
            else
                core->Script()->Machine()->GetSignature(vPath,vSignatures);
        }
    }
}

void Editor::GetResolutionForSimulation(int& w,int& h,ProjectInfo::EditorProject::ProjectInfo* pProject)
{
    w = pSimulationOptions->GetCurrentWidth();
    h = pSimulationOptions->GetCurrentHeight();

    if ((pProject->orintation == 0 && w < h) || (pProject->orintation == 1 && w > h))
    {
        h = pSimulationOptions->GetCurrentWidth();;
        w = pSimulationOptions->GetCurrentHeight();
    }		
}

void Editor::InitSimulationWindow()
{
    if(pGameViewport)
        delete pGameViewport;
    if(pGameWindow)
        delete pGameWindow;

    int w,h;
    GetResolutionForSimulation(w,h,pProject);
	
	string text = string("Runing - ") + string(pProject->CurTarget()->name);
    pGameWindow = new WindowWidget(777, text.c_str(), false, true, 100, 100, w, h);

    pGameWindow->SetNeedStrictSize(true);

    pGameViewport = new EditorViewPort(VIEW_PORT_ID, pGameWindow, "", 0, 0, w, h);
    pGameViewport->core_ed = this;
    pGameViewport->SetNeedShowActiveText(false);
}

#ifdef UNSTABLE
std::string Editor::g_sEditorName = "ION Unstable";
#else
std::string Editor::g_sEditorName = "";
#endif

#endif





