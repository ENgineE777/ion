
#include "TextEditorWidget.h"
#include "Core/ICore.h"
#include "Core\Scene\SceneInstance.h"
#include "TabSheetWidget.h"
#include "scintilla.h"
#include "WindowWidget.h"
#include "DialogWidget.h"

#include <strsafe.h>
#include "Entities\NGUI\NGUISymbol.h"
#include "Editor/Editor.h"
#include <algorithm>


TextEditorWidget::CodeList::~CodeList()
{
    if(pList)
        delete pList;
    if(pLabel)
        delete pLabel;
}

void TextEditorWidget::CodeList::Create(int id,BaseWidget* pParent,const char* sText,float x,float y,float w,const HFONT hFont)
{
    nWidth = w;
    pLabel = new LabelWidget(id,pParent,sText,false,x,y + 3,nWidth,26);
    HDC hdc = GetDC(pLabel->GetHandle());
    DrawText(hdc, sText, strlen(sText), &rLabelRect, DT_CALCRECT);
    SetWindowPos(pLabel->GetHandle(), 0,(int)x,(int)pLabel->GetY(), rLabelRect.right,rLabelRect.bottom,  SWP_NOZORDER);
    pList = new ComboBoxWidget(id, pParent, "",x + rLabelRect.right - 15, y,nWidth, nWidth,false);
    if (NULL != hFont) 
    {
        SendMessage(pList->GetHandle(),WM_SETFONT,(WPARAM)hFont,TRUE);
        SendMessage(pLabel->GetHandle(),WM_SETFONT,(WPARAM)hFont,TRUE);
    }
}

void TextEditorWidget::CodeList::SetX(float x)
{
    SetWindowPos(pLabel->GetHandle(), 0,(int)x,(int)pLabel->GetY(), rLabelRect.right,rLabelRect.bottom,  SWP_NOZORDER);
    SetWindowPos(pList->GetHandle(), 0,(int)x + rLabelRect.right - 15,(int)pList->GetY(), 0,0,  SWP_NOSIZE|SWP_NOZORDER);
    InvalidateRect(pLabel->GetHandle(),NULL,false);
    InvalidateRect(pList->GetHandle(),NULL,false);
}

TextEditorWidget::CodeList::CodeList() :pLabel(NULL),pList(NULL),nWidth(0),nMembersSeparator(-1)
{
    rLabelRect.left = rLabelRect.bottom = rLabelRect.right = rLabelRect.top = 0;
}


