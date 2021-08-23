
#include "ScriptDocumentWidget.h"
#include <scilexer.h>
#include <algorithm>
#include <sys/stat.h>

#include "Core/ICore.h"
#include "Core\Scene\SceneInstance.h"
#include "ButtonWidget.h"
#include "TextEditorWidget.h"
#include "DialogWidget.h"
#include "Editor/Editor.h"
#include "Core/Script/ScriptService.h"


const char kAngelScriptKeywords[] = "break case class const cast continue default do else enum import from funcdef for if namespace private protected public return switch this while override final mixin ";
const char kAngelScriptTypes[] = "bool double float int void false true";

static char vLineBuff[2048];


#define PANEL_HEIGTH 26

const char* g_vAutoCompleteExcludeNames[] ={"opAdd","opAddAssign","opAdd_r","opAssign","opCmp","opEquals","opIndex"};

IFileBuffer* ScriptDocumentWidget::pFileBuffer = NULL;

static bool IsAutoCompleteExcludeName(const string& sName)
{
    for(unsigned i =0;i <  7;++i)
        if(sName == g_vAutoCompleteExcludeNames[i])return true;
    return false;
}

ScriptDocumentWidget::ScriptDocumentWidget(int set_id, BaseWidget* prnt, float set_x, float set_y, float w, float h,EAutoSizeType sizerType) : BaseWidget(set_id, prnt, "", sizerType),
    eSearchFlags(0),pOwner(NULL),pScene(NULL),nIndex(-1),bModified(false),bNeedToParse(false),nGotoPos(-1),pMultyUndoRedoModifyData(NULL),bSckipUpdateMatches(false),bSkipModifiedText(false)
{
    x = set_x;
    y = set_y;
    width = w;
    height = h;
    handle = CreateWindow("Scintilla","",WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN | WS_BORDER,(int)x,(int)y + PANEL_HEIGTH,(int)width,(int)height - PANEL_HEIGTH,(HWND)parent->GetHandle(),NULL,NULL,NULL);
    SetWindowLong(handle, GWL_ID, id);

    MakeSubClassing();


    pSciMsg = (SciFnDirect)SendMessage(handle, SCI_GETDIRECTFUNCTION, 0, 0);
    pSciWndData = (sptr_t)SendMessage(handle, SCI_GETDIRECTPOINTER, 0, 0);

    ScintillaCall(SCI_SETLEXER, SCLEX_CPP);
    ScintillaCall(SCI_SETSTYLEBITS, 7);

    ScintillaCall(SCI_SETKEYWORDS, 0, (LPARAM)(kAngelScriptKeywords));
    ScintillaCall(SCI_SETKEYWORDS, 1, (LPARAM)(kAngelScriptTypes));

    ScintillaCall(SCI_SETTABWIDTH, 4);
    ScintillaCall(SCI_SETUSETABS, TRUE);
    ScintillaCall(SCI_SETTABINDENTS, TRUE);
    ScintillaCall(SCI_SETBACKSPACEUNINDENTS, TRUE);
    ScintillaCall(SCI_SETINDENTATIONGUIDES, SC_IV_LOOKBOTH);

    ScintillaCall(SCI_SETMARGINWIDTHN, 0, 0);
    ScintillaCall(SCI_SETMARGINWIDTHN, 1, 0);
    ScintillaCall(SCI_SETMARGINWIDTHN, 2, 0);

    ScintillaCall(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
    ScintillaCall(SCI_SETMARGINTYPEN, 1, SC_MARGIN_SYMBOL);
    ScintillaCall(SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL);

    ScintillaCall(SCI_SETMARGINSENSITIVEN, 0, TRUE);

    int margin_size = ScintillaCall(SCI_TEXTWIDTH, STYLE_LINENUMBER, (LPARAM) "_99999");
    ScintillaCall(SCI_SETMARGINWIDTHN, 0, margin_size);

    ScintillaCall(SCI_STYLESETSIZE, STYLE_DEFAULT, 10);
    ScintillaCall(SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)("Consolas"));
    ScintillaCall(SCI_STYLECLEARALL);

    ScintillaCall(SCI_CALLTIPUSESTYLE,4);
    ScintillaCall(SCI_STYLESETFONT, STYLE_CALLTIP, (LPARAM)("Consolas"));

    ScintillaCall(SCI_SETMODEVENTMASK,(SC_MOD_INSERTTEXT|SC_MOD_DELETETEXT));

    COLORREF background = RGB(0xFF, 0xFF, 0xFF);

    static string separators = " ;{}[]()\t+-,!`~@#$%^&*";
    ScintillaCall(SCI_AUTOCSTOPS,0, (LPARAM)separators.c_str());
    ScintillaCall(SCI_AUTOCSETAUTOHIDE,FALSE);
    ScintillaCall(SCI_AUTOCSETMAXHEIGHT,ScintillaCall(SCI_AUTOCGETMAXHEIGHT) * 3);
    ScintillaCall(SCI_AUTOCSETIGNORECASE,true);

    ScintillaCall(SCI_SETMULTIPLESELECTION,true);
    ScintillaCall(SCI_SETADDITIONALCARETSVISIBLE,false);

    ScintillaCall(SCI_SETCODEPAGE,SC_CP_UTF8);
   // int code  = ScintillaCall(SCI_GETCODEPAGE);

    int nCodeCtrl[21] = {'Q', 'W', 'E', 'R', 'I', 'O', 'P', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'B', 'N', 'M', 186, 187, 226};

    // To disable Ctrl+Char
    for(int i = 0; i < 21; i++)
    {
        nCodeCtrl[i] += (SCMOD_CTRL << 16);
        ScintillaCall(SCI_CLEARCMDKEY, nCodeCtrl[i], 0);
    }

    // To disable Ctrl+Shift+Char of some other characters
    for(int i = 48; i <= 122; i++)
    {
        int nCode = i + ((SCMOD_CTRL|SCMOD_SHIFT) << 16);
        ScintillaCall(SCI_CLEARCMDKEY, nCode, 0);
    }

    sFilePath[0] = 0;
    sFileName[0] = 0;
    signatures.SetOwner(this);
    
}

ScriptDocumentWidget::~ScriptDocumentWidget()
{
    DestroyWindow(handle);
}

