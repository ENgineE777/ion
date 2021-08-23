#pragma once

#include "BaseWidget.h"
#include <scintilla.h>
#include "Editor\Helpers\EditorProject.h"
#include "Editor\Helpers\SearchMatches.h"

class IFileBuffer;
class SceneInstance;
class TextEditorWidget;

#define CUSTOM_STYLE        31
#define CUSTOM_STYLE_SEARCH CUSTOM_STYLE - 1

class DEF_EXPORT ScriptDocumentWidget : public BaseWidget
{
private:
    struct TipsData
    {
    private:
        struct ParametrRange
        {
            int nStart;
            int nEnd;
            ParametrRange():nStart(0),nEnd(0){}
            ParametrRange(int s,int e):nStart(s),nEnd(e){}
        };
        typedef std::vector<ParametrRange> TRanges;
        struct Signature
        {
            string   sSignature;
            TRanges  vParametrs;
            Signature(){}
            Signature(const string& signature);

        };
        struct Comparator 
        {
            bool operator() (const Signature& i,const Signature& j) { return (i.sSignature.size() < j.sSignature.size());}
        }ComparatorObject;


        int                    nPosition;
        std::vector<Signature> vSignatures;
        ScriptDocumentWidget*  pOwner; 
        string                 sTipsMethodSignature;

    public:
        void   SetOwner(ScriptDocumentWidget* owner){pOwner = owner;nPosition = -1;}
        void   AddSignatures(const std::vector<string>& _vSignatures);
        void   BuildTips(unsigned parametrs);
        void   HighLightParametr(int nParametr);
        void   Hide();
        bool   IsVisible(){return pOwner->ScintillaCall(SCI_CALLTIPACTIVE) == 1;}
        void   OnDeleteText(int position);
    };


    struct MultyUndoRedoModifyData
    {
        int nPos;
        int nLine;
        int nOffset;
        int nLinesAdded;
        MultyUndoRedoModifyData():nPos(0),nOffset(0),nLinesAdded(0),nLine(0){}
        inline MultyUndoRedoModifyData* Init(int pos,int line)
        { 
            nLine = line;
            nPos = pos;
            nOffset = nLinesAdded = 0;
            return this;
        }
    };
    MultyUndoRedoModifyData  rMultyUndoRedoModifyData;
    MultyUndoRedoModifyData* pMultyUndoRedoModifyData;

    struct stat  fileInfo;
    char sFilePath[512];
    char sFileName[512];
    char sName[64];
    
    TipsData       signatures;
    SciFnDirect    pSciMsg;
    sptr_t         pSciWndData;
    int            nIndex;  
    bool           bModified;
	bool           bSkipModifiedText;
    bool           bNeedToParse;
    int            eSearchFlags;
	bool           bSckipUpdateMatches;
    

    int            nGotoPos; 
    TextEditorWidget* pOwner;

    ProjectInfo::Scene*   pScene;

    string          sNameSpaceSelected;


    inline LRESULT ScintillaCall(UINT msg, WPARAM wp = 0, LPARAM lp = 0);
    void           SetDocumentText(byte* text, int length);
    void           GetDocumentText(std::string& text);
    bool           ParsePathInLine(bool& bNameSpace,std::vector<string>& vPath,int nMaxCol = -1,const char* sLine = NULL);
    void           ParseCurrentParametrInMethodSignature(int nColumnCorrection  = 0);
    void           GetText(string& sText,int start,int end);
    void           GetFindText(string& sText,bool& bFindInSelection);
	void           GetLine(string& sText,unsigned nPos);
    
    void           GetSelectonPositions(int& start,int& end,int index);
    void           FindAllMatches(TMatches* pMatchesList,const string& sText,int nEndPos);
    bool           IsInclude(const char* sLine,int nLength,string& sFileName); 
    bool           ReLoadFromFile();
    size_t         ReadFileInBuffer();


    static IFileBuffer*   pFileBuffer;    



public:

    struct ColorScheme
    {
        int background;
        int text;
        int number;
        int caret;
        int preprocessor;
        int preprocessor_comment;
        int operators;
        int keywords;
        int types;
        int comment;
        int strings;
        int chars;

        int search_selection_text;
        int search_selection_back;
    };

    ScriptDocumentWidget(int id, BaseWidget* parent, float x, float y, float w, float h,EAutoSizeType sizerType = BaseWidget::eAutoSizeBoth);
    virtual ~ScriptDocumentWidget();

    virtual void  ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);
    virtual float GetYOffset();
    virtual void  Show(bool bSet);
    virtual void  Update();

	void        Goto(const SearchMatch& rMatcher);

    void        ClearText();
    void        SetFile(const char* name);
    bool        LoadFromFile();
    void        SaveToFile(bool bForceSave = false);
    void        SaveToFile(const char* sPath);
    void        GetCursorState(int& line, int& col);
    void        SetColorScheme(ColorScheme* pScheme);
    bool        CanUndo();
    void        Undo();
    bool        CanRedo();
    void        Redo();
    int         Index() const { return nIndex;}
    void        SetIndex(int index){nIndex = index;}
    void        SetScene(ProjectInfo::Scene* scene){pScene = scene;}
    int         SceneId() const {if(pScene)return pScene->Index();return -1;}
    const char* GetFileName() const {return sFileName;}
    const char* GetFilePath() const {return sFilePath;}
    const char* GetName() const {return sName;}
    void        SetOwner(TextEditorWidget* owner){pOwner = owner;}
    void        FindAllMatches(TMatches* pMatchesList,const string& sText,bool bFindInSelection);
    void        SetFindFlags(bool bCase,bool bWord);

    void        Replace(TMatches* pMatchesList,unsigned index,const char* sNewText);

	void        GetTextStyles(int start,int end,std::vector<char>& vStyles);
	void        SetTextStyle(int start,int end,int style);
	void        SetTextStyle(int start,int end,const std::vector<char>& vStyles);
	void        ClearMatchesSelection(TMatches* pMatchesList); 

    bool        isModified() const {return bModified;}
    bool        isNeedToParse() const {return bNeedToParse;}
    void        OnEndReparseFile(){bNeedToParse = false;} 
    void        SetSelectedNameSpace(const string& sClass){sNameSpaceSelected = sClass;}    
    void        GoToLine(int nLine,int nPos = -1);
    void        GoTo(int nLine,int nCol){GoToLine(-1,ScintillaCall(SCI_FINDCOLUMN,nLine,nCol));}
    void        GetAllText(string& sSext);
	LRESULT     GetCurrentPos(){return ScintillaCall(SCI_GETCURRENTPOS);}


    const string&  GetSelectedNameSpace() const {return sNameSpaceSelected;}
    ProjectInfo::Scene*  Scene() const {return pScene;}

};
