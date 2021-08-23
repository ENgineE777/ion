
#pragma once

#ifdef EDITOR

#include "Platforms/pc/gui/Widgets.h"
#include "Platforms/pc/EditorWindow.h"
#include "Platforms/PC/GUI/TextEditorWidget.h"
#include "Core/ICore.h"
#include "Core/Scene/SceneInstance.h"
#include "Core/Files/FileService.h"
#include "Editor/Helpers/Gizmo.h"
#include "Editor/Helpers/EditorProject.h"
#include "Helpers/SceneInfo.h"
#include "Helpers/SimulationOtions.h"
#include "Helpers/TextListWidget.h"

#include <algorithm>

class EditorViewPort;
class TextureInstance;
class NGUIWidget;
class NGUISymbol;

class DEF_EXPORT Editor
{
public:
    struct MenuDescriptor
    {
        enum ECrearteMenuCommand{eAddItem,eAddSeparator,eBeginSubMenu,eEndSubMenu,eRunCreator};
        typedef void (Editor::*CreatorFunc) (WindowMenuWidget* pParent);

        ECrearteMenuCommand eCommand;
        char                sName[64];
        int                 nId; 
        unsigned            nFlag;   
        CreatorFunc         pCreator;
    };
    void  CreateEntitiesMenu(WindowMenuWidget* pParent);
    void  ScriptEditorColorSchemesMenu(WindowMenuWidget* pParent);
    void  CreateRecentOpenProjects(WindowMenuWidget* pParent);
    void  CreateRecentOpenFiles(WindowMenuWidget* pParent);
private:
    std::vector<EntityDedeclarator*> vEntitiesDeclarations;

   typedef std::list<string> TRecentPaths;

   DockedPanelWidget*       pEditorDockHost;
   DockedPanelWidget*       pViewPortDockHost;
   TDockedWidgets           vDockedWidgets;

	Gizmo*                  pGizmo;
	SceneEntity*            pSelectedEntity;
	void*                   pSelectedObject;

	bool system_on_pause;

	Matrix view;

    void GetScriptEditorColorSchems();
    int  GetScriptEditorColor(IStreamLoader* loader,const char* sName,int default);
    void ParseRecentPaths(const string& sBuffer,TRecentPaths& vList);
    void AddRecentPath(const string& sPath,TRecentPaths& vList,const string& sType);
    void AddRecentProject(const string& sPath);
    void CreateRecentPathsMenu(const TRecentPaths& vPaths,int nBaseId,WindowMenuWidget* pMenu,HMENU* pHandle = NULL);

    typedef HashTable<string,ScriptDocumentWidget::ColorScheme>  TScriptEditorColorSchems;
    TScriptEditorColorSchems  vScriptEditorColorSchems;
    std::vector<string>       vScriptEditorColorSchemsNames;
    TRecentPaths              vRecentOpenProjects;
    TRecentPaths              vRecentOpenFiles;
    WindowMenuWidget*         pMainMenu;
    int                       nSelectedScriptEditorColorScheme;
    string                    sScriptEditorInitedColorScheme;

	WindowWidget* pGameWindow;

    TabSheetWidget* pWorkSpaceTabs;

	WindowWidget* pExportProgressWindow;
	LabelWidget*  pExportProgressLabel;

    TabSheetWidget* pProjectTabs;
	TreeViewWidget* pProjectList;

	TreeViewWidget* pSceneList;
	TreeViewWidget* pGuiList;
	TreeViewWidget* pGuiSymbolsList;	


	TabSheetWidget* pScriptTabs;

	TextBoxWidget* pEntityName;
	TextBoxWidget* pEntityClassName;
	SizerWidget*   pEntityProperties;	


	ScrollBarWidget* pEntityPropertiesScroll;

	LabelWidget* pStatusEditLine;
	LabelWidget* pStatusEditCol;
    LabelWidget* pStatus;

	TextEditorWidget* pScriptEditor;