bool ScriptDocumentWidget::ParsePathInLine(bool& bNameSpace,std::vector<string>& vPath,int nMaxCol,const char* sLine)
{
    LRESULT pos = ScintillaCall(SCI_GETCURRENTPOS);
    int curLine = ScintillaCall(SCI_LINEFROMPOSITION,pos);
    int col = nMaxCol;
    if(col == -1 )col = ScintillaCall(SCI_GETCOLUMN,pos);
    if(!sLine)
    {
        memset(vLineBuff,0,2048);
        if(ScintillaCall(SCI_LINELENGTH, curLine) < sizeof(vLineBuff))
        {
            ScintillaCall(SCI_GETLINE, curLine, reinterpret_cast<LPARAM>(static_cast<char*>(vLineBuff)));
            sLine = vLineBuff;
        }
    }
    vPath.clear();
    if(sLine && sLine[0])
    {
        int tabW = ScintillaCall(SCI_GETTABWIDTH) - 1;
        string sName;
        bNameSpace = false;
        int start = 0;
        for(int i = 0;i < col;++i)
            if(sLine[i] == '\t')col -= tabW;
        for(int i = col;i >= 0;--i)
        {
            unsigned char ch = sLine[i];
            if(ch == '\r' || ch == '\n')--col;
            else
                break;
        }
        for(int i = col;i >= 0;--i)
        {
            unsigned char ch = sLine[i];
            if(ch == '\t' || ch == ' ' || ch == ';' || (ch == '(' && i < col))
            {
                start = i + 1;
                break;
            }
            else
                 if(ch == ')')
                      while( i >= 0 && sLine[i] != '(')--i;
        }
        for(int i = start ;i <= col;++i)
        {
            unsigned char ch = sLine[i];
            if(ch ==  ':')
            {
                if(vPath.size() == 0)
                {
                    if(i > 0 &&  sLine[i - 1] == ':')
                    {
                        bNameSpace = true;
                        vPath.push_back(sName);
                        sName.clear();
                    }
                }
                else
                    vPath.clear();
            }
            else
            {
                if(!iswalpha(ch) && !iswdigit(ch))
                {
                    if(ch == '(')
                    {
                        sName += ch;
                        bool bFirst = true;
                        ++i;
                        while( i <= col && sLine[i] != ')')
                        {
                            unsigned char ch = sLine[i];
                            if(ch == ',')sName += sLine[i];
                            else
                                if(bFirst && (iswalpha(ch) || iswdigit(ch) || ch == '_'))
                                {
                                    sName += ',';
                                    bFirst = false;
                                }
                            ++i;
                        }
                        sName += ')';
                        if( i + 1 < col && sLine[i + 1] == '[')
                        {
                            ++i;
                            while( i <= col && sLine[i] != ']')++i;
                            if( i <= col && sLine[i] == ']')
                                sName += "[]";
                        }
                        vPath.push_back(sName);
                        sName.clear();
                    }
                    if(ch == '[')
                    {
                        while( i <= col && sLine[i] != ']')++i;
                        if( i <= col && sLine[i] == ']')
                            sName += "[]";
                        vPath.push_back(sName);
                        sName.clear();

                    }
                    else
                        if(ch == '_') sName += ch;
                        else
                            if(!sName.empty())
                            {
                                vPath.push_back(sName);
                                sName.clear();
                            }
                }
                else sName += ch;
            }
        }
        if(!sName.empty())
        {
             vPath.push_back(sName);
             return true;
        }

    }
    return !vPath.empty();

}

void ScriptDocumentWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
    if(!IsVisible())return;
    if(pOwner &&  pOwner->IsReParsing())return;
    if (msg ==  WM_NOTIFY)
    {
        if (handle == ((SCNotification*)lParam)->nmhdr.hwndFrom)
        {
            SCNotification* pNotification = ((SCNotification *)lParam);
            UINT code = pNotification->nmhdr.code;
            switch(code)
            {
            case SCN_MODIFIED:
                {
					if(bSkipModifiedText)return;

					bNeedToParse = bModified = true;

					if(pNotification->modificationType & SC_MULTISTEPUNDOREDO)
					{
						if(pNotification->modificationType & SC_MOD_DELETETEXT)
						{
							int line = ScintillaCall(SCI_LINEFROMPOSITION,ScintillaCall(SCI_GETCURRENTPOS));
							if(!pMultyUndoRedoModifyData)
								pMultyUndoRedoModifyData = rMultyUndoRedoModifyData.Init(pNotification->position,line);
							pMultyUndoRedoModifyData->nOffset -= pNotification->length;
							pMultyUndoRedoModifyData->nLinesAdded += pNotification->linesAdded;
						}
						else
							if(pNotification->modificationType & SC_MOD_INSERTTEXT)
							{
								int line =  ScintillaCall(SCI_LINEFROMPOSITION,ScintillaCall(SCI_GETCURRENTPOS));
								if(!pMultyUndoRedoModifyData)
									pMultyUndoRedoModifyData = rMultyUndoRedoModifyData.Init(pNotification->position,line);
								pMultyUndoRedoModifyData->nOffset += pNotification->length;
								pMultyUndoRedoModifyData->nLinesAdded += pNotification->linesAdded;
							}

							if(pNotification->modificationType & SC_LASTSTEPINUNDOREDO)
							{
								if(pMultyUndoRedoModifyData)
								{
									core->Script()->OnTextMofified(GetFileName(),pMultyUndoRedoModifyData->nLine,pMultyUndoRedoModifyData->nLinesAdded,pMultyUndoRedoModifyData->nPos,pMultyUndoRedoModifyData->nOffset);
									pOwner->UpdateMatches(this,pMultyUndoRedoModifyData->nPos,pMultyUndoRedoModifyData->nOffset);
									pMultyUndoRedoModifyData = NULL;
								}
							}

					}
					else
					{
						if(pNotification->modificationType & SC_MOD_DELETETEXT)
						{
							if(signatures.IsVisible())
								signatures.OnDeleteText(pNotification->position);
							int line = ScintillaCall(SCI_LINEFROMPOSITION,ScintillaCall(SCI_GETCURRENTPOS));
							core->Script()->OnTextMofified(GetFileName(),line,pNotification->linesAdded,pNotification->position,-pNotification->length);
						}
						else
						{
							if(pNotification->modificationType & SC_MOD_INSERTTEXT)
							{
								int line =  ScintillaCall(SCI_LINEFROMPOSITION,ScintillaCall(SCI_GETCURRENTPOS));
								core->Script()->OnTextMofified(GetFileName(),line,pNotification->linesAdded,pNotification->position,pNotification->length);
							}
						}
						if(!bSckipUpdateMatches)
							pOwner->UpdateMatches(this,pNotification->position,pNotification->length);

					}
					pOwner->OnModified();

                    break;
                }
            case SCN_CHARADDED:
                {
                    char ch = pNotification->ch; 
                    if(ch == '.' || ch == ':')
                    {
                        memset(vLineBuff,0,2048);
                        int curLine = ScintillaCall(SCI_LINEFROMPOSITION,ScintillaCall(SCI_GETCURRENTPOS));
                        if(ScintillaCall(SCI_LINELENGTH, curLine) < sizeof(vLineBuff))
                            ScintillaCall(SCI_GETLINE, curLine, reinterpret_cast<LPARAM>(static_cast<char*>(vLineBuff)));
                        if(ch == ':' && string(vLineBuff).find("::") == string::npos)return;
                        bool bNameSpace = false;
                        std::vector<string> vPath;
                        if(ScintillaCall(SCI_AUTOCACTIVE))
                            ScintillaCall(SCI_AUTOCCANCEL);
                        if(ParsePathInLine(bNameSpace,vPath,ScintillaCall(SCI_GETCOLUMN,ScintillaCall(SCI_GETCURRENTPOS)) - 1,vLineBuff))
                        {
                            std::vector<string> vMembers; 
                            int curLine = ScintillaCall(SCI_LINEFROMPOSITION,ScintillaCall(SCI_GETCURRENTPOS));
                            core_ed.GetMembers(curLine,GetFileName(),vPath,vMembers,bNameSpace);
                            if(vMembers.size() > 0)
                            {
                                std::vector<string> vFinalList; 
                                static HashTable<string,bool> vMemory = HashTable<string,bool>(128);
                                vMemory.Clear();
                                for(unsigned i = 0;i < vMembers.size();++i)
                                {
                                    if(IsAutoCompleteExcludeName(vMembers[i]) ||  vMemory.Find(vMembers[i]))continue;
                                    vFinalList.push_back(vMembers[i]);
                                    vMemory[vMembers[i]] = true;
                                }
                                std::sort(vFinalList.begin(), vFinalList.end());
                                string vList;
                                for(unsigned i = 0;i < vFinalList.size();++i)
                                {
                                    vList += vFinalList[i];
                                    if(i + 1 <  vFinalList.size())
                                        vList += " ";
                                }
                                ScintillaCall(SCI_AUTOCSHOW,0,(LPARAM)vList.c_str());
                            }
                        }
                    }
                    else
                    {
                        if(ScintillaCall(SCI_AUTOCACTIVE))
                        {
                            memset(vLineBuff,0,2048);
                            int curLine = ScintillaCall(SCI_LINEFROMPOSITION,ScintillaCall(SCI_GETCURRENTPOS));
                            int lineLength = ScintillaCall(SCI_LINELENGTH, curLine);
                            if  (lineLength < sizeof(vLineBuff) && lineLength > 4)
                            {
                                ScintillaCall(SCI_GETLINE, curLine, reinterpret_cast<LPARAM>(static_cast<char*>(vLineBuff)));
                                int i = lineLength - 2;
                                for(;i > 0;--i)
                                {
                                    unsigned char ch = vLineBuff[i - 1];
                                    if(ch == '.' || ch == ':')
                                        break;
                                }
                                unsigned len = lineLength - 2 - i;
                                if(len > 0)
                                {
                                    vLineBuff[lineLength - 2] = '\0';
                                    ScintillaCall(SCI_AUTOCSELECT,0,(LPARAM)(vLineBuff + i));
                                }
                            }
                        }
                        else
                        {
                            switch(ch)
                            {
                            case ',':if(signatures.IsVisible())ParseCurrentParametrInMethodSignature();break;
                            case ')':if(signatures.IsVisible())signatures.Hide();break;
                            case '(':
                                {
                                    bool bNameSpace = false;
                                    std::vector<string> vPath;
                                    if(ParsePathInLine(bNameSpace,vPath,ScintillaCall(SCI_GETCOLUMN,ScintillaCall(SCI_GETCURRENTPOS)) - 1))
                                    {
                                        vPath.back().erase(vPath.back().end() - 2,vPath.back().end());
                                        std::vector<string> vSignatures;
                                        int curLine = ScintillaCall(SCI_LINEFROMPOSITION,ScintillaCall(SCI_GETCURRENTPOS));
                                        core_ed.GetSignature(curLine,GetFileName(),vPath,vSignatures,bNameSpace);
                                        signatures.AddSignatures(vSignatures);
                                        ParseCurrentParametrInMethodSignature();
                                    }
                                    break;
                                }
                            case '\n':
                                {
                                    int pos = ScintillaCall(SCI_GETCURRENTPOS);
                                    int line =  ScintillaCall(SCI_LINEFROMPOSITION,pos);
                                    if(line > 0)
                                    {
                                        memset(vLineBuff,0,2048);
                                        unsigned offset = ScintillaCall(SCI_GETLINEINDENTATION,line - 1);
                                        ScintillaCall(SCI_GETLINE, line - 1, reinterpret_cast<LPARAM>(static_cast<char*>(vLineBuff)));
                                        int i = ScintillaCall(SCI_LINELENGTH, line - 1);
                                        for(;i >= 0;--i)
                                            if(vLineBuff[i] == '{')break;
                                        if(i >= 0)
                                        {
                                            int nLines = ScintillaCall(SCI_GETLINECOUNT);
                                            int  nOpen = 0,nClosed = 0;
                                            line = 0;
                                            while(line < nLines)
                                            {
                                                memset(vLineBuff,0,2048);
                                                ScintillaCall(SCI_GETLINE,line, reinterpret_cast<LPARAM>(static_cast<char*>(vLineBuff)));
                                                int nLength = ScintillaCall(SCI_LINELENGTH, line);
                                                for(int i = 0; i < nLength;++i)
                                                {
                                                    if(vLineBuff[i] == '{')++nOpen;
                                                    else
                                                        if(vLineBuff[i] == '}')++nClosed;
                                                }
                                                ++line;
                                            }
                                            if(nOpen != nClosed)
                                            {
                                                string indent,str;
                                                int _pos = pos;
                                                int tabW = ScintillaCall(SCI_GETTABWIDTH);
                                                for(unsigned i = 0;i < offset;++i)indent +=' ';
                                                str = indent;
                                                for(int i = 0;i < tabW;++i)str += ' ';
                                                str += "\r\n";
                                                ScintillaCall(SCI_INSERTTEXT,_pos,(LPARAM)str.c_str());
                                                _pos += str.length();
                                                str.clear();
                                                str = indent + "}\r\n";
                                                ScintillaCall(SCI_INSERTTEXT,_pos,(LPARAM)str.c_str());
                                                offset += tabW;
                                            }
                                            else
                                            {
                                                string str;
                                                offset += ScintillaCall(SCI_GETTABWIDTH);
                                                for(unsigned i = 0;i < offset;++i)str+=' ';
                                                ScintillaCall(SCI_INSERTTEXT,pos,(LPARAM)str.c_str());
                                            }
                                        }
                                        else
                                        {
                                            string str;
                                            for(unsigned i = 0;i < offset;++i)str+=' ';
                                            ScintillaCall(SCI_INSERTTEXT,pos,(LPARAM)str.c_str());
                                        }
                                        ScintillaCall(SCI_GOTOPOS,pos + offset);
                                        //core->Script()->OnCharAdded(GetFileName(),line,pos,offset);
                                    }
                                    break;
                                }
                            }
                        }
                    }


                    break;
                }
            case SCN_MARGINCLICK:
                {
                    ScintillaCall(SCI_LINEFROMPOSITION, pNotification->position);
                    break;
                }
            }
        }
    }
    else
        if(msg == WM_KEYDOWN)
        {

            if(signatures.IsVisible() && (wParam == VK_NUMPAD4  || wParam == VK_NUMPAD6 || wParam == VK_LEFT || wParam == VK_RIGHT))
            {
                int add = 0;
                if(wParam == VK_NUMPAD4 ||  wParam == VK_LEFT )add = -1;
                else
                    if(wParam == VK_NUMPAD6 ||  wParam == VK_RIGHT )add = 1;
                ParseCurrentParametrInMethodSignature(add);
            }
            else
                if(wParam == 0x47   && (GetKeyState(VK_CONTROL) == -127 || GetKeyState(VK_CONTROL) == -128))
                {
                    WINDOWINFO info;
                    GetWindowInfo(handle,&info);
                    LRESULT pos = ScintillaCall(SCI_GETCURRENTPOS);
                    pOwner->ShowGoToDialog(this,info.rcWindow.left + (GetWidth() - GOTO_WINDOW_WIDTH) * 0.5f ,info.rcWindow.top + (GetHeight() - GOTO_WINDOW_HEIGHT) * 0.5f,ScintillaCall(SCI_LINEFROMPOSITION,pos) + 1,ScintillaCall(SCI_GETLINECOUNT));

                }
                else
				{
                    if((wParam == 0x46 || wParam == 0x48) && (GetKeyState(VK_CONTROL) == -127 || GetKeyState(VK_CONTROL) == -128))
                    {
						
                        WINDOWINFO info;
                        GetWindowInfo(handle,&info);
                        bool bFindInSelection;string sText;
                        GetFindText(sText,bFindInSelection);

						TextEditorWidget::ESearchType eType = TextEditorWidget::eFindInDocument;
						if(GetKeyState(VK_SHIFT) == -127 || GetKeyState(VK_SHIFT) == -128)
						{
							eType = TextEditorWidget::eFindInProject;
						}
						else
						{
							if(bFindInSelection)
								eType = TextEditorWidget::eFindInSelection;
						}
                        if(wParam == 0x46)
                            pOwner->ShowSearchDialog(this,(float)info.rcClient.right  - SEARCH_WINDOW_WIDTH ,(float)info.rcClient.top,sText.c_str(),eType);
                        else
                            pOwner->ShowSearchAndReplaceDialog(this,(float)info.rcClient.right - SEARCH_WINDOW_WIDTH,(float)info.rcClient.top,sText.c_str(),eType);

                    }
				}
        }
        else
           if(msg == DIALOG_OK_PRESS)
           {
               switch(wParam)
               {
                    case SCRIPT_DOCUMENT_GOTO_WINDOW_ID:
                        {
                            const char * sNumber = pOwner->GetGoToDialogText();
                            if(strlen(sNumber) > 0)
                                GoToLine(atoi(sNumber) - 1);
                            break;
                        }
                    case SCRIPT_DOCUMENT_CONFIRM_REOPEN_WINDOW_ID:pOwner->ReOpenCurrentDocument(true);break;
                    case SCRIPT_DOCUMENT_CONFIRM_CLOSE_WINDOW_ID:pOwner->CloseScript(true);break;
               }
           }
           else
               if(msg == DIALOG_CANSEL_PRESS)
               {
                   switch(wParam)
                   {
                        case SCRIPT_DOCUMENT_SEARCH_WINDOW_ID:
                            {
                                SetFocus(GetHandle());
                                break;
                            }
                        case SCRIPT_DOCUMENT_CONFIRM_REOPEN_WINDOW_ID: pOwner->ReOpenCurrentDocument(false);break;
                        case SCRIPT_DOCUMENT_CONFIRM_CLOSE_WINDOW_ID:pOwner->CloseScript(false);break;
                   }
               }
               else
                   if(msg == WM_LBUTTONUP)
                   {
                       if((GetKeyState(VK_CONTROL) == -127 || GetKeyState(VK_CONTROL) == -128))
                       {
                           memset(vLineBuff,0,2048);
                           int nCurLine = ScintillaCall(SCI_LINEFROMPOSITION,ScintillaCall(SCI_GETCURRENTPOS));
                           int nLineLength = ScintillaCall(SCI_LINELENGTH, nCurLine);
                           if(nLineLength < sizeof(vLineBuff))
                           {
                               ScintillaCall(SCI_GETLINE, nCurLine, reinterpret_cast<LPARAM>(static_cast<char*>(vLineBuff)));
                               string sFileName;
                               if(!IsInclude(vLineBuff,nLineLength,sFileName))
                               {
                                   int nCol = ScintillaCall(SCI_GETCOLUMN, ScintillaCall(SCI_GETCURRENTPOS));
                                   int nTabW = ScintillaCall(SCI_GETTABWIDTH) - 1;
                                   int nRealCol = nCol;

                                   for(int i = 0;i < nCol;++i)
                                       if( vLineBuff[i] == '\t')
                                           nRealCol -= nTabW;

                                   while(nRealCol < nLineLength)
                                   {
                                       char ch = vLineBuff[nRealCol];
                                       if(ch == '(')
                                       {
                                           while( nRealCol < nLineLength && vLineBuff[nRealCol] != ')')++nRealCol;
                                           break;
                                       }
                                       else
                                           if(iswalpha(ch) || iswdigit(ch) || ch == '_')++nRealCol;
                                           else
                                           {
                                               --nRealCol;
                                               break;
                                           }
                                   }

                                   for(int i = 0;i < nCol;++i)
                                       if( vLineBuff[i] == '\t')nRealCol += nTabW;

                                   
                                   bool bNameSpace = false;std::vector<string> vPath;
                                   if(ParsePathInLine(bNameSpace,vPath,nRealCol))
                                   {
                                      string sFile;
                                      int pos = core->Script()->FindPosition(nCurLine,GetFileName(),vPath,bNameSpace,sFile);
                                      if(pos != -1)
                                      {
                                          if(sFile == this->sFileName)nGotoPos = pos;
                                          else
											core_ed.GoToScript(sFile.c_str(),0);
                                      }
                                   }
                                }
                           }
                       }
                   }

}

