#pragma once
#include "Common/Color.h"
#include "Common/common.h"
#include "TextureEntry.h"

#define TOKEN_LIBRARY	 							       \
    TextProcessing::RegistryItem vTokenRegistryTable[] =   \
    {

#define REGISTER_TOKEN(name,class)				           \
    { name, class::GetInstance },

#define TOKEN_LIBRARY_END  								   \
};

class NGUIWidget;
class FontInstance;

namespace TextProcessing
{
    class Cursor;
    class RichText;
    typedef std::vector<FontRes::LineData> TLinesData;

    class TextUtils
    {
    public:
        static int   GetCharPositionInLine(FontInstance* pFont,const string& text,float& pos_x,int start_pos,int end_pos,float cur_pos_x,float scale);
        static float GetTextWidthFrom(FontInstance* pFont,const string& text,int start_pos,int index,bool bMultiLine,float scale);
        static void  GetWordBorders(FontInstance* pFont,TLinesData* line_indexes,const string& text,const Cursor& cursor,int& start,int& end,float minX,float maxX,float scale);
        static void  GetWordBorders(const string& text,int index,int& start,int& end);
    };
    

    class Menu
    {
    public:
        typedef std::vector<unsigned>     TMenuIds;
        typedef std::vector<std::string>  TMenuNames;

        static TMenuNames vEditMenuText;
        static TMenuNames vLabelMenuText;

        static TextureInstance*     pMenuAnchorTexture;
        static TextureInstance*     pMenuArrowRTexture;
        static TextureInstance*     pMenuArrowLTexture;
        static TextureInstance*     pLineTexture;
        static FontInstance*        pMenuFont;
        static TextureEntry::Slice slice;

        static void CreateFont();

        static Menu*  GetInstance(); 
        static void   Release();

    private:
        static Menu*  pInstance;

        struct SMenuPage
        {
            unsigned nMin;
            unsigned nMax;
            SMenuPage():nMax(0),nMin(0){} 
            SMenuPage(unsigned min,unsigned max):nMax(max),nMin(min){} 
        };
        enum EMenuControlsId{eUser,eMenuLeftArrow,eMenuRightArrow};
        struct SMenuEelement
        {
            unsigned            eId;
            ClipRect            rect;
            float               nWidth;
            EMenuControlsId     eSystemId;
            string              sName; 
            SMenuEelement(unsigned id,const string& name,float x,float w,float h):eId(id),nWidth(w),eSystemId(eUser),sName(name)
            {
                rect.x1 = x;
                rect.y1 = 0;
                rect.x2 = rect.x1  + w;
                rect.y2 = h;
            }
            SMenuEelement(float x,float w,float h,EMenuControlsId id):eId(-1),nWidth(w),eSystemId(id)
            {
                rect.x1 = x;
                rect.y1 = 0;
                rect.x2 = rect.x1  + w;
                rect.y2 = h;
            }
            SMenuEelement& operator += (float offsetX){rect.x1 += offsetX;rect.x2 += offsetX;return *this;}
        };
        typedef   std::vector<SMenuEelement>  TMenu;
        typedef   std::vector<SMenuPage>      TMenuPages;

        TMenu            menu; 
        TMenu            currentMenu; 
        TMenuPages       menuPages;  
        Matrix           menuTextTransform;
        float            nWidth;
        float            nHeigth;
        float            nLineWidth;
        float            nX;
        float            nY;
        float            nAnchorX;
        float            nAnchorW;
        float            nAnchorH;
        float            nArrowW;
        float            nArrowH;
        unsigned         nCurrentPage;         
        bool             bShow;

        bool BuildCurrentMenu(const TMenuIds& ids,const TMenuNames& menuText,float cursorStartX,float cursorEndX,float y);
        void CalculateSize();
        void CalculateX(float cursorStartX,float cursorEndX,NGUIWidget* pParent);
        void CalculateX(float cursorX,NGUIWidget* pParent){CalculateX(cursorX,cursorX,pParent);}

    public:

        Menu():nCurrentPage(0),nX(0),nY(0),nWidth(0),nHeigth(0),nAnchorX(0),nAnchorW(0),nAnchorH(0),nArrowH(0),nArrowW(0),nLineWidth(0),bShow(false)
        {}
        void Reset(){currentMenu.clear();}
        void Render(float alpha,bool bClearTouchRegion,NGUIWidget* pParent);
        void AddTouchRegion();
        void ShowMenuPage(unsigned index);
        int  CheckPress(float localX,float localY,bool& bSuccess);
        bool IsHitted(float local_ms_pos_x, float local_ms_pos_y);
        void Create(const TMenuIds& ids,const TMenuNames& menuText,float cursorStartX,float cursorEndX,float y,NGUIWidget* pParent);
        void Create(const TMenuIds& ids,const TMenuNames& menuText,float cursorX,float y,NGUIWidget* pParent);
        void CalculatePosition(float cursorX,float y,NGUIWidget* pParent);
        void CalculatePosition(float cursorStartX,float cursorEndX,float y,NGUIWidget* pParent);
        void SetY(float y,float nParentY);
    };

    class Cursor
    {
    public:
        enum ECursorAlign{ eCursorCenter,eCursorBottom,eCursorLeft,eCursorRight,eCursorTop};

        static float nWidth;
        static float nHeigth;
        static TextureInstance* pTexture;
        static TextureInstance* pCarretTexture;

    private:
        float nCurretAlpha;
        float nAlpha;
        float nFadeTime;
        float nShowTime;
        float nFadeTimer;
        float nX,nY;

        bool  bShow;
        bool  bRender;
        int   nIndex;
        int   eAlign;
        int   nLine;
        bool  bAnimatedCurret;
        bool  bRenderWithCurret;
        float nDraggetOffsetY;
        float nDraggetOffsetX;
        float scale;
        Color color;

        Cursor*   pLeftBorder;
        Cursor*   pRightBorder;

        NGUIWidget* pParent;
        bool        bMenuShow; 
    public:
        Cursor():pParent(null),nX(0),nY(0),bShow(false),nFadeTimer(0),bAnimatedCurret(true),nLine(0),pLeftBorder(null),nAlpha(0),bRender(false)
            ,nFadeTime(1.0f),nShowTime(5.f),nCurretAlpha(0),nIndex(-1),eAlign(eCursorCenter),nDraggetOffsetY(0),nDraggetOffsetX(0),pRightBorder(null),bRenderWithCurret(false),bMenuShow(false)
        {}
        void    Init(const Color& _color,float _scale,int align,bool animatedCurret,NGUIWidget* parent,bool renderWithCurret = false);
        void    SetColor(const Color& _color){color = _color;}
        void    SetPosition(float x,float y,int index,int line,bool bCompareLineWithBorder = false);
        void    SetX(float x){nX = x;}
        void    SetY(float y){nY = y;}
        void    Show(bool bSet);
        void    Hide(){ bShow = false;nAlpha = 0;bMenuShow = false;}
        void    StopRender(){bRender = false;}
        void    StartRender(){bRender = true;}
        bool    IsRender() const {return bRender;}
        bool    IsOn() const;
        bool    IsShow() const {return bShow;}
        float   GetAlpha() const { return nAlpha;}
        float   GetY() const {return nY;} 
        float   GetDraggetOffsetY() const{return nDraggetOffsetY;}
        float   GetDraggetOffsetX() const{return nDraggetOffsetX;}
        float   GetX() const {return nX;} 
        int     Index() const {return nIndex;}
        void    SetIndex(int index )  {nIndex = index;}
        int     Line() const {return nLine;}
        bool    IsIn(float x,float y,bool bCalcDraggOffsets = false); 
        void    SetTexture(const char* sName);
        void    Render(FontInstance* pFont,float nScale,float offsetX,float offsetY,bool bClearTouchRegion);
        void    Render(float offsetX,float offsetY,bool bClearTouchRegion);
        void    Reset();
        void    ResetFadeTimer(){if(nFadeTimer < 0)nFadeTimer = 0;}
        void    CalculatePosition(const string& text,const std::vector<FontRes::LineData>& lines,FontInstance* font);
        void    CalculatePosition(const string& text,FontInstance* font,float y);
        void    SetLeftBorder(Cursor* pCursor){pLeftBorder = pCursor;}
        void    SetRightBorder(Cursor* pCursor){pRightBorder = pCursor;}
        bool    FindPosition(float x,float y,FontInstance* pFont,TLinesData* line_indexes,const string& text,float scale);

        void    CreateMenu(const Menu::TMenuIds& ids,const Menu::TMenuNames& menuText,float offsetX  = 0,float offsetY = 0);
        int     CheckMenuPress(float localX,float localY,bool& bSuccess);
        void    HideMenu(){ bMenuShow = false;}
        int     GetAlign() const {return  eAlign;}
        Cursor& operator -= (int index){nIndex -= index;return *this;}
        Cursor& operator += (int index){nIndex += index;return *this;}
    };