	TextBoxWidget* pAssetsName;

	ButtonWidget* pActiveButton;
	ButtonWidget* pVisbleButton;

	ComboBoxWidget* pButtonGrid;

	EditorViewPort* pViewport;	
	EditorViewPort* pGameViewport;

	WindowWidget*   pSymbolImport;
	ListBoxWidget*  pSymbolList;

	WindowWidget*   pProjectWindow;
	
	
	TextBoxWidget*  pProjectName;	
	ButtonWidget*   pProjectButtonPath;	
	ComboBoxWidget* pProjectOrientation;	
	ComboBoxWidget* pProjectThemeStyle_IOS;	
	CheckBoxWidget* pProjectIsWhite_SB;
	ComboBoxWidget* pProjectThemeStyle_DROID;	

	TextBoxWidget* pProjectWidthRef;
	TextBoxWidget* pProjectHeightRef;

	TextBoxWidget* pProjectPackegName;
	TextBoxWidget* pProjectVersionName;
	TextBoxWidget* pProjectVersionCode;
	
	TextBoxWidget* pProjectBundleName;
	TextBoxWidget* pProjectBundleShortName;	
	TextBoxWidget* pProjectBundleVersion;

	TextBoxWidget* pProjectFBid;
	TextBoxWidget* pProjectVKid;			

	CheckBoxWidget* pProjectNeedIsi;

	TextBoxWidget* pProjectGoogleAnlt_Droid;
	TextBoxWidget* pProjectGoogleAnlt;			
	TextBoxWidget* pProjectFlurry_Droid;
	TextBoxWidget* pProjectFlurry_IOS;				

	CheckBoxWidget* pProjectAddPermissionsModule;			

	CheckBoxWidget* pProjectForceBitmap_IOS;	

	ComboBoxWidget* pProjectDebugInfo;

	ButtonWidget*   pProjectHackExtSceneButton;

	TabSheetWidget*		pFindResultsTabs;
	TextListWidget 	    rFindResultsWidget;

	WindowWidget*   pSceneIncludeWindow;
    WindowWidget*   pDevicesWindow;
    ListBoxWidget*  pDevicesList;
    ButtonWidget*   pDevicesConnectButton;
    ButtonWidget*   pDevicesSendButton;
    TextBoxWidget*  pDevicesSendMessage;
    float           nDevicesWindowRefreshTimer;

    SimulationOtions* pSimulationOptions; 

    void            DevicesListSelectedConnected(bool bFlag);

	int last_ed_line;
	int last_ed_col;

	bool in_canvas_drag;
	int prev_mx;
	int prev_my;
	
	ProjectInfo::EditorProject::ProjectInfo*  pProject;

	bool		   in_loading_scene;
	SceneInstance* pCurrentScene;	

	SceneInstance* imported_scene;	
	vector<string> imported_symbol_names;

	static int main_thread_id;
		
	class ProgressExportCallback : public ProjectInfo::EditorProject::IProgressExportCallback
	{
	public:
		Editor* pOwner;
		virtual void SetText(const char *text);
	};

	ProgressExportCallback progress_callback;


    static TabSheetWidget*                 g_pLogTabs;


    static  TLoggerWidgets g_vLoggerWidgets;

    static  string         g_sEditorName;

public:

	ProjectInfo::EditorProject editor_project;
    EditorWindow* pMainWindow;

    void DeleteProjectObject(void* pHandle);
    void DeleteScene(int nIndex);
    void SaveSceneAs(ProjectInfo::Scene* pSceneSrc);
    void SaveSceneAsAndDuplicate(ProjectInfo::Scene* pSceneSrc);
    void SaveSceneAs(ProjectInfo::Scene* pSceneSrc,ProjectInfo::Scene* pNewScene,const char* sName,char* sPath);
    void AddNewScene(bool bScript,void* pParent = NULL);
    void AddExistingScene(bool bScript,void* pParent = NULL);