void ScriptDocumentWidget::ClearText()
{
	bSkipModifiedText = true;
    ScintillaCall(SCI_ANNOTATIONCLEARALL);
    ScintillaCall(SCI_ANNOTATIONSETVISIBLE, ANNOTATION_HIDDEN);
    ScintillaCall(SCI_CLEARALL);
	bSkipModifiedText = false;
}

void ScriptDocumentWidget::SetFile(const char* _sName)
{
    int nNameLength = strlen(_sName);
    if(nNameLength)
    {
        string sProjectPath =  core->GetProjectPath();
        string sFile = _sName;
        String::FixSlashes(&sProjectPath[0]);
        String::LowerCase(sProjectPath);
        String::FixSlashes(&sFile[0]);
        String::LowerCase(sFile);

        String::Copy(sFilePath, 512, _sName);
        
        if(sProjectPath[0] != sFile[0])
        {
            String::ExtractFileName(_sName,sFileName);
            String::RemoveExctention(sFileName);
            String::Copy(sName, 64, sFileName);
        }
        else
        {
            memset(sFileName,0,512);
            unsigned i = 0;
            for(;i < sFile.size();++i)
                if(sProjectPath[i] != sFile[i])break;
            unsigned nSlahes = 0;
            if(i > 0)
            {
                for(int j = i;j < sProjectPath.size();++j)
                    if(sProjectPath[j] == '/')++nSlahes;
                if(nSlahes > 0)
                    for(unsigned i = 0; i < nSlahes;++i)
                        String::Cat(sFileName,512,"../");
            }
            String::Cat(sFileName,512,&sFile[i]);
            String::ExtractFileName(sFileName,sName);
            String::RemoveExctention(sName);
            String::RemoveExctention(sFileName);
        }
      

    }
    else
        sFilePath[0] = sFileName[0] = 0; 
}

