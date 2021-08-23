
#pragma once


#include "ScriptDocumentWidget.h"
#include "Core\Files\IStreamLoader.h"
#include "Editor\Helpers\SearchMatches.h"

class SceneInstance;

class ButtonWidget;
class TabSheetWidget;
class WindowWidget;
class TextBoxWidget;
class DialogWidget;
class LabelWidget;
class ComboBoxWidget;


#define GOTO_WINDOW_HEIGHT 120
#define GOTO_WINDOW_WIDTH  200


#define CONFIRM_WINDOW_HEIGHT 100
#define CONFIRM_WINDOW_WIDTH  200


#define SEARCH_WINDOW_HEIGHT			123
#define SEARCH_REPLACE_WINDOW_HEIGHT	153
#define SEARCH_WINDOW_WIDTH				300

class DEF_EXPORT TextEditorWidget : public TaskHoldersOwner
{
    friend class ScriptDocumentWidget;
public:
	enum ESearchType {eFindInSelection,eFindInProject,eFindInDocument};
private:
    typedef std::vector<ScriptDocumentWidget*> TDocuments;

    struct CodeList
    {
    private:
        RECT  rLabelRect;
        float nWidth;
        int   nMembersSeparator;
    public:
        LabelWidget*     pLabel;
        ComboBoxWidget*  pList;
        CodeList();
        ~CodeList();
        void  Create(int id,BaseWidget* pParent,const char* sText,float x,float y,float w,const  HFONT hFont);
        void  SetX(float x);
        void  Show(bool bSet){pLabel->Show(bSet);pList->Show(bSet);}
        float Width() const {return rLabelRect.right - 11 + nWidth;}
        void  SetSeparator(int nIndex){nMembersSeparator = nIndex;}
        int   GetSeparator() const { return nMembersSeparator;}
    };

    HFONT                               hFont;

    ButtonWidget*                       pUndoButton;
    ButtonWidget*                       pRedoButton;
    ButtonWidget*                       pCloseButton;
    ButtonWidget*                       pLoadButton;
    ButtonWidget*                       pSaveButton;

    TabSheetWidget*                     pTabs;

    DialogWidget*                       pConfirmWindow;
    LabelWidget*                        pConfirmStaticText;

    DialogWidget*                       pGoToWindow;
    TextBoxWidget*                      pGoToLineNumber;
    LabelWidget*                        pGoToStaticText;

    DialogWidget*                       pSearchWindow;

    ComboBoxWidget*                     pSearchText;
    ButtonWidget*                       pSearchNext;
    ButtonWidget*                       pSearchPrev;
    ComboBoxWidget*                     pReplaceText;
    ButtonWidget*                       pReplaceNext;
    ButtonWidget*                       pReplaceAll;
    ButtonWidget*                       pMatchCase;   
    ButtonWidget*                       pMatchWord;   
    ComboBoxWidget*                     pSearchTarget;
	LabelWidget*                        pSearchResults;

    CodeList                            rClassesList;
    CodeList                            rMembersList;

	TMatchesMap							vMatches;
	TMatches*                           pSearchMatchesInProjectIter;
    
       
    TDocuments                          vDocuments; 
    ScriptDocumentWidget::ColorScheme*  pCurColorScheme;
    ScriptDocumentWidget*               pCurDocument;
    ScriptDocumentWidget*               pClosedDocument;
	ScriptDocumentWidget*               pHelperDocument;
    unsigned                            nDocumentsCount;
    int                                 nStartTabIndex;
    ESearchType                         eFindType;
    int                                 nCurrTabIndex;   
    bool                                bInReParseMode;

    void  InitFind(ScriptDocumentWidget* pListener,float x,float y,const char* sText,ESearchType eType);
    void  CloseScript(ScriptDocumentWidget*  pDocument);
    void  Invalidate();
    void  ReOpenCurrentDocument(bool bSaveCurrent);
    void  CloseScript(bool bSave);
    void  OpenConfirmDialog(int id);
    void  SaveDocument(ScriptDocumentWidget*  pDocument);
    void  UpdatePanel(int index);
    void  UpdateMembers();
    void  ReParseDocuments(float dt, int level);
    void  FillNameSpaceBox();
    void  OpenDocument(int nIndex);

	int       FindNextMatcherInCurDocument(LRESULT pos,const TMatches* pList,bool bLoop,bool bCheckIn = false);
	int       FindPrevMatcherInCurDocument(LRESULT pos,const TMatches* pList,bool bLoop,bool bCheckIn = false);
	void      FindAllMatches();
	bool      GetMatch(int nIndex,SearchMatch& result);

	TMatches*    GetDocumentIterInMatches(ScriptDocumentWidget*  pDocument);

public:
	
	TextEditorWidget(TabSheetWidget* tabs,int nStartIndex);
	~TextEditorWidget();

    void ClearDocuments();
    void AddScript(const char* name,ProjectInfo::Scene* scene);
    void LoadScene(const char* name,ProjectInfo::Scene* scene,bool bSwitchTab = true);
    void SaveScene(const char* name);
    void SetSceneName(const char* name);
    void SaveAll();
    void SaveScene(ProjectInfo::Scene* scene);
    void CloseScript();
    void CloseScript(ProjectInfo::Scene* scene);
    void SaveCurrentDocument();
    void ReOpenCurrentDocument();
    int  GetCurDocumentSceneId();
    int  GetDocumentSceneId(int index);
	void GetCursorState(int& line, int& col);
    bool IsReParsing()const {return bInReParseMode;}
    void GoToLine(int nLine,int nPos = -1);
	void GoTo(int nLine,int nCol);
    bool GoToScriptError(const string& sText);
	bool GoToFindResult(int nIndex,const string& sPath);
    void OpenScriptTab(const char* sTabName);
    void OpenScriptTab(int nSceneId);
    void OnModified(){pSaveButton->SetEnable(true);}

    void SetColorScheme(ScriptDocumentWidget::ColorScheme* pScheme);
    void ShowGoToDialog(ScriptDocumentWidget* pListener,float x,float y,unsigned nCurLine,unsigned nLinesCount);
    void ShowSearchDialog(ScriptDocumentWidget* pListener,float x,float y,const char* sText,ESearchType eType);
    void ShowSearchAndReplaceDialog(ScriptDocumentWidget* pListener,float x,float y,const char* sText,ESearchType eType);
    const char* GetGoToDialogText();

    void Undo();
    void Redo();
    void GoToLine();
    void ReFind();
    void OnChangeFindTarget();
    void OnChangeFindFlags();
    void NextSearch();
    void PrevSearch();
    void Replace();
    void ReplaceAll(); 
    void OnClassesChange();
    void OnMembersChange();
	void Update();
	void UpdateMatches(ScriptDocumentWidget* pDocument,int nPos,int nOffset);
private:
	void ClearMatches();
	inline TMatches* GetMatches()
	{
		if(pCurDocument)
			return vMatches.Find(pCurDocument->GetFilePath());
		return NULL;
	}
};