    class Selection
    {
    private:
        NGUIWidget*      pParent;
        bool             bShowMenu;
        Color            color;
        float            scale; 
    public:
        enum{eSelectionCursorStart,eSelectionCursorEnd};
        Cursor       cursors[2];

        static TextureInstance* pTexture;	

        Selection():pParent(null),bShowMenu(false)
        {}
        void Init(NGUIWidget* parent,const Color& _color,float _scale);
        void SetCursorsColor(const Color& _color);
        void SetColor(const Color& _color){color = _color;}
        void Set(float startX,float endX,int start,int end,int line,float lineY);
        void Render(float offsetX,float offsetY,float scale);
        void Render(const RichText* pText);
        void RenderCursors(float offsetX,float offsetY);
        void Hide();
        void HideMenu(){bShowMenu = false;}
        void ShowMenu(float offsetX = 0,float offsetY = 0);
        void CreateMenu(const Menu::TMenuIds& ids,const Menu::TMenuNames& menuText,float offsetX  = 0,float offsetY = 0);
        bool IsShow() const {return cursors[Selection::eSelectionCursorStart].IsShow();}
        bool IsOn() const {return cursors[Selection::eSelectionCursorStart].IsOn();}
        int  CheckMenuPress(float localX,float localY,bool& bSuccess);
        bool IsHitted(float local_ms_pos_x, float local_ms_pos_y,float offsetX = 0,float offsetY = 0);
        bool Select(FontInstance* pFont,TLinesData* line_indexes,const string& text,const Cursor& curret,int start, int end,float scale);
    };

    struct StaticData
    {
        static void Init();
        static void Release();
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class IToken;
    typedef std::vector<IToken*>    TParserStack;
    class ParserState
    {
    public: 
        typedef std::vector<Color>         TColorTable;
        typedef std::vector<TParserStack>  TFontTable;

        TParserStack   vTokens;
        TColorTable    vColors;
        TFontTable     vFonts;

        void AddFontToTable(const TParserStack& font){vFonts.push_back(font);}
        void AddColorToTable(const Color& color){vColors.push_back(color);}
        void GetColorFromTable(Color& color,unsigned index) const;
        void GetColor(Color& color,unsigned& index,const TParserStack& tokens) const;
        void GetColor(Color& color,unsigned& index) const {GetColor(color,index,vTokens);}
        void GetFontParams(const IToken* pToken,FontParams& desc) const;
        void GetFontParams(FontParams& desc,unsigned& index) const;

        void Clear();
    };

    class IToken
    {
    public:
        enum ETokenID{ eUndefine,eText,eColorTable,eFontTable,eTextColor,eTab,eShadow,eTextShadowColor,eFontItalic,eFontBold,eFontSize,eFontGap,eFontName,eScale,eFontFromTable,eAlignLeft,eAlignRight,eAlignCenter,eNextLine,eLink,ePushColor,ePopColor,eTextAlpha,eTextShadowAlpha};
    protected:
        static  void ParseTokenName(const char** pIter,string& sCmd);
        static  bool ParseData(const char** pIter,float& data);
        static  bool ParseData(const char** pIter,int& data);
        static  bool ParseData(const char** pIter,string& data);
    public:
        static  IToken* ParseToken(const char** pIter,ParserState* pState);

        virtual IToken::ETokenID  Id() const = 0;

        virtual bool      ParseData(const char** pIter,ParserState* pState);
        virtual void      GetColor(Color& color,const ParserState* pState = NULL) const {}

        virtual ~IToken(){}
    };

    typedef IToken * (* TTokenInstanceFunction)();
   

    struct RegistryItem
    {
        string            sName; 
        TTokenInstanceFunction getInstanceFunction;    
    };

    class Parser
    {
        static ParserState result;
        static HashMap<TextProcessing::RegistryItem>  vTokensTable;
    public: 
        static  void                Init();
        static  void                Clear(){result.Clear();}
        static  const ParserState&  Parse(const char* pText);
        static  IToken*             GetToken(const char* sName);
    };


    template<typename T,const IToken::ETokenID ID>
    class Token: public IToken
    {
    public:
        static IToken * GetInstance()
        {
            T * instance = new T;
            return static_cast<IToken*>(instance);
        }
        virtual IToken::ETokenID Id() const {return ID;}
    };
   

    template<typename Type,const IToken::ETokenID ID,typename DataType>
    class TokenWithData : public Token<Type,ID>
    {
    protected:
        DataType data;
    public:
        virtual bool ParseData(const char** pIter,ParserState* pState){return IToken::ParseData(pIter,data);}
        virtual const DataType& GetData() const {return data;}
    };