size_t ScriptDocumentWidget::ReadFileInBuffer()
{
    if (!sFilePath[0])
	{
        return 0;
	}

	RELEASE(pFileBuffer)
	pFileBuffer = core->Files()->LoadFile(sFilePath);

	if (pFileBuffer)
	{
		return pFileBuffer->GetSize();    
    }

    return 0;
}
bool ScriptDocumentWidget::ReLoadFromFile()
{
    size_t size = ReadFileInBuffer();
    if(size)
    {
        ClearText();
        bModified = false;
        bNeedToParse = false;
        SetDocumentText(pFileBuffer->GetBuffer(), size);
        ScintillaCall(SCI_EMPTYUNDOBUFFER);
        stat(sFilePath,&fileInfo);
        return true;
    }
    return false;
}

bool ScriptDocumentWidget::LoadFromFile()
{
    ClearText();
    size_t size = ReadFileInBuffer();
    if(size)
    {
		bSkipModifiedText = true;
        SetDocumentText(pFileBuffer->GetBuffer(),size);
		ScintillaCall(SCI_EMPTYUNDOBUFFER);
		bSkipModifiedText = false;
    }
	else 
		return false;

	bModified = false;
	bNeedToParse = false;
    stat(sFilePath,&fileInfo);
	return true;
}

void ScriptDocumentWidget::SaveToFile(const char* sPath)
{	
    if (sFilePath[0] && bModified)
    {
        LRESULT len = ScintillaCall(SCI_GETLENGTH, null, null);
        byte* buffer = NULL;
        if(len != 0)
        {
			len++;
            buffer = new byte[len];
            ScintillaCall(SCI_GETTEXT, len, (LPARAM)buffer);
        }

		IFile* file = core->Files()->OpenFile(sPath, IFile::write); 

		if (file && len > 0)
		{
			file->Write(buffer, len - 1);
			RELEASE(file)
		}

        delete[] buffer;
    }
}

void ScriptDocumentWidget::SaveToFile(bool bForceSave/* = false*/)
{
  if(bForceSave)
	  bModified = true;
  SaveToFile(sFilePath);
  stat(sFilePath,&fileInfo);
}

void ScriptDocumentWidget::GetCursorState(int& line, int& col)
{
    LRESULT pos = ScintillaCall(SCI_GETCURRENTPOS);
    line =  ScintillaCall(SCI_LINEFROMPOSITION,pos) + 1;
    col = ScintillaCall(SCI_GETCOLUMN,pos);
}

void ScriptDocumentWidget::Update()
{
    if(nGotoPos != -1)
    {
        GoToLine(-1,nGotoPos);
        nGotoPos = -1;
    }
    struct stat flInfo;
    if (!stat(sFilePath,&flInfo))
    {        
        if (fileInfo.st_mtime != flInfo.st_mtime || fileInfo.st_atime != flInfo.st_atime || fileInfo.st_ctime != flInfo.st_ctime)
        {
            if(ReLoadFromFile())
                pOwner->ReParseDocuments(0,0);
        }
    }
}

LRESULT ScriptDocumentWidget::ScintillaCall(UINT msg, WPARAM wp /*= 0*/, LPARAM lp /*= 0*/)
{
    return pSciMsg(pSciWndData, msg, wp, lp);
}

void ScriptDocumentWidget::SetDocumentText(byte* text, int length)
{
    ScintillaCall(SCI_SETTEXT, 0, (LPARAM)text);
}

void ScriptDocumentWidget::GetDocumentText(std::string& text)
{
    LRESULT length = ScintillaCall(SCI_GETLENGTH, null, null) + 1;

    char* buffer = new char[length];
    ScintillaCall(SCI_GETTEXT, length, (LPARAM)buffer);

    text.assign(buffer);

    delete [] buffer;
}

void ScriptDocumentWidget::ParseCurrentParametrInMethodSignature(int nColumnCorrection)
{
    memset(vLineBuff,0,2048);
    LRESULT pos = ScintillaCall(SCI_GETCURRENTPOS);
    int curLine = ScintillaCall(SCI_LINEFROMPOSITION,pos);
    int col = ScintillaCall(SCI_GETCOLUMN,pos) + nColumnCorrection;
    int lineLength = ScintillaCall(SCI_LINELENGTH, curLine);
    if  (lineLength < sizeof(vLineBuff))
    {
        ScintillaCall(SCI_GETLINE, curLine, reinterpret_cast<LPARAM>(static_cast<char*>(vLineBuff)));
        int tabW = ScintillaCall(SCI_GETTABWIDTH) - 1;
        for(int i = 0;i < col;++i)
            if(vLineBuff[i] == '\t')col -= tabW;
        unsigned mode = 0; unsigned parametr = 1;
        int start = 0;
        for(int i = col;i >= 0;--i)
        {
            unsigned char ch = vLineBuff[i];
            if(ch == '\t' || ch == ' ' || ch == ';')
            {
                start = i + 1;
                break;
            }
        }
        for(int i = start ;i < col;++i)
        {
            unsigned char ch = vLineBuff[i];
            if(ch ==  '(')mode = 1;
            else
                if(ch ==  ',' && mode == 1)++parametr;
                else
                    if(ch ==  ')')break;
        }
        signatures.HighLightParametr(parametr);
    }
}

void ScriptDocumentWidget::SetColorScheme(ColorScheme* pScheme)
{
    if(pScheme)
    {

        
        ScintillaCall(SCI_STYLESETFORE, STYLE_DEFAULT, pScheme->text);
        ScintillaCall(SCI_STYLESETBACK, STYLE_DEFAULT, pScheme->background);
        ScintillaCall(SCI_STYLECLEARALL);


        ScintillaCall(SCI_SETCARETFORE, pScheme->caret);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_NUMBER, pScheme->number);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_NUMBER, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_PREPROCESSOR, pScheme->preprocessor);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_PREPROCESSOR, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_PREPROCESSORCOMMENT, pScheme->preprocessor_comment);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_PREPROCESSORCOMMENT, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_OPERATOR,pScheme->operators);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_OPERATOR, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_WORD,pScheme->keywords);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_WORD, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_WORD2, pScheme->types);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_WORD2, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_COMMENT, pScheme->comment);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_COMMENT, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_COMMENTLINE, pScheme->comment);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_COMMENTLINE, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_STRING, pScheme->strings);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_STRING, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_STRINGRAW, pScheme->strings);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_STRINGRAW, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_STRINGEOL, pScheme->strings);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_STRINGEOL, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, SCE_C_CHARACTER, pScheme->chars);
        ScintillaCall(SCI_STYLESETBACK, SCE_C_CHARACTER, pScheme->background);


        ScintillaCall(SCI_STYLESETFORE, STYLE_LINENUMBER, pScheme->text);
        ScintillaCall(SCI_STYLESETBACK, STYLE_LINENUMBER, pScheme->background);

        ScintillaCall(SCI_STYLESETFORE, STYLE_CALLTIP,pScheme->text);
        ScintillaCall(SCI_STYLESETBACK, STYLE_CALLTIP,pScheme->background);

        ScintillaCall(SCI_CALLTIPSETFOREHLT,pScheme->types);

        ScintillaCall(SCI_STYLESETFORE, CUSTOM_STYLE_SEARCH, pScheme->search_selection_text);
        ScintillaCall(SCI_STYLESETBACK, CUSTOM_STYLE_SEARCH, pScheme->search_selection_back);
    }

}

