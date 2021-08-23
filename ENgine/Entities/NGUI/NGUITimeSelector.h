#pragma once
#include "NGUIWidget.h"

#ifdef EDITOR
#include "Platforms/PC/GUI/ComboBoxWidget.h"
extern void EditorFontEnumerator(ComboBoxWidget* widget);
extern void EditorGUITexrtureEnumerator(ComboBoxWidget* widget);
extern void StartEditBackGroundTexture(PropertiesOwner* owner);
extern void StartEditSelectedBackGroundTexture(PropertiesOwner* owner);
#endif


#define AUTO_SELECT_SPEED 3


class NGUITimeSelector : public NGUIWidget
{
    enum EScrollType{eScrollNone,eScrollHour,eScrollMinute,eScrollAmPm,eScrollDate,eTimeLineScroller,eScrollCount};

    class Scroller
    {
    protected:
        float           nVelocity;
        float           nTimeAccum;
        float           nOffsetAccum;
        float           nOffset;
        float           nTarget;
        bool            bAuto;
        float           nLimitMin;
        float           nLimitMax;
        float           nWidth; 
        float           nX;
        bool            bNeedAuto;
        bool            bAutoSelect;
        bool            bIsDraged;
        unsigned        nSelectedIndex;
        NGUITimeSelector* pOwner; 
    public:


        Scroller(bool _Auto):bAuto(_Auto),bIsDraged(false),bNeedAuto(false),nWidth(0),nX(0),bAutoSelect(false),
            nVelocity(0),nTimeAccum(0),nOffsetAccum(0),nOffset(0),nTarget(0),nLimitMin(0),nLimitMax(0),nSelectedIndex(0)
        {}
        virtual void  Step(float dt);
        virtual void  Step(float dt,float delta);
        virtual void  Draw(NGUITimeSelector* pOwner) const = 0;
        virtual bool  AutoSelect(float dt)  = 0;
        virtual void  Init();  
        virtual void  Init(NGUITimeSelector* owner);  
        virtual bool  CanScroll(float delta){return true;}

        template <class TData >   const TData* CalcalateVelocityForAutoSelect(const std::vector<TData>& list);


        bool  IsIn(float scr_pos);
        void  SetLimits(float min,float max){nLimitMax = max;nLimitMin = min;}
        float Heigth()const {return pOwner->nElementHeigth;}
        float Width()const {return nWidth;}
        float LimitMin()const {return nLimitMin;}
        float LimitMax()const {return nLimitMax;}
        bool  NeedAuto()const{return bNeedAuto;}
        bool  IsDraged()const{return bIsDraged;}
        float GetX()const{return nX;}
        void  StartDrag(){bIsDraged = true;bAutoSelect = bNeedAuto = false;}
        int   SelectedIndex() const {return nSelectedIndex;}
        void  SetPosSize(float x,float width){nX = x;nWidth = width;}
    protected:
        virtual void Scroll(float delta) = 0;
    };

    struct  Interval
    {
        int         nMin;
        int         nMax;

        Interval(int min,int max):nMax(max),nMin(min){}
    };
    typedef std::vector<Interval> TIntervals;

    class TimeScroller: public Scroller
    {
    protected:
        typedef std::vector<int> TTimeData;
        typedef std::vector<TTimeData> TTimes;
        class Element
        {
        public:
            int    nData;
            float  nPos;
            Element(float pos):nData(-1000),nPos(pos){}
            Element(int data,float pos):nData(data),nPos(pos){}
            Element():nData(-1000),nPos(-1){}
        };

        EScrollType eType;
        int         nTextWidth;
        int         nStep;
        TTimes      vTimes; 
        unsigned    nCurList; 
        TIntervals  vIntervals;    
        int         nMax; 
        bool        bCheckTime;

        const Element*   pAutoSelectElement;

        virtual unsigned    GetIndex(int value);
        virtual void        ResetData(unsigned index);
        virtual void        ResetPositions();
        virtual void        BuildTimeData(int min,int max,TTimes& vTimes);
        virtual void        CopyTail(TTimeData& dest,const TTimeData& sourse); 
        virtual void        OnStopScroll(int time){}

    public:
        typedef std::vector<Element> TDataList;
        TDataList vData;

        TimeScroller():Scroller(true),nTextWidth(0),nStep(1),pAutoSelectElement(NULL),nCurList(0),nMax(0),bCheckTime(true)
        {}
        virtual void  Init(int min,int max,int step,EScrollType type);
        virtual void  Init(NGUITimeSelector* owner);
        virtual void  ClearTimeIntervals(){vIntervals.clear();}
        virtual void  AddTimeInterval(int min,int max);
        virtual void  SetMinMax(int min,int max);
        virtual void  SetListIndex(unsigned index,int time);
        virtual void  SetTime(int value);
        virtual void  Draw(NGUITimeSelector* pOwner) const;
        virtual bool  AutoSelect(float dt);
        virtual void  GoTo(int dir,int data);
        virtual void  SetStep(int step);