    void GetDevicesList(); 

	void InitGUI();

    void ShowLoggerWindow(bool bCreate = false);
	void ShowFindResultsWindow(bool bCreate = false);
    void ShowProjectPanel(bool bCreate = false);
    void ShowObjectPanel(bool bCreate = false);
    void ShowScenePanel(bool bCreate = false);

    void CreateControlBar(SizerWidget* pSizer);
    void CreateStatusBar(SizerWidget* pSizer);
    void CreateWorkSpace(DockedWidget* pParent);
    void CreateMenu(WindowMenuWidget* pParent,Editor::MenuDescriptor* vMenuDeclarator,unsigned nCount);

	void CreateSymbolImport();
	void CreateSettingsWindow();
    void CreateDevicesWindow();
	
	void OpenProject(const char* name);    
	void SaveProject();
    void SaveProjectAs();




    void CreateWidgetEntity(const char* sName);
    void CreateWidgetEntity(const char* sName, ProjectInfo::ISceneTree* pTree,void* pPHandel,SceneEntity* pDesc = NULL,int weight = 0);
    void SelectEntity(SceneEntity* entity);
	void CopySelectedEntity();


	void ExportResources(Platform platform, bool gen_package);

	void UpdateSceneLists();
	void ReloadScene(ProjectInfo::Scene* pScene,bool bClear = false);
	void ReloadSceneNodes();



	void ClearProject();


	void UpdateLoadingElems();
	void ProcessEditorControls();
	void ProcessEditorGUI();
	void ProcessTestsceneGUI();

	void SetEdScene(int index);
	
	void UpdateEntitityScroll();

	void ProcessMessageLoop();
	void SetProgressExportText( const char* format, ... );

	
	void ShowFindResults(TMatchesMap& vMatshes,const char* sSerchText);


    void  ScanDir(const char* sPath,const char* sFileType,std::vector<string>& vFiles);


    void  GetMembersInScene(const std::vector<string>& vPath,std::vector<string>& vMembers);
    void  GetSignatureInScene(const std::vector<string>& vPath,std::vector<string>& vSignatures);


    bool  IsSceneExists(const char* sName);

    static void ConverterProgress(const char* file_name, int current, int total);
    static void TraceCallback(const char* log, const char* text);

    void GetResolutionForSimulation(int& w,int& h,ProjectInfo::EditorProject::ProjectInfo*  pProject);
    void InitSimulationWindow();

	void OpenScriptFile(const string& sFile);

public:	

	Editor();
    ~Editor();

    bool            Run(HINSTANCE instance, int nArgs, char** args);

    SceneInstance*  GetCurrentScene(){return pCurrentScene;}
    TreeViewWidget* GetSymbolList(){return pGuiSymbolsList;}
    void            GetMembers(int nLine,const char* sFileName,std::vector<string>& vPath,std::vector<string>& vMembers,bool bNameSpace);
    void            GetSignature(int nLine,const char* sFileName,std::vector<string>& vPath,std::vector<string>& vSignatures,bool bNameSpace);
    void            OnSelectSceneTab(int sceneId);
    void            MakeSelection(int mx, int my);
    void            SetStatusText(const char* format, ...);
    ProjectInfo::Scene*  FindSceneSource(const char* sName);
    void            GoToScript(const string& sFile,int nPos);
	void            GoToScript(const string& sFile,int nLine,int nCol);

    void            OnDockedWindowDestroy(const string& sName);
    void            SaveDockedWindowsLocation();
    void            OnSceneTreeClick(ProjectInfo::ISceneTree* pTree);
    void            AddRecentFile(const string& sPath);
  	
	void				MoveImortedSymbolToCurScene(NGUIWidget* widget, bool add_to_scene);
	void				MoveImortedSymbolsToCurScene();

	ProjectInfo::Scene* GetSceneInfo(const char* sName = NULL);
};

extern Editor core_ed;

#endif