    template<typename Type,const IToken::ETokenID ID>
    class TokenSwitcher : public Token<Type,ID>
    {
    protected:
        int data;
    public:
        TokenSwitcher():data(1){}
        virtual bool ParseData(const char** pIter,ParserState* pState)
        {
            data = 1;
            IToken::ParseData(pIter,data);
            return true;
        }
        virtual  bool  IsOn() const {return data != 0;}
    };


    class ColorTable : public Token<ColorTable,IToken::eColorTable>
    {
    public:
        virtual bool ParseData(const char** pIter,ParserState* pState);
    };

    class FontTable : public Token<FontTable,IToken::eFontTable>
    {
    public:
        virtual bool ParseData(const char** pIter,ParserState* pState);
    };

    class PushColor : public Token<PushColor,IToken::ePushColor>
    {};

    class PopColor : public Token<PopColor,IToken::ePopColor>
    {};

    class AlignLeft : public Token<AlignLeft,IToken::eAlignLeft>
    {};

    class AlignRight : public Token<AlignRight,IToken::eAlignRight>
    {};

    class AlignCenter : public Token<AlignCenter,IToken::eAlignCenter>
    {};

    class NextLine : public Token<NextLine,IToken::eNextLine>
    {};

    class TextToken : public TokenWithData<TextToken,IToken::eText,string>
    {
    public:
        TextToken(const string& text){data = text;}
        virtual bool ParseData(const char** pIter,ParserState* pState){return false;}
    };

    class Link : public Token<Link,IToken::eLink>
    {
        string sCallBack;
        int    nIndex; 
    public:
        Link():nIndex(-1){}
        virtual bool ParseData(const char** pIter,ParserState* pState);
        void GetLink(string& callback,int& index) const {callback = sCallBack;index = nIndex;}
    };

    class Tab : public TokenWithData<Tab,IToken::eTab,int>
    {
    public:
        virtual bool ParseData(const char** pIter,ParserState* pState);
    };

    class FontFromTable : public TokenWithData<FontFromTable,IToken::eFontFromTable,int>
    {};
    

    class FontName : public TokenWithData<FontName,IToken::eFontName,string>
    {};

    class FontItalic : public TokenSwitcher<FontItalic,IToken::eFontItalic>
    {};

    class FontBold: public TokenSwitcher<FontBold,IToken::eFontBold>
    {};

    class FontSize: public TokenWithData<FontSize,IToken::eFontSize,float>
    {};

    class FontGap: public TokenWithData<FontGap,IToken::eFontGap,float>
    {};

    class Scale: public TokenWithData<Scale,IToken::eScale,float>
    {};

    class RedColor : public TokenWithData<RedColor,IToken::eTextColor,float>
    {
    public:
        virtual  void  GetColor(Color& color,const ParserState* pState = NULL)const {color.r = data / 255.f;}
    };
    class GreenColor : public TokenWithData<GreenColor,IToken::eTextColor,float>
    {
    public:
        virtual  void  GetColor(Color& color,const ParserState* pState = NULL)const {color.g = data / 255.f;}
    };
    class BlueColor :public TokenWithData<BlueColor,IToken::eTextColor,float>
    {
    public:
        virtual  void  GetColor(Color& color,const ParserState* pState = NULL)const {color.b = data / 255.f;}
    };
    class AlphaColor :public TokenWithData<AlphaColor,IToken::eTextAlpha,float>
    {
    public:
        virtual  void  GetColor(Color& color,const ParserState* pState = NULL)const {color.a *= data;}
    };
    class TableColor :public TokenWithData<TableColor,IToken::eTextColor,int>
    {
    public:
        virtual  void  GetColor(Color& color,const ParserState* pState = NULL) const {if(pState)pState->GetColorFromTable(color,static_cast<unsigned>(data));}
    };


    class Shadow : public TokenSwitcher<Shadow,IToken::eShadow>
    {};