    protected:
        virtual void Scroll(float delta);

    };

    class TimeLineScroller: public TimeScroller
    {
          int         nMin; 
    protected:
         virtual void        BuildTimeData(int min,int max,TTimes& vTimes);
         virtual void        OnStopScroll(int time);
    public:
         TimeLineScroller():TimeScroller()
         {
             bCheckTime = false;
         }
         virtual void  Init(int min,int max,int step,EScrollType type){TimeScroller::Init(min,max,step,type);}
         virtual void  Init(NGUITimeSelector* owner);
         virtual void  SetListIndex(unsigned index,int time){}
         virtual void  SetTime(int value){ ResetData(GetIndex(value));}
         virtual void  AddTimeInterval(int min,int max){vIntervals.push_back(Interval(min,max));}
         virtual void  SetMinMax(int min,int max);
         virtual void  Draw(NGUITimeSelector* pOwner) const;
         virtual void  GoTo(int dir,int data){}
         virtual void  SetStep(int step);

    };

    class AmPmScroller: public Scroller
    {
        class Element
        {
        public:
            int    nData;
            float  nPos;
            Element(int data,float pos):nPos(pos),nData(data){}
            Element():nPos(-1000),nData(-1){}

        };
        int    nTextWidth;
    public:
        typedef std::vector<Element> TDataList;
        TDataList vData;

        AmPmScroller():Scroller(false),nTextWidth(0)
        {}
        void  Init();  
        void  Init(NGUITimeSelector* owner);
        bool  CanScroll(float delta);
        void  Build();
        void  Draw(NGUITimeSelector* pOwner) const;
        bool  AutoSelect(float dt);

    protected:
        virtual void Scroll(float delta);

    };

    class DateScroller: public Scroller
    {
        class Element
        {
        public:
            int    nDay;
            int    nMonth;
            int    nYear;
            float  nPos;
            Element(int day, int month, int year,float pos):nPos(pos),nDay(day),nMonth(month),nYear(year){}
            Element():nPos(-1000),nDay(0),nMonth(0),nYear(0){}
        };
        const Element*   pAutoSelectElement;
        int  GetDaysInMonth(int month, int year);
        void Incriment(int& day, int& month, int& year);
        void Decriment(int& day, int& month, int& year);
    public:
        typedef std::vector<Element> TDataList;
        TDataList vData;

        DateScroller():Scroller(true),pAutoSelectElement(NULL){}

        void  Build(int day, int month, int year);
        void  Draw(NGUITimeSelector* pOwner) const;
        bool  AutoSelect(float dt);
        void  GoTo(int dir,int data);

    protected:
        virtual void Scroll(float delta);

    };

    char  vDaysText[7][64];	
    char  vMonthsText[12][64];	
    char  vAmPmText[2][3];	

    char  vBuffer[64];

    int   nYear,nMonth,nDay,nHour,nMinute;
    EScrollType  eScrollMode;
    EScrollType  eDragged;
    int          nMinTime;
    int          nMaxTime;
    TIntervals   vTimeLine;

    bool         bFirstStep;


    std::vector<Scroller*> vScrollers;
    int          nAmPmData;
    bool         bNextDay; 

    TimeScroller hourScroller;
    TimeScroller minuteScroller;
    AmPmScroller ampmScroller;
    DateScroller dateScroller;

    TimeLineScroller timelineScroller;

	ScriptFunction callback_select_time;	


    const char* GetDayOfWeek(int day, int month, int year);


    EScrollType CheckScrollMode(float x);
    void        CheckPress(float dt, int level);
    void        PrintText(const Scroller* pScroller,float x,float y,const char* text,bool bVailed = true);
    void        InitScrollers();  
    void        InitScrollersView();  
    void        CheckTime(int time,EScrollType type);
    void        SetTime(int nHour,int nMinute);
    void        SelectDate(int day, int month, int year);
    void        SetAmPm(int  data);
    bool        ChaeckTimeLine(int& dir,int& hour,int newTime);

public:
    TextureEntry::Slice background;
    TextureEntry::Slice slectedBackground;

    ENTITY_CONSTRUCT(NGUITimeSelector, NGUIWidget)
    ENTITY_SCRIPTDEC

	PROP_START

    FONT_PROP( NGUITimeSelector, font, "Font", "Visual" )