void ScriptDocumentWidget::Undo()
{
    if(ScintillaCall(SCI_CANUNDO))
        ScintillaCall(SCI_UNDO);
}

void ScriptDocumentWidget::Redo()
{
    if(ScintillaCall(SCI_CANREDO))
        ScintillaCall(SCI_REDO);
}

bool ScriptDocumentWidget::CanUndo()
{
    if(ScintillaCall(SCI_CANUNDO))return true;
    return false;
}

bool ScriptDocumentWidget::CanRedo()
{
    if(ScintillaCall(SCI_CANREDO))return true;
    return false;
}

void ScriptDocumentWidget::GetLine(string& sText,unsigned nPos)
{
	 unsigned nStart = nPos;
	 unsigned nEnd = nPos;
	 unsigned nSize = ScintillaCall(SCI_GETLENGTH);
	 char c = 0; 
	 do 
	 {
		 c = (char)ScintillaCall(SCI_GETCHARAT,nStart--);
	 }
	 while(c != '\n' && c != '\r' && nStart != 0);
	 do 
	 {
		 c = (char)ScintillaCall(SCI_GETCHARAT,nEnd++);
	 }
	 while(c != '\n' && c != '\r' && nEnd != nSize);
	 GetText(sText,nStart + 2,nEnd - 1);
}

void ScriptDocumentWidget::GetFindText(string& sText,bool& bFindInSelection)
{
    bFindInSelection = false;
    sText.clear();
    if(ScintillaCall(SCI_GETSELECTIONS) == 1)
    {
        int start = ScintillaCall(SCI_GETSELECTIONNANCHOR,0);
        int end = ScintillaCall(SCI_GETSELECTIONNCARET,0);
        if(start == end)
        {
            int pos = ScintillaCall(SCI_GETCURRENTPOS);
            int start = ScintillaCall(SCI_WORDSTARTPOSITION,pos,true);
            int end = ScintillaCall(SCI_WORDENDPOSITION,pos,true);
            if(start == end)
            {
                int iter = start - 1;char c = 0; 
                do 
                {
                    c = (char)ScintillaCall(SCI_GETCHARAT,iter--);
                }
                while(isalpha(c) || c == isdigit(c));
                sText = c;
            }
            GetText(sText,start,end);
        }
        else
            if(ScintillaCall(SCI_LINEFROMPOSITION,start) == ScintillaCall(SCI_LINEFROMPOSITION,end))GetText(sText,start,end);
            else
                bFindInSelection = true;
    }
    else 
        bFindInSelection = true;
}

void ScriptDocumentWidget::GetText(string& sText,int start,int end)
{
    Sci_TextRange range;
    range.chrg.cpMin = start;
    range.chrg.cpMax = end;
    unsigned size = end - start + 1;
    range.lpstrText = (char*)malloc(size * sizeof(char));
    memset(range.lpstrText,0,size * sizeof(char));
    ScintillaCall(SCI_GETTEXTRANGE,0,(LPARAM)&range);
    sText = range.lpstrText;
    free(range.lpstrText);
}

void ScriptDocumentWidget::SetTextStyle(int start,int end,int style)
{
    int t = ScintillaCall(SCI_GETENDSTYLED);
    ScintillaCall(SCI_STARTSTYLING,start,CUSTOM_STYLE);
    ScintillaCall(SCI_SETSTYLING,end-start,style);
    ScintillaCall( SCI_STARTSTYLING, t, CUSTOM_STYLE);
    ScintillaCall( SCI_SETSTYLING, 0, 0 );
}

void ScriptDocumentWidget::SetTextStyle(int start,int end,const std::vector<char>& vStyles)
{
    int t = ScintillaCall(SCI_GETENDSTYLED);
    ScintillaCall(SCI_STARTSTYLING,start,CUSTOM_STYLE);
    ScintillaCall(SCI_SETSTYLINGEX,end - start,(LPARAM)&vStyles[0]);
    ScintillaCall( SCI_STARTSTYLING, t, CUSTOM_STYLE);
    ScintillaCall( SCI_SETSTYLING, 0, 0 );
}


void ScriptDocumentWidget::GetTextStyles(int start,int end,std::vector<char>& vStyles)
{
     for(int i = start;i <= end;++i) 
          vStyles.push_back((char)ScintillaCall(SCI_GETSTYLEAT,i));
}


void ScriptDocumentWidget::FindAllMatches(TMatches*  pMatchesList,const string& sText,bool bFindInSelection)
{
    if(sText.empty())return;
    ScintillaCall(SCI_SETSEARCHFLAGS,eSearchFlags);
    if(bFindInSelection)
    {
       if(ScintillaCall(SCI_GETSELECTIONS) == 1)
       {
           ScintillaCall(SCI_TARGETFROMSELECTION);
           FindAllMatches(pMatchesList,sText,ScintillaCall(SCI_GETTARGETEND));
       }
       else
       {
           int count = ScintillaCall(SCI_GETSELECTIONS);
           std::vector< Sci_CharacterRange > vRanges;
           vRanges.resize(count);
           for(int i = 0 ;i < count;++i)
           {
               int start,end;
               GetSelectonPositions(start,end,i);
               vRanges[i].cpMin = start;
               vRanges[i].cpMax = end;
           }
           for(unsigned i = 0 ;i < vRanges.size();++i)
           {
               ScintillaCall(SCI_SETTARGETSTART,vRanges[i].cpMin);
               ScintillaCall(SCI_SETTARGETEND,vRanges[i].cpMax);
               FindAllMatches(pMatchesList,sText,vRanges[i].cpMax);
           }
       }
    }
    else
    {
       ScintillaCall(SCI_TARGETWHOLEDOCUMENT);
       FindAllMatches(pMatchesList,sText,ScintillaCall(SCI_GETTARGETEND));
    }
    InvalidateRect(handle,NULL,false);
}