TextEditorWidget::TextEditorWidget(TabSheetWidget* tabs,int nStartIndex):pSaveButton(NULL),pLoadButton(NULL),pUndoButton(NULL),pRedoButton(NULL),pCloseButton(NULL),pCurDocument(NULL),pCurColorScheme(NULL)
    ,pGoToStaticText(NULL),pGoToLineNumber(NULL),pGoToWindow(NULL),pConfirmWindow(NULL),pConfirmStaticText(NULL),eFindType(eFindInDocument),nStartTabIndex(nStartIndex),pTabs(tabs),nCurrTabIndex(-1)
    ,pClosedDocument(NULL),hFont(NULL),bInReParseMode(false),pSearchMatchesInProjectIter(NULL)
{
#ifdef EDITOR
    NGUIWidget::BuildSignatures();
#endif
    LoadLibrary(TEXT("SciLexer.dll"));

    float x =  pTabs->GetXOffset();
    float y =  pTabs->GetYOffset();
    float w =  pTabs->GetWidth();
    float h =  pTabs->GetHeight();



    pUndoButton = new ButtonWidget(SCRIPT_DOCUMENT_UNDO_BUTTON_ID, pTabs, true, "Undo", x,y - 2, 24, 24);
    pUndoButton->SetImage("Editor/btn_arrow_prev.bmp");	
    pUndoButton->SetHoweredImage("Editor/btn_arrow_prev_over.bmp");
    pUndoButton->SetEnable(false);
    pRedoButton = new ButtonWidget(SCRIPT_DOCUMENT_REDO_BUTTON_ID, pTabs, true, "Redo",x + 30,y - 2, 24, 24);
    pRedoButton->SetImage("Editor/btn_arrow_next.bmp");	
    pRedoButton->SetHoweredImage("Editor/btn_arrow_next_over.bmp");
    pRedoButton->SetEnable(false);


    pSaveButton = new ButtonWidget(SCRIPT_DOCUMENT_SAVE_ID, pTabs, true, "Save", x + 60,y - 6, 32, 32);
    pSaveButton->SetImage("Editor/btn_save.bmp");	
    pSaveButton->SetHoweredImage("Editor/btn_save_over.bmp");
    pSaveButton->SetEnable(false);

    pLoadButton = new ButtonWidget(SCRIPT_DOCUMENT_OPEN_ID, pTabs, true, "Load", x + 96,y - 6, 32, 32);
    pLoadButton->SetImage("Editor/btn_open.bmp");	
    pLoadButton->SetHoweredImage("Editor/btn_open_over.bmp");
    pLoadButton->SetEnable(false);


    rClassesList.Create(SCRIPT_DOCUMENT_CLASSES_LIST_ID,pTabs,"NameSpaces:",x + 132, y,300,hFont);
    rMembersList.Create(SCRIPT_DOCUMENT_MEMBERS_LIST_ID,pTabs,"Members:",x + 132 + rClassesList.Width(), y,350,hFont);

    pCloseButton = new ButtonWidget(SCRIPT_DOCUMENT_CLOSE_BUTTON_ID, pTabs, true, "Close", x + w - 30,y, 30, 20);
    pCloseButton->SetImage("Editor/close2.bmp");	
    pCloseButton->SetHoweredImage("Editor/closehot2.bmp");

	pHelperDocument = new ScriptDocumentWidget(SCRIPT_DOCUMENTS_ID_OFFSET,pTabs,x,y,w,h);
	pHelperDocument->SetIndex(0);
	pHelperDocument->Show(false);

    pCurDocument = new ScriptDocumentWidget(SCRIPT_DOCUMENTS_ID_OFFSET,pTabs,x,y,w,h);
//    pCurDocument->SetIndex(nStartTabIndex);
    pCurDocument->SetOwner(this);
    nDocumentsCount = 1;
    pCurDocument->SetIndex(0);
    pTabs->AddTabWidget(nStartIndex,pCurDocument);
    vDocuments.push_back(pCurDocument);

    LOGFONT lf;
    memset( &lf, 0, sizeof(LOGFONT) );
    StringCchCopy(lf.lfFaceName, 32, "Times New Roman");
    lf.lfHeight = 18;
    hFont = CreateFontIndirect(&lf);

    pConfirmWindow = new DialogWidget(SCRIPT_DOCUMENT_CONFIRM_CLOSE_WINDOW_ID,"Confirm Dialog...",100, 100, CONFIRM_WINDOW_WIDTH, CONFIRM_WINDOW_HEIGHT);
    RECT rect;
    GetClientRect(pConfirmWindow->GetHandle(),&rect);
    pConfirmStaticText = new LabelWidget(SCRIPT_DOCUMENT_GOTO_WINDOW_ID,pConfirmWindow,"",false,(float)rect.left + 10,(float)rect.top + 10,(float)rect.right - 15,20,BaseWidget::eAutoSizeNone,SS_CENTER);
    if (NULL != hFont) 
       SendMessage(pConfirmStaticText->GetHandle(),WM_SETFONT,(WPARAM)hFont,TRUE);

    pGoToWindow = new DialogWidget(SCRIPT_DOCUMENT_GOTO_WINDOW_ID,"Go To Line",100, 100, GOTO_WINDOW_WIDTH, GOTO_WINDOW_HEIGHT);
    GetClientRect(pGoToWindow->GetHandle(),&rect);
    pGoToStaticText = new LabelWidget(SCRIPT_DOCUMENT_GOTO_WINDOW_ID,pGoToWindow,"",false,(float)rect.left + 10,(float)rect.top + 10,(float)rect.right - 15,20);
    pGoToLineNumber = new TextBoxWidget(SCRIPT_DOCUMENT_GOTO_WINDOW_ID,pGoToWindow,"",(float)rect.left + 10,(float)rect.top + 30,(float)rect.right - 15,20,BaseWidget::eAutoSizeNone,TextBoxWidget::eUnsigned);

    pSearchWindow = new DialogWidget(SCRIPT_DOCUMENT_SEARCH_WINDOW_ID,"Find and Replace",100, 100, SEARCH_WINDOW_WIDTH, SEARCH_REPLACE_WINDOW_HEIGHT,DialogWidget::DialogStyles::eButtonCancel);
    GetClientRect(pSearchWindow->GetHandle(),&rect);
   
	pSearchResults  = new LabelWidget(SCRIPT_DOCUMENT_SEARCH_WINDOW_ID,pSearchWindow,"",false,(float)rect.left + 10,(float)rect.top + 5,(float)rect.right - 20,20);
	

    pSearchText = new ComboBoxWidget(SCRIPT_DOCUMENT_SEARCH_TEXT_ID,pSearchWindow,"",(float)rect.left + 10,(float)rect.top + 28,(float)rect.right - 80,(float)rect.bottom - rect.top - 10 ,false,BaseWidget::eAutoSizeNone,true);
    if (NULL != hFont) 
          SendMessage(pSearchText->GetHandle(),WM_SETFONT,(WPARAM)hFont,TRUE);

    pSearchNext = new ButtonWidget(SCRIPT_DOCUMENT_SEARCH_NEXT_ID, pSearchWindow, true, "Next", (float)rect.right - 30,(float)rect.top + 28, 24, 24);
    pSearchNext->SetImage("Editor/btn_arrow_next.bmp");	
    pSearchNext->SetHoweredImage("Editor/btn_arrow_next_over.bmp");

    pSearchPrev = new ButtonWidget(SCRIPT_DOCUMENT_SEARCH_PREV_ID, pSearchWindow, true, "Prev",(float)rect.right - 60,(float)rect.top + 28, 24, 24);
    pSearchPrev->SetImage("Editor/btn_arrow_prev.bmp");	
    pSearchPrev->SetHoweredImage("Editor/btn_arrow_prev_over.bmp");


    pReplaceText = new ComboBoxWidget(SCRIPT_DOCUMENT_SEARCH_WINDOW_ID,pSearchWindow,"",(float)rect.left + 10,(float)rect.top + 58,(float)rect.right - 80,(float)rect.bottom - rect.top - 40,false,BaseWidget::eAutoSizeNone,true);
    if (NULL != hFont) 
        SendMessage(pReplaceText->GetHandle(),WM_SETFONT,(WPARAM)hFont,TRUE);

    pReplaceNext = new ButtonWidget(SCRIPT_DOCUMENT_SEARCH_REPLACE_ID, pSearchWindow, true, "Replace", (float)rect.right - 60,(float)rect.top + 58, 24, 24);
    pReplaceNext->SetImage("Editor/btn_replace.bmp");	
    pReplaceNext->SetHoweredImage("Editor/btn_replace_over.bmp");

    pReplaceAll = new ButtonWidget(SCRIPT_DOCUMENT_SEARCH_REPLACEAll_ID, pSearchWindow, true, "Replace All",(float)rect.right - 30,(float)rect.top + 58, 24, 24);
    pReplaceAll->SetImage("Editor/btn_replaceall.bmp");	
    pReplaceAll->SetHoweredImage("Editor/btn_replaceall_over.bmp");

    pMatchCase = new ButtonWidget(SCRIPT_DOCUMENT_SEARCH_MATCH_CASE_ID,pSearchWindow, true,"Match case",(float)rect.left + 10,(float)rect.top + 88,24,24);
    pMatchCase->SetPushable(true);
    pMatchCase->SetImage("Editor/btn_match_case.bmp");	
    pMatchCase->SetHoweredImage("Editor/btn_match_case_over.bmp");
    pMatchCase->SetPushedImage("Editor/btn_match_case_pushed.bmp");
    pMatchCase->SetHoweredPushedImage("Editor/btn_match_case_over_pushed.bmp");
    pMatchWord = new ButtonWidget(SCRIPT_DOCUMENT_SEARCH_MATCH_WORD_ID,pSearchWindow, true,"Match whole word",(float)rect.left + 42,(float)rect.top + 88,24,24);
    pMatchWord->SetPushable(true);
    pMatchWord->SetImage("Editor/btn_match_word.bmp");	
    pMatchWord->SetHoweredImage("Editor/btn_match_word_over.bmp");
    pMatchWord->SetPushedImage("Editor/btn_match_word_pushed.bmp");
    pMatchWord->SetHoweredPushedImage("Editor/btn_match_word_over_pushed.bmp");

    pSearchTarget = new ComboBoxWidget(SCRIPT_DOCUMENT_SEARCH_TARGET_ID,pSearchWindow,"",(float)rect.left + 72,(float)rect.top + 88,(float)rect.right - rect.left - 142,100,false);

//#ifndef _DEBUG
    core->AddTask(this,this, (TaskHolder::Task)&TextEditorWidget::ReParseDocuments, lvl_camera1, 5.0f);
//#endif


}