    class ShadowRedColor : public TokenWithData<ShadowRedColor,IToken::eTextShadowColor,float>
    {
    public:
        virtual  void  GetColor(Color& color,const ParserState* pState = NULL)const {color.r = data / 255.f;}
    };
    class ShadowGreenColor : public TokenWithData<ShadowGreenColor,IToken::eTextShadowColor,float>
    {
    public:
        virtual  void  GetColor(Color& color,const ParserState* pState = NULL)const {color.g = data / 255.f;}
    };
    class ShadowBlueColor :public TokenWithData<ShadowBlueColor,IToken::eTextShadowColor,float>
    {
    public:
        virtual  void  GetColor(Color& color,const ParserState* pState = NULL)const {color.b = data / 255.f;}
    };
    class ShadowAlphaColor :public TokenWithData<ShadowAlphaColor,IToken::eTextShadowAlpha,float>
    {
    public:
        virtual  void  GetColor(Color& color,const ParserState* pState = NULL)const {color.a *= data;}
    };
    class ShadowTableColor :public TokenWithData<ShadowTableColor,IToken::eTextShadowColor,int>
    {
    public:
        virtual  void  GetColor(Color& color,const ParserState* pState = NULL)const {if(pState)pState->GetColorFromTable(color,static_cast<unsigned>(data));}
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    class RichText
    {
    public:
        struct TextDescription
        {
        public:
            TextAlign       eAlign;
            float           nScale;
            FontParams      font;
            string          sLink;
            int             nLinkIndex;

            TextDescription():nScale(1),eAlign(text_left),nLinkIndex(-1){}
            TextDescription(const FontParams& desc,float scale,TextAlign align) :font(desc),nScale(scale),eAlign(align)
            {}
            TextDescription(const TextDescription& desc):font(desc.font),nScale(desc.nScale),eAlign(desc.eAlign),sLink(desc.sLink),nLinkIndex(desc.nLinkIndex)
            {}
        };
        class TextElement
        {
        public:
            string          sText;
            float           x,y,width;
            TextDescription descriptor; 

            TextElement(const TextDescription& desc,const string& text);
            void Render(NGUIWidget* pOwner,float alpha,bool bAddEndPoints,float nStartY = 0) const;
        private:
            void Render(NGUIWidget* pOwner,const string& sText,float alpha,float nStartY = 0) const;
        };
        struct TextLine
        {
            std::vector<TextElement>    vElements;
            float                       nWidth;
            float                       nHeigth;
            float                       nGap;
            float                       nY;
            TextLine():nWidth(0),nHeigth(0),nY(0),nGap(0){}
            TextLine(float h):nWidth(0),nHeigth(h),nY(0),nGap(0){}
        };

        typedef std::vector<TextLine>            TText;
        typedef std::vector<FontParams>          TFontTable;

    private:
        TText                       vText;
        TFontTable                  vFontTable;
        HashMap<FontInstance>       vFonts;
        std::vector<TextElement*>   vLinks;  

        float                   nMaxWidth;

        FontInstance* GetFont(const FontParams& desc);
        void          AddText(float& y,const TextDescription& descriptor,const string& text);
        void          CalculateLinesY(float& y);
        TextLine*     AddText(TextLine* pLine,float& y,const TextDescription& descriptor,string& text);
		TextLine*     AddLine(TextLine* pLine,float& y,const TextDescription& descriptor,string& text);
        void          RenderCursor(Cursor& cursor);
        void          CutSpacesAtStart(string& sText);
        TextLine*     NextLine(float& y);

        static  bool  IsSeparator(const char  symbol);

#define AlphaDomainsNextLevelCount 22
#define AlphaDomainsCount          25

        struct AlphaDomains
        {
            char     cAlpha;
            char     vNextAlpha[AlphaDomainsNextLevelCount];
        };
        static const AlphaDomains   vAlphaDomains[AlphaDomainsCount];
        static const string         vDomains[];
        static const unsigned       nDomainsCount;

    public:
        RichText():vFonts(_FL_){}
        void   Set(const char* pText,const FontParams& initDesc,float scale,float width);
        void   Set(const char* pText,const FontParams& initDesc,float scale,float width,const string& sLinksCallback,const Color& linksColor);
        void   Clear();
        void   Render(NGUIWidget* pOwner,int nMaxLines,float nAlpha,float nStartY = 0);
        void   ChangeFont(const FontInstance* pOld,FontInstance* pNew);
        void   CheckPress(Vector point,string& sCallBack,int& index,string& sLink);
        float  GetTextWidth();	
        float  GetTextHeight(int nMaxLines);	
        int    GetTextLinesCount(){return (int)vText.size();}
        void   GetWordBorders(const Cursor& cursor,int& start,int& end);
        bool   Select(int nLine,int start, int end,Selection& selection);
        bool   FindPosition(float x,float y,Cursor* pCursor);
        void   RenderCursor(Selection& selection);
        void   RenderSelection(Selection& selection);
        void   CopyText(Selection& selection);

        const TText& GetText() const {return vText;}
    };



}