void ScriptDocumentWidget::FindAllMatches(TMatches* pMatchesList,const string& sText,int nEndPos)
{
    int result = -1;
    do 
    {
        result = ScintillaCall(SCI_SEARCHINTARGET,sText.size(),(LPARAM)sText.c_str());
        if(result != -1)
        {
			int nTargetEnd = ScintillaCall(SCI_GETTARGETEND);
			int nLine =  ScintillaCall(SCI_LINEFROMPOSITION,nTargetEnd) + 1;
			int nCol = ScintillaCall(SCI_GETCOLUMN,nTargetEnd);
			string sLine;
			GetLine(sLine,nTargetEnd);
            pMatchesList->push_back(SearchMatch(ScintillaCall(SCI_GETTARGETSTART),nTargetEnd,nLine,nCol,sLine));
            SearchMatch& rMatcher = pMatchesList->back();
            int count = ScintillaCall(SCI_GETSELECTIONS);
            int index  = -1;
            for(int i = 0 ;i < count;++i)
            {
                int start,end;
                GetSelectonPositions(start,end,i);
                if(start == end)continue;
                if((rMatcher.nStart >= start  && rMatcher.nStart <= end) || (rMatcher.nEnd >= start  && rMatcher.nEnd <= end))
                {
                    index = i;
                    break;
                }
            }
            if(index != -1)
            {
                int start,end;
                GetSelectonPositions(start,end,index);
                rMatcher.vCutSelection[0] = start;
                rMatcher.vCutSelection[1] = end;
                if(count == 1)
                {
                    rMatcher.vSelectionIndexs[0] = count;
                    rMatcher.vSelectionIndexs[1] = count + 1;
                    ScintillaCall(SCI_CLEARSELECTIONS);
                }
                else
                {
                    rMatcher.vSelectionIndexs[0] = count - 1;
                    rMatcher.vSelectionIndexs[1] = count;
                    ScintillaCall(SCI_DROPSELECTIONN,index);
                    for(unsigned i = 0;i < pMatchesList->size() - 1;++i)
                    {
						SearchMatch& rMatcher = pMatchesList->at(i);
                        if(rMatcher.vSelectionIndexs[0] == index)
                        {
                            rMatcher.vSelectionIndexs[0] = -1;
                            break;
                        }
                        if(rMatcher.vSelectionIndexs[1] == index)
                        {
                            rMatcher.vSelectionIndexs[1] = -1;
                            break;
                        }
                    }
                }
                ScintillaCall(SCI_ADDSELECTION,rMatcher.nStart,start);
                ScintillaCall(SCI_ADDSELECTION,end,rMatcher.nEnd);
            }
            GetTextStyles(rMatcher.nStart,rMatcher.nEnd,rMatcher.vOldStyles);
            SetTextStyle(rMatcher.nStart,rMatcher.nEnd,CUSTOM_STYLE_SEARCH);
            ScintillaCall(SCI_SETTARGETRANGE,rMatcher.nEnd + 1,nEndPos);
        }
    } 
    while (result != -1);
}

void ScriptDocumentWidget::GetSelectonPositions(int& start,int& end,int index)
{
    int s = ScintillaCall(SCI_GETSELECTIONNANCHOR,index);
    int e = ScintillaCall(SCI_GETSELECTIONNCARET,index);
    if(e < s)
    {
        start = e;
        end = s;
    }
    else
    {
        start = s;
        end = e;
    }
}
void ScriptDocumentWidget::ClearMatchesSelection(TMatches* pMatchesList)
{
	if(pMatchesList)
	{
		std::vector<int*> vOldSelections;
		for(unsigned i = 0;i < pMatchesList->size();++i)
		{
			SearchMatch& rMatcher = pMatchesList->at(i);
			if(rMatcher.vSelectionIndexs[0] != -1)
			{
				if(ScintillaCall(SCI_GETSELECTIONS) == 1 && rMatcher.vSelectionIndexs[0] == 0)ScintillaCall(SCI_CLEARSELECTIONS);
				else
				{
					ScintillaCall(SCI_DROPSELECTIONN,rMatcher.vSelectionIndexs[0]);
					if(rMatcher.vSelectionIndexs[1] != -1)--rMatcher.vSelectionIndexs[1];
				}
				for(unsigned j = i + 1;j < pMatchesList->size();++j)
				{
					SearchMatch& rMatcher = pMatchesList->at(j);
					if(rMatcher.vSelectionIndexs[0] != -1)--rMatcher.vSelectionIndexs[0];
					if(rMatcher.vSelectionIndexs[1] != -1)--rMatcher.vSelectionIndexs[1];
				}
			}
			if(rMatcher.vSelectionIndexs[1] != -1)
			{
				if(ScintillaCall(SCI_GETSELECTIONS) == 1 && rMatcher.vSelectionIndexs[1] == 0)ScintillaCall(SCI_CLEARSELECTIONS);
				else ScintillaCall(SCI_DROPSELECTIONN,rMatcher.vSelectionIndexs[1]);
				for(unsigned j = i + 1;j < pMatchesList->size();++j)
				{
					SearchMatch& rMatcher = pMatchesList->at(j);
					if(rMatcher.vSelectionIndexs[0] != -1)--rMatcher.vSelectionIndexs[0];
					if(rMatcher.vSelectionIndexs[1] != -1)--rMatcher.vSelectionIndexs[1];
				}
			}
			SetTextStyle(rMatcher.nStart,rMatcher.nEnd,rMatcher.vOldStyles);
			if(rMatcher.vCutSelection[0] != -1)
			{
				if(vOldSelections.empty())vOldSelections.push_back(rMatcher.vCutSelection);
				else
				{
					for(unsigned j = 0;j < vOldSelections.size();++j)
					{
						int start = vOldSelections[j][0];
						int end = vOldSelections[j][1];
						if(rMatcher.vCutSelection[0] > end || rMatcher.vCutSelection[1] < start)vOldSelections.push_back(rMatcher.vCutSelection);
						else
						{
							if(rMatcher.vCutSelection[0] < start && rMatcher.vCutSelection[1] > start)
							{
								vOldSelections[j][0] = rMatcher.vCutSelection[0];
								if(rMatcher.vCutSelection[1] > end)vOldSelections[j][1] = rMatcher.vCutSelection[1]; 
							}
							else
								if(rMatcher.vCutSelection[0] < end && rMatcher.vCutSelection[1] > end)
									vOldSelections[j][1] = rMatcher.vCutSelection[1];
						}
					}
				}
			}
		}
		for(unsigned i = 0;i < vOldSelections.size();++i)
		{
			if(ScintillaCall(SCI_GETSELECTIONS) == 1 && ScintillaCall(SCI_GETSELECTIONEMPTY))
				ScintillaCall(SCI_SETSELECTION,vOldSelections[i][1],vOldSelections[i][0]);
			else
				ScintillaCall(SCI_ADDSELECTION,vOldSelections[i][1],vOldSelections[i][0]);
		}
	}
}

void ScriptDocumentWidget::SetFindFlags(bool bCase,bool bWord)
{
    eSearchFlags = 0;
    if(bCase)eSearchFlags = SCFIND_MATCHCASE;
    if(bWord)eSearchFlags |= SCFIND_WHOLEWORD;
}

void ScriptDocumentWidget::Goto(const SearchMatch& rMatcher)
{
    SetFocus(handle);
    int t = ScintillaCall(SCI_GETENDSTYLED);
    ScintillaCall(SCI_GOTOPOS,rMatcher.nEnd);
    ScintillaCall(SCI_STARTSTYLING,rMatcher.nStart,CUSTOM_STYLE);
    ScintillaCall(SCI_SETSTYLING,rMatcher.nEnd-rMatcher.nStart,CUSTOM_STYLE_SEARCH);
    ScintillaCall( SCI_STARTSTYLING, t, CUSTOM_STYLE);
    ScintillaCall( SCI_SETSTYLING, 0, 0 );
    
}