TextEditorWidget::~TextEditorWidget()
{
//#ifndef _DEBUG
    core->DelTask(this, (TaskHolder::Task)&TextEditorWidget::ReParseDocuments);
//#endif
    if(pUndoButton)delete pUndoButton;
    if(pRedoButton)delete pRedoButton;
    if(pCloseButton)delete pCloseButton;
    if(pLoadButton)delete pLoadButton;
    if(pSaveButton)delete pSaveButton;
    if(pGoToWindow)delete pGoToWindow;
    if(pGoToStaticText)delete pGoToStaticText;
    if(pGoToLineNumber)delete pGoToLineNumber;
    if(pConfirmStaticText)delete pConfirmStaticText;
    if(pConfirmWindow)delete pConfirmWindow;
    if(hFont)
        DeleteObject(hFont);

}
void TextEditorWidget::LoadScene(const char* name,ProjectInfo::Scene* scene,bool bSwitchTab)
{
    char script_name[512];
    String::Copy(script_name, 512,name);				

    script_name[strlen(script_name)-3] = 's';
    script_name[strlen(script_name)-2] = 'r';
    script_name[strlen(script_name)-1] = 't';

    char sTabName[128];
    String::ExtractFileName(script_name,sTabName);
    String::RemoveExctention(sTabName);

    /*if(String::IsEqual(vDocuments[0]->GetName(),sTabName))
    {
       OpenDocument(0);
       return;
    }*/

    vDocuments[0]->SetFile(script_name);
	if(!vDocuments[0]->LoadFromFile())
		vDocuments[0]->SaveToFile(true);
    vDocuments[0]->SetScene(scene);
    pCurDocument = vDocuments[0];
    pTabs->SetTabName(nStartTabIndex,vDocuments[0]->GetName());
    if(bSwitchTab)
        pTabs->SetCurrentTab(nStartTabIndex);
    nCurrTabIndex = -1;
    UpdatePanel(pTabs->GetCurTabIndex());
    nCurrTabIndex = pTabs->GetCurTabIndex();
    core_ed.AddRecentFile(vDocuments[0]->GetFilePath());



}
void  TextEditorWidget::OpenDocument(int nIndex)
{
    pCurDocument = vDocuments[nIndex];
    nCurrTabIndex = -1;
    UpdatePanel(nIndex + nStartTabIndex);
    nCurrTabIndex = nIndex + nStartTabIndex;
    pTabs->SetCurrentTab(nCurrTabIndex);
    Invalidate();
}
void TextEditorWidget::OpenScriptTab(const char* sTabName)
{
    for(unsigned i = 0;i <nDocumentsCount;++i)
        if(String::IsEqual(vDocuments[i]->GetName(),sTabName))
        {
            OpenDocument(i);
            return;
        }
}

void TextEditorWidget::OpenScriptTab(int nSceneId)
{
    for(unsigned i = 0;i <nDocumentsCount;++i)
        if(vDocuments[i]->SceneId() == nSceneId)
        {
            OpenDocument(i);
            return;
        }
}


void TextEditorWidget::AddScript(const char* name,ProjectInfo::Scene* scene)
{
    char script_name[512];
    String::Copy(script_name, 512,name);				

    script_name[strlen(script_name)-3] = 's';
    script_name[strlen(script_name)-2] = 'r';
    script_name[strlen(script_name)-1] = 't';

    char sTabName[128];
    String::ExtractFileName(script_name,sTabName);
    String::RemoveExctention(sTabName);

    for(unsigned i = 0;i <nDocumentsCount;++i)
        if(String::IsEqual(vDocuments[i]->GetName(),sTabName))
        {
            OpenDocument(i);
            return;
        }

    pTabs->AddTab(sTabName);

    ++nDocumentsCount;
    if(nDocumentsCount > vDocuments.size())
    {
        pCurDocument = new ScriptDocumentWidget(SCRIPT_DOCUMENTS_ID_OFFSET + vDocuments.size(),pTabs,pTabs->GetXOffset(), pTabs->GetYOffset(),pTabs->GetWidth(),pTabs->GetHeight());
        pCurDocument->SetColorScheme(pCurColorScheme);
        pCurDocument->SetSize(pTabs->GetWidth(),pTabs->GetHeight());
        pCurDocument->SetOwner(this);
        vDocuments.push_back(pCurDocument);
    }
    else
        pCurDocument = vDocuments[nDocumentsCount - 1];
    pCurDocument->SetFile(script_name);
    pCurDocument->SetScene(scene);
    pCurDocument->LoadFromFile();
    pCurDocument->SetIndex(nDocumentsCount - 1);
    UpdatePanel(nDocumentsCount - 1 + nStartTabIndex);
    nCurrTabIndex = nDocumentsCount - 1 + nStartTabIndex;
    pTabs->AddTabWidget(nCurrTabIndex,pCurDocument);
    pTabs->SetCurrentTab(nCurrTabIndex);
    Invalidate();
    core_ed.AddRecentFile(pCurDocument->GetFilePath());
}

void TextEditorWidget::SaveScene(const char* name)
{
     char script_name[512];
     String::Copy(script_name, 512,name);				
     script_name[strlen(script_name)-3] = 's';
     script_name[strlen(script_name)-2] = 'r';
     script_name[strlen(script_name)-1] = 't';	
     vDocuments[0]->SaveToFile(script_name);
}

void TextEditorWidget::SetSceneName(const char* name)
{
    char script_name[512];
    String::Copy(script_name, 512,name);				
    script_name[strlen(script_name)-3] = 's';
    script_name[strlen(script_name)-2] = 'r';
    script_name[strlen(script_name)-1] = 't';	
    vDocuments[0]->SetFile(script_name);
    vDocuments[0]->SaveToFile();
    pTabs->SetTabName(nStartTabIndex,vDocuments[0]->GetName());
    core_ed.AddRecentFile(vDocuments[0]->GetFilePath());
    nCurrTabIndex = -1;
    UpdatePanel(pTabs->GetCurTabIndex());
    nCurrTabIndex = pTabs->GetCurTabIndex();
}

void TextEditorWidget::SaveScene(ProjectInfo::Scene* scene)
{
    ScriptDocumentWidget*  pDocument = NULL;
    for(unsigned i = 0; i < vDocuments.size();++i)
        if(vDocuments[i]->Scene() == scene)
        {
            pDocument = vDocuments[i];
            break;
        }

     if(pDocument)
       pDocument->SaveToFile();
}