    BOOL_PROP(NGUITimeSelector, backgroundUseEntity, false, "Background", "BackgroundUseEntity")
    STRING_ENUM_PROP(NGUITimeSelector, backgroundEntityName, "", "Background", "Background", true, EditorGUITexrtureEnumerator)

#ifdef EDITOR
    CALLBACK_PROP(NGUITimeSelector, backgroundEdit, StartEditBackGroundTexture, "Background", "Edit")
#endif

    BOOL_PROP(NGUITimeSelector, selectedBackgroundUseEntity, false, "SelectedBackground", "SelectedBackgroundUseEntity")
    STRING_ENUM_PROP(NGUITimeSelector, selectedBackgroundEntityName, "", "SelectedBackground", "SelectedBackground", true, EditorGUITexrtureEnumerator)

#ifdef EDITOR
    CALLBACK_PROP(NGUITimeSelector, selectedBackgroundEdit, StartEditSelectedBackGroundTexture, "SelectedBackground", "Edit")
#endif

    BOOL_PROP(NGUITimeSelector, bTimeLine, false, "Properties","TimeLine")
    BOOL_PROP(NGUITimeSelector, bAllowDate, true, "Properties","AllowDate")
    BOOL_PROP(NGUITimeSelector, bAmPm, true, "Properties","AmPmMode")
    INT_PROP(NGUITimeSelector,nMinutesStep, 1, "Properties", "MinutesStep")
    INT_PROP(NGUITimeSelector,nMaxHour, 24, "Properties", "MaxHour")
    INT_PROP(NGUITimeSelector,nMinHour, 0, "Properties", "MinHour")
    INT_PROP(NGUITimeSelector,nMaxMinute,60, "Properties", "MaxMinute")
    INT_PROP(NGUITimeSelector,nMinMinute, 0, "Properties", "MinMinute")
    INT_PROP(NGUITimeSelector,nElementsCount, 7, "Properties", "ElementsCount")
    FLOAT_PROP(NGUITimeSelector,nElementHeigth,32,"Properties", "ElementHeigth")

    STRING_PROP(NGUITimeSelector, onSelectFunc, "", "Events", "OnSelect")
    COLOR_PROP(NGUITimeSelector,selectedColor, 0xffffffff, "Visual", "SlectedColor")
    COLOR_PROP(NGUITimeSelector,choiceColor, 0xffffffff, "Visual", "ChoiceColor")
    COLOR_PROP(NGUITimeSelector,notVailedTimeColor, 0xffffffff, "Visual", "NotVailedTimeColor")

#ifdef PC
    virtual void DataFromGizmo();	
#endif

	virtual void RegisterProperties();
    virtual void Init();
    virtual void ReInit();
	virtual bool Start();
	virtual void Finish();		
    virtual void Release();    
    virtual void RenderWidget();
    virtual void MethodsToScript(const char* script_class_name);
    // Script 
    void NTSSetDayTextScript(int index, string& text);
    void NTSSetMonthTextScript(int index, string& text);
    void NTSSetAmPmTextScript(int index, string& text);
    void NTSSetTimeScript(int hour,int minute);
    void NTSClearTimeIntervalsScript();
    void NTSAddTimeIntervalScript(int minHour,int minMinute,int maxHour,int maxMinute);
    void NTSSetMaxTimeScript(int hour,int minute);
    void NTSSetMinTimeScript(int hour,int minute);
    void NTSSetDateScript(int day,int month,int year);
    void NTSSetAmPmModeScript(bool bSet);
    void NTSSetDateModeScript(bool bSet);
    void NTSSetMinuteStepScript(int step);
    void NTSGetSelectedDateScript(int& day, int& month, int& year);
    void NTSGetTimeScript(int& hour,int& minute);

    void NTSSetTimesColorScript(float r, float g, float b);
    void NTSSetSelectedTimeColorScript(float r, float g, float b);
    void NTSSetNotValidTimeColorScript(float r, float g, float b);
    bool NTSIsTimeValid(int hour,int minute);

	void NTSScriptSetDelegate(asIScriptGeneric *gen);
};


template <class TData >
const TData* NGUITimeSelector::Scroller::CalcalateVelocityForAutoSelect(const std::vector<TData>& list)
{
    nVelocity = 0;
    float  h = Heigth(), nLength = h;
    const TData* memory = NULL;
    for(unsigned i = 0;i < list.size();++i)
    {
        float elementPos = list[i].nPos - nLimitMin;
        float dPos = nSelectedIndex *  h - elementPos;
        if(nLength >= fabs(dPos) )
        {
            memory = &list[i];
            nLength = fabs(dPos);
            nVelocity = dPos * AUTO_SELECT_SPEED;
        }
    }
    if(!memory)
        core->TraceTo("Debug","Error Did not find target");
    return memory;
}