void ScriptDocumentWidget::Replace(TMatches* pMatchesList,unsigned index,const char* sNewText)
{
    if(index < (int)pMatchesList->size())
    {
        SearchMatch& rMatcher = pMatchesList->at(index);
        int dRanges = strlen(sNewText) - (rMatcher.nEnd - rMatcher.nStart);
		SetTextStyle(rMatcher.nStart,rMatcher.nEnd,rMatcher.vOldStyles);
		ScintillaCall(SCI_SETTARGETSTART,rMatcher.nStart);
		ScintillaCall(SCI_SETTARGETEND,rMatcher.nEnd);
		bSckipUpdateMatches = true;
        ScintillaCall(SCI_REPLACETARGET,strlen(sNewText),(LPARAM)sNewText);
		bSckipUpdateMatches = false;
		rMatcher.nEnd += dRanges;
        for(unsigned i = index + 1;i < pMatchesList->size();++i)
        {
            SearchMatch& rMatcher = pMatchesList->at(i);
            rMatcher.nStart += dRanges;
            rMatcher.nEnd += dRanges;
        }
    }
}

void ScriptDocumentWidget::Show(bool bSet)
{
    BaseWidget::Show(bSet);
}

void ScriptDocumentWidget::GoToLine(int nLine,int nPos)
{
    if(nLine == -1 && nPos == -1)return;
    if(nLine == -1)
        nLine = ScintillaCall(SCI_LINEFROMPOSITION,nPos);
    SetFocus(GetHandle());
    int nCurLine = ScintillaCall(SCI_LINEFROMPOSITION,ScintillaCall(SCI_GETCURRENTPOS));
    MathClip(nLine,ScintillaCall(SCI_GETLINECOUNT),0);
    int nLinesOffsetPlus = 0;
    if( nCurLine > nLine)
    {
        if(nLine - (int)(ScintillaCall(SCI_LINESONSCREEN) * 0.5f) > 0)
            nLinesOffsetPlus = nLine - (int)(ScintillaCall(SCI_LINESONSCREEN) * 0.5f);
    }
    else
        if(nCurLine < nLine)
            nLinesOffsetPlus = nLine + (int)(ScintillaCall(SCI_LINESONSCREEN) * 0.5f);
        else
        {
            if(nPos == -1)
                ScintillaCall(SCI_GOTOPOS,ScintillaCall(SCI_POSITIONFROMLINE,nLine));
            else 
                ScintillaCall(SCI_GOTOPOS,nPos);
            return;
        }
    MathClip(nLinesOffsetPlus,ScintillaCall(SCI_GETLINECOUNT),0);
    ScintillaCall(SCI_GOTOLINE,nLinesOffsetPlus);
    if(nPos == -1)
        ScintillaCall(SCI_GOTOPOS,ScintillaCall(SCI_POSITIONFROMLINE,nLine));
    else 
        ScintillaCall(SCI_GOTOPOS,nPos);
}

void ScriptDocumentWidget::GetAllText(string& sSext)
{
    LRESULT len = ScintillaCall(SCI_GETLENGTH, null, null) + 1;
    char* buffer = new char[len];
    ScintillaCall(SCI_GETTEXT, len, (LPARAM)buffer);
    sSext = buffer;
    delete[] buffer;
}

bool ScriptDocumentWidget::IsInclude(const char* sLine,int nLength,string& sFileName)
{
    for(int i =0 ;i < nLength;++i)
    {
        if(sLine[i] == '#' && i + 8 < nLength )
        {
            string word;
            word.assign(&sLine[i + 1],7);
            if(word == "include")
            {
               int j = i + 8;
               while(j < nLength && (sLine[j] == ' ' || sLine[j] == '\t'))++j;
               word.clear();
               while(j < nLength && sLine[j] != ' ' && sLine[j] != '\t' &&  sLine[j] != '\r' && sLine[j] != '\n')
               {
                   if(sLine[j] != '"')
                       word += sLine[j];
                   ++j;
               }
               if(!word.empty())
               {
				   core_ed.GoToScript(word.c_str(),0);
                   return true;
               }
               else return false;
            }
        }
    }
    return false;
}

float ScriptDocumentWidget::GetYOffset()
{
    return  -PANEL_HEIGTH;
}



ScriptDocumentWidget::TipsData::Signature::Signature(const string& signature):sSignature(signature)
{
    unsigned start = 0;
    for(unsigned i = 0;i < sSignature.size();++i)
    {
        if(sSignature[i] == '(')start = i + 1;
        else
            if(sSignature[i] == ',' || sSignature[i] == ')')
            {
                if(i > start)
                    vParametrs.push_back(ParametrRange(start,i));
                start = i + 1;
            }
    }
}

void  ScriptDocumentWidget::TipsData::BuildTips(unsigned parametrs)
{
    sTipsMethodSignature.clear();
    for(unsigned i = 0; i < vSignatures.size();++i)
    {
        if(!vSignatures[i].vParametrs.empty())
        {
            if(parametrs > vSignatures[i].vParametrs.size())continue;
        }
        else
            if(parametrs > 1)continue;
        sTipsMethodSignature += vSignatures[i].sSignature;
        if(i + 1 < vSignatures.size())
            sTipsMethodSignature += "\n";
    }
    if(!sTipsMethodSignature.empty())
    {
        if(nPosition == -1)
            nPosition = pOwner->ScintillaCall(SCI_GETCURRENTPOS);
        pOwner->ScintillaCall(SCI_CALLTIPSHOW,nPosition,(LPARAM)sTipsMethodSignature.c_str());
    }
}

void ScriptDocumentWidget::TipsData::HighLightParametr(int nParametr)
{
     BuildTips(nParametr);
     for(unsigned i = 0; i < vSignatures.size();++i)
     {
          if(nParametr - 1 < (int)vSignatures[i].vParametrs.size())
          {
               ParametrRange range =  vSignatures[i].vParametrs[nParametr - 1];
               pOwner->ScintillaCall(SCI_CALLTIPSETHLT,range.nStart,range.nEnd);
               return;
          }
     }
//     Hide();
}

void ScriptDocumentWidget::TipsData::AddSignatures(const std::vector<string>& _vSignatures)
{
    nPosition =  -1;
    vSignatures.clear();
    for(unsigned i =0;i < _vSignatures.size();++i)
            vSignatures.push_back(Signature(_vSignatures[i]));
    std::sort(vSignatures.begin(),vSignatures.end(),ComparatorObject);
}

void ScriptDocumentWidget::TipsData::Hide()
{
   nPosition = -1;
   pOwner->ScintillaCall(SCI_CALLTIPCANCEL);
}

void ScriptDocumentWidget::TipsData::OnDeleteText(int position)
{
    if(nPosition != -1 && position > nPosition - 1)
        pOwner->ParseCurrentParametrInMethodSignature();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