void TextEditorWidget::SaveAll()
{
    for(unsigned i = 0;i <vDocuments.size();++i)
        vDocuments[i]->SaveToFile();
}

void TextEditorWidget::GetCursorState(int& line, int& col)
{
    if(pCurDocument)
        pCurDocument->GetCursorState(line,col);
}


void TextEditorWidget::UpdatePanel(int index)
{
    if(index != nCurrTabIndex)
    {
        pUndoButton->Show(index >=  nStartTabIndex);
        pRedoButton->Show(index >=  nStartTabIndex);
        pSaveButton->Show(index >=  nStartTabIndex);
        rClassesList.Show(index >=  nStartTabIndex);
        rMembersList.Show(index >=  nStartTabIndex);
        pCloseButton->Show(index >  nStartTabIndex);
        pLoadButton->Show(index >  nStartTabIndex);

        if(pCurDocument)
            pSaveButton->SetEnable(pCurDocument->isModified());
        pLoadButton->SetEnable(index >  nStartTabIndex);
        FillNameSpaceBox();
        SetWindowPos(pCloseButton->GetHandle(), 0,(int)(pTabs->GetXOffset() + vDocuments[0]->GetWidth() - pCloseButton->GetWidth()),(int)pCloseButton->GetY(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        Invalidate();
    }
}


void TextEditorWidget::Update()
{
    int index = pTabs->GetCurTabIndex();
    if(index >= nStartTabIndex)
    {
        pCurDocument = vDocuments[index - nStartTabIndex];
        if(index != nCurrTabIndex)
            core_ed.OnSelectSceneTab(pCurDocument->SceneId());
    }
    UpdatePanel(index);
    nCurrTabIndex = index;
    for(unsigned i = 0;i < nDocumentsCount;++i)
        vDocuments[i]->Update();
    if(pCurDocument)
    {
        pUndoButton->SetEnable(pCurDocument->CanUndo());
        pRedoButton->SetEnable(pCurDocument->CanRedo());
	}
	if(pSearchWindow->IsVisible())
	{
		pReplaceText->GetTextFromWidget();
		pReplaceNext->SetEnable(strlen(pReplaceText->GetText()) > 0 );
		pReplaceAll->SetEnable(strlen(pReplaceText->GetText()) > 0 );
		TMatches* pvList = GetMatches();
		if(pvList)
		{
			for(unsigned i = 0;i < pvList->size();++i)            
			{
				SearchMatch& rMatcher = pvList->at(i);
				if(rMatcher.vOldStyles.empty())
					pCurDocument->GetTextStyles(rMatcher.nStart,rMatcher.nEnd,rMatcher.vOldStyles);
				pCurDocument->SetTextStyle(rMatcher.nStart,rMatcher.nEnd,CUSTOM_STYLE_SEARCH);
			}
		}
	}
	else
		ClearMatches();
}

void TextEditorWidget::Invalidate()
{
    InvalidateRect(pUndoButton->GetHandle(), null, false);
    InvalidateRect(pRedoButton->GetHandle(), null, false);
    InvalidateRect(pCloseButton->GetHandle(), null, false);
    InvalidateRect(pCurDocument->GetHandle(), null, false);
    InvalidateRect(pSaveButton->GetHandle(), null, false);
    InvalidateRect(pLoadButton->GetHandle(), null, false);
}


void TextEditorWidget::SetColorScheme(ScriptDocumentWidget::ColorScheme* pScheme)
{
    pCurColorScheme = pScheme;
    for(unsigned i = 0;i <vDocuments.size();++i)
        vDocuments[i]->SetColorScheme(pScheme);
}

void TextEditorWidget::Undo()
{
    if(pCurDocument)
        pCurDocument->Undo();
}

void TextEditorWidget::Redo()
{
    if(pCurDocument)
        pCurDocument->Redo();
}


void TextEditorWidget::ClearDocuments()
{
    while(nDocumentsCount > 1)
    {
        ScriptDocumentWidget* pDocument = vDocuments[1];
        CloseScript(pDocument);
    }
    vDocuments[0]->ClearText();
    vDocuments[0]->SetFile("");
    vDocuments[0]->SetScene(NULL);
}

void TextEditorWidget::CloseScript()
{
    if(pCurDocument)
    {
        if(pCurDocument->isModified())
        {
            pClosedDocument = pCurDocument;
            OpenConfirmDialog(SCRIPT_DOCUMENT_CONFIRM_CLOSE_WINDOW_ID);
        }
        else
            CloseScript(pCurDocument);
    }
}

void TextEditorWidget::CloseScript(ProjectInfo::Scene* scene)
{
    ScriptDocumentWidget*  pDocument = NULL;
    for(unsigned i = 0; i < vDocuments.size();++i)
        if(vDocuments[i]->Scene() == scene)
        {
            pDocument = vDocuments[i];
            break;
        }

    if(pDocument)
    {
      if(pDocument->isModified())
      {
          pClosedDocument = pDocument;
          OpenConfirmDialog(SCRIPT_DOCUMENT_CONFIRM_CLOSE_WINDOW_ID);
      }
      else 
          CloseScript(pDocument);
    }

}


void TextEditorWidget::CloseScript(bool bSave)
{
    pConfirmWindow->Show(false);
    if(pClosedDocument)
    {
        if(bSave)SaveDocument(pClosedDocument);
        CloseScript(pClosedDocument);
        pClosedDocument = NULL;
    }
}



void TextEditorWidget::CloseScript(ScriptDocumentWidget* pDocument)
{
    int index = pDocument->Index();
    if(index >= 0 && index < (int)vDocuments.size())
    {
        for(unsigned i = index; i < nDocumentsCount - 1;++i)
        {
            vDocuments[i] = vDocuments[i + 1];
            vDocuments[i]->SetIndex(i);
        }
        vDocuments[nDocumentsCount - 1] = pDocument;
        pDocument->SetIndex(-1);
        pDocument->SetScene(NULL);
        pDocument->ClearText();
        pDocument->SetFile("");
		pTabs->SetTabName(nStartTabIndex + index, "");
        if(index - 1 >= 0)
        {
			--nDocumentsCount;
            if(pTabs->GetCurrentTab() == nStartTabIndex + index)
            {
                UpdatePanel(nStartTabIndex + index - 1);
                nCurrTabIndex = nStartTabIndex + index - 1;
                pTabs->SetCurrentTab(nCurrTabIndex);
                pCurDocument = vDocuments[index - 1];
                core_ed.OnSelectSceneTab(pCurDocument->SceneId());
            }
            pTabs->CloseTab(nStartTabIndex + index);
            Invalidate();
        }
    }
}

void TextEditorWidget::ShowGoToDialog(ScriptDocumentWidget* pListener,float x,float y,unsigned nCurLine,unsigned nLinesCount)
{
    pSearchWindow->Show(false);
    pGoToWindow->SetListener(pListener);
    pGoToWindow->SetPos(x,y);
    pGoToWindow->Show(true);
    char  buff[256];
    _itoa_s(nCurLine,buff,10);
    pGoToLineNumber->SetText(buff);
    pGoToLineNumber->SelectText();
    String::Printf(buff,256,"Line number(1 - %d):",nLinesCount);
    pGoToStaticText->SetText(buff);
    SetFocus(pGoToLineNumber->GetHandle());
}

const char* TextEditorWidget::GetGoToDialogText()
{
    return pGoToLineNumber->GetText();
}

void TextEditorWidget::GoToLine()
{
    pGoToWindow->OkPress();
}

void TextEditorWidget::GoToLine(int nLine,int nPos /*= -1*/)
{
    if(pCurDocument)
        pCurDocument->GoToLine(nLine,nPos);
}
void TextEditorWidget::GoTo(int nLine,int nCol)
{
	if(pCurDocument)
		pCurDocument->GoTo(nLine,nCol);
}

int TextEditorWidget::GetCurDocumentSceneId()
{
    if(pCurDocument)
        return pCurDocument->SceneId();
    return -1;
}

int TextEditorWidget::GetDocumentSceneId(int index)
{
    if(index < (int)vDocuments.size())
        return vDocuments[index]->SceneId();
    return -1;
}

void TextEditorWidget::ShowSearchDialog(ScriptDocumentWidget* pListener,float x,float y,const char* sText,ESearchType eType)
{
     RECT rect;
     GetClientRect(pGoToWindow->GetHandle(),&rect);
     SetWindowText(pSearchWindow->GetHandle(),"Find");
     pReplaceText->Show(false);
     pReplaceNext->Show(false);
     pReplaceAll->Show(false);
     long newY = rect.top + 58;
     SetWindowPos(pMatchCase->GetHandle(), 0,(int)pMatchCase->GetX(), newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
     SetWindowPos(pMatchWord->GetHandle(), 0,(int) pMatchWord->GetX(), newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
     SetWindowPos(pSearchTarget->GetHandle(), 0,(int) pSearchTarget->GetX(), newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
     pSearchWindow->SetSize(pSearchWindow->GetWidth(),SEARCH_WINDOW_HEIGHT);
     SetFocus(pSearchText->GetHandle());
     InitFind(pListener,x,y,sText,eType);
}



void TextEditorWidget::ShowSearchAndReplaceDialog(ScriptDocumentWidget* pListener,float x,float y,const char* sText,ESearchType eType)
{
    RECT rect;
    GetClientRect(pGoToWindow->GetHandle(),&rect);
    SetWindowText(pSearchWindow->GetHandle(),"Find and Replace");
    pReplaceText->Show(true);
    pReplaceNext->Show(true);
    pReplaceAll->Show(true);
    long newY = rect.top + 88;
    SetWindowPos(pMatchCase->GetHandle(), 0,(int)pMatchCase->GetX(), newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(pMatchWord->GetHandle(), 0, (int)pMatchWord->GetX(), newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(pSearchTarget->GetHandle(), 0, (int)pSearchTarget->GetX(), newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    pSearchWindow->SetSize(pSearchWindow->GetWidth(),SEARCH_REPLACE_WINDOW_HEIGHT);
    SetFocus(pReplaceText->GetHandle());
    InitFind(pListener,x,y,sText,eType);
}

struct SearchMatchesComparator 
{
	bool operator() (const SearchMatch& i,const SearchMatch& j) { return (i.nEnd<j.nEnd);}
} SearchMatchesComparatorObject;


void TextEditorWidget::FindAllMatches()
{
	static char sResultText[64];
	if(eFindType == eFindInDocument || eFindType == eFindInSelection)
	{
		if(pCurDocument)
		{
			pCurDocument->ClearMatchesSelection(GetMatches()); 
			vMatches.Clear();
			vMatches.Insert(pCurDocument->GetFilePath(),TMatches());
			TMatches* pList = vMatches.Find(pCurDocument->GetFilePath());
			pCurDocument->SetFindFlags(pMatchCase->IsPushed(),pMatchWord->IsPushed());
			pCurDocument->FindAllMatches(pList,pSearchText->GetText(),eFindType == eFindInSelection);
			if(pList->size() > 1)
				std::sort(pList->begin(),pList->end(),SearchMatchesComparatorObject);
			String::Printf(sResultText,64,"Matches found: %d",pList->size());
			pSearchResults->SetText(sResultText);
		}
	}
	else
	{
		ClearMatches();
		pSearchMatchesInProjectIter = NULL;
		std::vector<string> vList;
		core->Script()->GetScriptFilesList(vList);
		const char* sSearchText = pSearchText->GetText();
		unsigned nSize = 0, nMatchesCount = 0;
		for(unsigned i = 0;i < vList.size();++i)
		{
			const char* sFilePath =  ScriptService::GetScriptFilePath(vList[i].c_str());
			ScriptDocumentWidget* pDocument = NULL;
			for(unsigned i =0 ;i < nDocumentsCount;++i)
			{
				if(vDocuments[i]->GetFilePath() == string(sFilePath))
				{
					pDocument = vDocuments[i];
					break;
				}
			}
			if(i >= nDocumentsCount)
			{
				pHelperDocument->SetFile(sFilePath);
				if(pHelperDocument->LoadFromFile())
					pDocument = pHelperDocument;
			}
			if(pDocument)
			{
				vMatches.Insert(sFilePath,TMatches());
				TMatches* pList = vMatches.Find(sFilePath);
				pDocument->SetFindFlags(pMatchCase->IsPushed(),pMatchWord->IsPushed());
				pDocument->FindAllMatches(pList,sSearchText,false);
				if(pList->empty())
					vMatches.Delete(sFilePath);
				else
					nMatchesCount += pList->size();
			}
		}
		String::Printf(sResultText,64,"Matches found: %d in %d files",nMatchesCount,vMatches.Size());
		pSearchResults->SetText(sResultText);
		core_ed.ShowFindResults(vMatches,sSearchText);
	}
}

void TextEditorWidget::InitFind(ScriptDocumentWidget* pListener,float x,float y,const char* sText,ESearchType eType)
{
    pGoToWindow->Show(false);
    eFindType = eType;
    pSearchTarget->ClearList();

	pSearchTarget->AddString("Current Document");
    pSearchTarget->AddString("Current Project");
    if(eFindType == eFindInSelection)
    {
        pSearchTarget->AddString("Selection");
        pSearchTarget->SetCurString(2);
    }
    else
	{
		if(eFindType == eFindInDocument)
			pSearchTarget->SetCurString(0);
		else
			pSearchTarget->SetCurString(1);
	}

    if(strlen(sText) > 0)
    {
        int index = pSearchText->GetIndex(sText);
        if (index == CB_ERR)
        {
            pSearchText->AddString(sText);
            pSearchText->SetCurString(sText);
        }
        else
            pSearchText->SetCurString(index);
    }
	else
		pSearchText->ClearText();

    if(strlen(pSearchText->GetText()) > 0)
		FindAllMatches();
	else
		pSearchResults->SetText("Matches found: 0");

    pSearchWindow->SetListener(pListener);
    pSearchWindow->SetPos(x,y);
    pSearchWindow->Show(true);
}

void TextEditorWidget::ReFind()
{
    if(pSearchWindow->IsVisible())
		FindAllMatches();
}

void TextEditorWidget::OnChangeFindTarget()
{    
	eFindType = eFindInDocument;
    if(String::IsEqual(pSearchTarget->GetText(),"Selection"))
	{
		eFindType = eFindInSelection;
	}
	else
	{
		if(String::IsEqual(pSearchTarget->GetText(),"Current Project"))
			eFindType = eFindInProject;
	}
	FindAllMatches();
}

void TextEditorWidget::OnChangeFindFlags()
{
    FindAllMatches();
}

int  TextEditorWidget::FindNextMatcherInCurDocument(LRESULT pos,const TMatches* pList,bool bLoop,bool bCheckIn)
{
	unsigned i = 0;
	for(;i < pList->size() - 1;++i)
	{
		const SearchMatch& rMatcher = pList->at(i);
		const SearchMatch& rMatcherNext = pList->at(i + 1);
		if(bCheckIn && pos > rMatcher.nStart && pos < rMatcher.nEnd)break;
		if(pos > rMatcher.nStart && pos < rMatcherNext.nStart)
		{
			++i;
			break;
		}
	}
	if(i == pList->size() - 1 && (pList->at(i).nEnd <= pos || pList->at(0).nStart > pos))
	{
		if(bLoop)i = 0;
		else return -1;
	}
	return i;
}

void TextEditorWidget::NextSearch()
{
	if(eFindType == eFindInDocument || eFindType == eFindInSelection)
	{
		if(pCurDocument)
		{
			TMatches* pList = GetMatches();
			pCurDocument->Goto(pList->at(FindNextMatcherInCurDocument(pCurDocument->GetCurrentPos(),pList,true)));
		}
	}
	else
	{
		if(!pSearchMatchesInProjectIter)
		{
			if(pCurDocument)
				pSearchMatchesInProjectIter = GetDocumentIterInMatches(pCurDocument);
			if(!pSearchMatchesInProjectIter)
			{
				TMatchesMap::Iterator it  = vMatches.Begin();
				pSearchMatchesInProjectIter = (*it);
				core_ed.GoToScript(it.GetKey().c_str(),0);
			}
		}
		int index = FindNextMatcherInCurDocument(pCurDocument->GetCurrentPos(),pSearchMatchesInProjectIter,false);
		if(index != -1)
		{
			pCurDocument->Goto(pSearchMatchesInProjectIter->at(index));
		}
		else
		{
			TMatchesMap::Iterator it = vMatches.Begin();
			while(it != vMatches.End())
			{
				if(string(pCurDocument->GetFilePath()) == it.GetKey())
				{
					++it;
					string sPath;
					if(it == vMatches.End())
					{
						TMatchesMap::Iterator first =  vMatches.Begin();
						sPath = first.GetKey();
						pSearchMatchesInProjectIter = *first;
					}
					else
					{
						sPath = it.GetKey();
						pSearchMatchesInProjectIter = *it;
					}
					if(!sPath.empty())
					{
						SearchMatch& rMatch = pSearchMatchesInProjectIter->at(0);
						core_ed.GoToScript(sPath.c_str(),rMatch.nRow - 1,rMatch.nCol);
					}
					return;
				}
				++it;
			}
		}
	}
}
int  TextEditorWidget::FindPrevMatcherInCurDocument(LRESULT pos,const TMatches* pList,bool bLoop,bool bCheckIn)
{
	int i = pList->size() - 1;
	for(;i > 0;--i)
	{
		const SearchMatch& rMatcher = pList->at(i);
		const SearchMatch& rMatcherPrev = pList->at(i - 1);
		if(pos <= rMatcher.nEnd && pos > rMatcherPrev.nEnd)
		{
			--i;
			break;
		}
	}
	if(i == 0 && (pList->at(i).nEnd >= pos  ||  pList->at( pList->size() - 1).nEnd < pos ))
	{
		if(bLoop)
			i = pList->size() - 1;
		else
			return -1;
	}
	return i;
}
void TextEditorWidget::PrevSearch()
{
	if(eFindType == eFindInDocument || eFindType == eFindInSelection)
	{
		if(pCurDocument)
		{
			TMatches* pList = GetMatches();
			pCurDocument->Goto(pList->at(FindPrevMatcherInCurDocument(pCurDocument->GetCurrentPos(),pList,true)));
		}
	}
	else
	{
		if(!pSearchMatchesInProjectIter)
		{
			if(pCurDocument)
				pSearchMatchesInProjectIter = GetDocumentIterInMatches(pCurDocument);
			if(!pSearchMatchesInProjectIter)
			{
				TMatchesMap::ReversIterator it  = vMatches.ReBegin();
				pSearchMatchesInProjectIter = (*it);
				core_ed.GoToScript(it.GetKey().c_str(),0);
			}
		}
		int index = FindPrevMatcherInCurDocument(pCurDocument->GetCurrentPos(),pSearchMatchesInProjectIter,false);
		if(index != -1)
		{
			pCurDocument->Goto(pSearchMatchesInProjectIter->at(index));
		}
		else
		{
			TMatchesMap::ReversIterator it = vMatches.ReBegin();
			while(it != vMatches.ReEnd())
			{
				if(string(pCurDocument->GetFilePath()) == it.GetKey())
				{
					++it;
					string sPath;
					if(it == vMatches.ReEnd())
					{
						TMatchesMap::ReversIterator first = vMatches.ReBegin();
						sPath = first.GetKey();
						pSearchMatchesInProjectIter = *first;
					}
					else
					{
						sPath = it.GetKey();
						pSearchMatchesInProjectIter = *it;
					}
					if(!sPath.empty())
					{
						SearchMatch& rMatch = pSearchMatchesInProjectIter->at(pSearchMatchesInProjectIter->size() - 1);
						core_ed.GoToScript(sPath.c_str(),rMatch.nRow - 1,rMatch.nCol);
					}
					return;
				}
				++it;
			}
		}
	}
}

void TextEditorWidget::Replace()
{
    if(pCurDocument)
    {
		const char* sNewText = pReplaceText->GetText();
        if(strlen(sNewText) > 0)
        {
			TMatches* pList = GetMatches();
			unsigned  nIndex = FindNextMatcherInCurDocument(pCurDocument->GetCurrentPos(),pList,true,true);
            pCurDocument->Replace(pList,nIndex,sNewText);
			pCurDocument->SetTextStyle(pList->at(nIndex).nStart,pList->at(nIndex).nEnd,CUSTOM_STYLE_SEARCH);
			pCurDocument->Goto(pList->at(nIndex));
			pReplaceText->AddString(sNewText);
        }
    }
}

void TextEditorWidget::ReplaceAll()
{
    if(pCurDocument)
    {
		const char* sNewText = pReplaceText->GetText();
        if(strlen(sNewText) > 0)
        {
			TMatches* pList = GetMatches();

			for(unsigned i = 0;i < pList->size();++i)
				  pCurDocument->Replace(pList,i,sNewText);


            pReplaceText->AddString(pReplaceText->GetText());
            SetFocus(pCurDocument->GetHandle());
			for(unsigned i = 0;i < pList->size();++i)
				pCurDocument->SetTextStyle(pList->at(i).nStart,pList->at(i).nEnd,CUSTOM_STYLE_SEARCH);
        }
    }
}

void TextEditorWidget::SaveCurrentDocument()
{
    if(pCurDocument)
    {
        SaveDocument(pCurDocument);
        pSaveButton->SetEnable(false);
    }

}

void TextEditorWidget::SaveDocument(ScriptDocumentWidget* pDocument)
{
    const char* pFileName = pDocument->GetFilePath();
    if(strlen(pFileName) == 0)
    {
        char sPath[512];
        sPath[0] = 0;
        if (DialogSelectFile(false, sPath, true, "Script file(*.srt)\0*.srt\0"))
        {	
            if (string(sPath).find(".") == string::npos)
                String::Cat(sPath, 512, ".srt");							
            pDocument->SetFile(sPath);
            pDocument->SaveToFile();
            pTabs->SetTabName(nCurrTabIndex,pDocument->GetName());
        }
    }
    else
        pDocument->SaveToFile();
}

void TextEditorWidget::OpenConfirmDialog(int id)
{
    pConfirmStaticText->SetText("Save current file ?");
    pConfirmWindow->SetListener(pCurDocument);
    pConfirmWindow->SetId(id);
    WINDOWINFO info;
    GetWindowInfo(pCurDocument->GetHandle(),&info);
    pConfirmWindow->SetPos(info.rcWindow.left + (pCurDocument->GetWidth() - CONFIRM_WINDOW_WIDTH) * 0.5f ,info.rcWindow.top + (pCurDocument->GetHeight() - CONFIRM_WINDOW_HEIGHT) * 0.5f);
    pConfirmWindow->Show(true);
}

void TextEditorWidget::ReOpenCurrentDocument()
{
    if(pCurDocument->isModified())
        OpenConfirmDialog(SCRIPT_DOCUMENT_CONFIRM_REOPEN_WINDOW_ID);
    else
        ReOpenCurrentDocument(false);
}

void TextEditorWidget::ReOpenCurrentDocument(bool bSaveCurrent)
{
    pConfirmWindow->Show(false);
    if(bSaveCurrent)
        SaveCurrentDocument();
    char sPath[512];
    sPath[0] = 0;
    if (DialogSelectFile(true, sPath, true, "Script file(*.srt)\0*.srt\0"))
    {	
        pCurDocument->SetFile(sPath);
        pCurDocument->LoadFromFile();
        pTabs->SetTabName(nCurrTabIndex,pCurDocument->GetName());
        char sName[512];
        String::Copy(sName,512,pCurDocument->GetName());
        ProjectInfo::Scene* pScene = core_ed.GetSceneInfo(sName);
        if(pScene)
        {
            pCurDocument->SetScene(pScene);
            core_ed.OnSelectSceneTab(pCurDocument->SceneId());
        }
    }
}

void TextEditorWidget::OnClassesChange()
{
    if(pCurDocument)
    {
        pCurDocument->SetSelectedNameSpace(rClassesList.pList->GetText());
        UpdateMembers();
    }

}

void TextEditorWidget::OnMembersChange()
{
    string sClass = rClassesList.pList->GetText();
    if(sClass.empty())return;
    string sMember = rMembersList.pList->GetText();
    if(sMember.empty())return;
    int nIndex = rMembersList.pList->GetCurString();
    if(sClass == "Global")sClass =  string(pCurDocument->GetFileName()) + "_Global";
    if(nIndex < rMembersList.GetSeparator())
    {
        if(!sClass.empty() && !sMember.empty())
        {
            int pos = core->Script()->GetMemberPos(sClass,sMember);
            if(pos >= 0)
                pCurDocument->GoToLine(-1,pos);
        }
    }
    else
    {
        string sName;
        sName.assign(&sMember[0],sMember.find("("));
        int start = -1;unsigned i = 0;
        for(;i < sName.size();++i)
        {
            if(start != -1 && sName[i] != ' ' && sName[i] != '\t')break;
            if(start == -1 && (sName[i] == ' ' || sName[i] == '\t'))start = i;
        }
        if(i < sName.size())
        {
            sName.assign(&sName[i],sName.size() - i);
            int pos = core->Script()->GetFunctionPos(sClass,sName);
            if(pos >= 0)
                pCurDocument->GoToLine(-1,pos);
        }
    }
}


void TextEditorWidget::UpdateMembers()
{
    string sClass = rClassesList.pList->GetText();
    rMembersList.pList->ClearList();
    rMembersList.pList->ClearText();
    rMembersList.Show(false);

    if(!sClass.empty())
    {
        if(sClass == "Global")sClass =  string(pCurDocument->GetFileName()) + "_Global";
        std::vector<string> sNames;
        sNames.clear();
        rMembersList.pList->ClearList();
        rMembersList.pList->ClearText();
        core->Script()->GetFunctions(sNames,sClass,false);
        if(!sNames.empty())
        {
            std::sort(sNames.begin(),sNames.end());
            rMembersList.Show(true);
            HDC hdc = GetDC(rMembersList.pList->GetHandle());
            for(unsigned i = 0; i < sNames.size();++i)
            {
                RECT rLabelRect = {0,0,0,0};
                DrawText(hdc,sNames[i].c_str(), strlen(sNames[i].c_str()), &rLabelRect, DT_CALCRECT);
                if(i == 0  || rLabelRect.right > rMembersList.pList->GetWidth())
                {
                    int nMaxWidth = rMembersList.pList->GetParent()->GetWidth() - rMembersList.pList->GetX() - 30;
                    if(nMaxWidth < rLabelRect.right)
                        rLabelRect.right = nMaxWidth;
                    rMembersList.pList->SetSize(rLabelRect.right,rMembersList.pList->GetHeight());
                }
                rMembersList.pList->AddString(sNames[i].c_str());
            }
        }
    }
    int nAddX = 96;
    if(pLoadButton->GetVisibleState())nAddX = 132;
    rClassesList.SetX(pTabs->GetXOffset() + nAddX);
    if(rMembersList.pList->IsVisible())
       rMembersList.SetX(pTabs->GetXOffset() + nAddX + rClassesList.Width());


}

void TextEditorWidget::ReParseDocuments(float dt, int level)
{
    bInReParseMode = true;
    for(unsigned i = 0;i <vDocuments.size();++i)
    {
        if(vDocuments[i]->isNeedToParse())
        {
            string sText;
            vDocuments[i]->GetAllText(sText);
            core->Script()->ReParseCode(vDocuments[i]->GetFileName(),sText);
            FillNameSpaceBox();
            vDocuments[i]->OnEndReparseFile();
        }
    }
    bInReParseMode = false;
}

void TextEditorWidget::FillNameSpaceBox()
{
    std::vector<string> sNames;
    rClassesList.pList->ClearList();
    core->Script()->GetNameSpaces(sNames,pCurDocument->GetFileName());
    rClassesList.pList->AddString("Global");
    std::sort(sNames.begin(),sNames.end());
    for(unsigned i = 0; i < sNames.size();++i)
        rClassesList.pList->AddString(sNames[i].c_str());

    rClassesList.pList->ClearText();
    if(pCurDocument)
        rClassesList.pList->SetText(pCurDocument->GetSelectedNameSpace().c_str());
    UpdateMembers();
}
bool TextEditorWidget::GetMatch(int nIndex,SearchMatch& result)
{
	unsigned nIterIndex = 0;
	TMatchesMap::Iterator it  = vMatches.Begin();
	while(it != vMatches.End())
	{
		TMatches* pList = *it;
		for(unsigned i =0;i < pList->size();++i)
		{
			if(nIterIndex == nIndex)
			{
				result = pList->at(i);
				return true;
			}
			++nIterIndex;
		}
		++it;
	}
	return false;
}
TMatches*   TextEditorWidget::GetDocumentIterInMatches(ScriptDocumentWidget*  pDocument)
{
	TMatchesMap::Iterator it  = vMatches.Begin();
	while(it != vMatches.End())
	{
		if(pDocument->GetFilePath() == it.GetKey())return *it;
		++it;
	}
	return NULL;

}

bool TextEditorWidget::GoToFindResult(int nIndex,const string& sPath)
{
	if(!sPath.empty())
	{
		SearchMatch result;
		if(GetMatch(nIndex,result))
		{
			if(!pCurDocument || sPath != string(pCurDocument->GetFileName()))
			{
			   core_ed.GoToScript(sPath.c_str(),result.nRow - 1,result.nCol);
			}
			else
			{
				if(pCurDocument)
					pCurDocument->GoTo(result.nRow - 1,result.nCol);
			}
		}
		return true;
	}
	return false;
}
bool TextEditorWidget::GoToScriptError(const string& sText)
{
    int nPos = sText.find("in file:");
    if(nPos != string::npos)
    {
        string sFileName;string sBuffer;unsigned state = 0;int nLine = 0,nCol = 0;
        for(unsigned i = nPos + 9;i <sText.size();++i)
        {
            switch (state)
            {
            case 0:
                {
                    if(sText[i] == ' ')state = 1;
                    else sFileName += sText[i];
                    break;
                }
            case 1:
                {
                    if(sText[i] == ',')
                    {
                        nLine = atoi(sBuffer.c_str());
                        sBuffer.clear();
                        state = 2;
                    }
                    else
                        if(isalnum(sText[i]))sBuffer += sText[i];
                    break;
                }
            case 2:
                {
                    if(sText[i] == ')')
                    {
                        nCol = atoi(sBuffer.c_str());
                        i  = sText.size();
                    }
                    else
                        if(isalnum(sText[i]))sBuffer += sText[i];
                }

            }

        }
        if(!sFileName.empty())
        {
            if(!pCurDocument || sFileName != string(pCurDocument->GetFileName()))
			{
				core_ed.GoToScript(sFileName.c_str(),nLine - 1,nCol);
			}
			else
			{
				if(pCurDocument)
					pCurDocument->GoTo(nLine - 1,nCol);
			}
            return true;
        }
    }
    return false;
}
void TextEditorWidget::UpdateMatches(ScriptDocumentWidget* pDocument,int nPos,int nOffset)
{
	if(pCurDocument)
	{
		TMatches* pList = GetMatches();
		if(pList)
		{
			for(unsigned i = 0;i < pList->size();++i)            
			{
				SearchMatch& rMatcher = pList->at(i);
				pCurDocument->SetTextStyle(rMatcher.nStart,rMatcher.nEnd,rMatcher.vOldStyles);
				if(rMatcher.nStart >= nPos)
					rMatcher += nOffset;
			}
		}
	}
}

void TextEditorWidget::ClearMatches()
{
	for(unsigned i = 0;i <nDocumentsCount;++i)
	{
		TMatches* pList = vMatches.Find(vDocuments[i]->GetFilePath());
		if(pList)
			vDocuments[i]->ClearMatchesSelection(pList);
	}
	vMatches.Clear();
}



