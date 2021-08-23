#include "NGUITimeSelector.h"

ENTITY(NGUITimeSelector)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUITimeSelector,"NGUITimeSelector","NGUI", "TimeSelector")

#ifdef EDITOR
void StartEditBackGroundTexture(PropertiesOwner* owner)
{
    NGUITimeSelector* label = (NGUITimeSelector*)owner;
    TextureEntry::StartEditGUITexture(&label->background);
}
void StartEditSelectedBackGroundTexture(PropertiesOwner* owner)
{
    NGUITimeSelector* label = (NGUITimeSelector*)owner;
    TextureEntry::StartEditGUITexture(&label->slectedBackground);
}
#endif

void NGUITimeSelector::RegisterProperties()
{
	NGUIWidget::RegisterProperties();	

	PROP_REG( font )	
	PROP_REG(selectedColor)
	PROP_REG(choiceColor)
	PROP_REG(notVailedTimeColor)
	PROP_REG(nElementHeigth)
	PROP_REG(nElementsCount)
	PROP_REG(bAllowDate)
	PROP_REG(bAmPm)
	PROP_REG(bTimeLine)
	PROP_REG(nMinutesStep)
	PROP_REG(nMaxHour)
	PROP_REG(nMaxMinute)
	PROP_REG(nMinHour)
	PROP_REG(nMinMinute)

	PROP_REG(onSelectFunc)

	PROP_REG( backgroundUseEntity )
	PROP_REG( backgroundEntityName )
#ifdef EDITOR
	PROP_REG( backgroundEdit )	
#endif

	PROP_REG( selectedBackgroundUseEntity )
	PROP_REG( selectedBackgroundEntityName )

#ifdef EDITOR
	PROP_REG( selectedBackgroundEdit )	
#endif
}

void NGUITimeSelector::Init()
{
    NGUIWidget::Init();	    

#ifdef EDITOR
        slices.push_back(TexSlice(&background, &backgroundUseEntity,get_backgroundEntityNameprop_inst(), get_backgroundEditprop_inst(), backgroundEntityName.c_str()));
#else
        slices.push_back(TexSlice(&background, &backgroundUseEntity,backgroundEntityName.c_str()));
#endif

#ifdef EDITOR
    slices.push_back(TexSlice(&slectedBackground, &selectedBackgroundUseEntity,get_selectedBackgroundEntityNameprop_inst(), get_selectedBackgroundEditprop_inst(),selectedBackgroundEntityName.c_str()));
#else
    slices.push_back(TexSlice(&slectedBackground, &selectedBackgroundUseEntity,selectedBackgroundEntityName.c_str()));
#endif


    static const char* day_str_src [] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };				
    static const char* month_str_src [] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    static const char* ampm_str_src [] = {"AM", "PM"};
    for (int i=0;i<7;i++)
        String::Copy(vDaysText[i],64,day_str_src[i]);
    for (int i=0;i<12;i++)
        String::Copy(vMonthsText[i], 64, month_str_src[i]);
    for (int i=0;i<2;i++)
        String::Copy(vAmPmText[i], 3, ampm_str_src[i]);
    SetUpdate((TaskHolder::Task)&NGUITimeSelector::CheckPress, lvl_pre5 + 3);

    vScrollers.resize(eScrollCount);
    for(unsigned i = 0;i < vScrollers.size();++i)vScrollers[i] = NULL;
    vScrollers[eScrollHour] = &hourScroller;
    vScrollers[eScrollMinute] = &minuteScroller;
    vScrollers[eScrollAmPm] = &ampmScroller;
    vScrollers[eScrollDate] = &dateScroller;
    vScrollers[eTimeLineScroller] = &timelineScroller;

    nMinMinute = nMinHour = 0;
    nMaxMinute = 59;nMaxHour = 24;
}

void NGUITimeSelector::ReInit()
{
    NGUIWidget::ReInit();


    eScrollMode = eScrollNone;
    clipChilds = true;
    nYear = GetTime(5);	
    nMonth = GetTime(4);
    nDay = GetTime(3);
    nMinute = GetTime(1);
    nHour = GetTime(0);
    bNextDay = false;

    RELEASE(font.font)
    font.font = en_core.Font()->CreateFont(font.name, font.bold, font.italic,(int)ConvertVert(font.height), font.gap, font.outline);		

    if(nMinHour < 0)nMinHour = 0;
    if(nMaxMinute > 59)nMaxMinute = 59;
    if(nMinMinute < 0)nMinMinute = 0;
    nMaxTime  = 0;
    if(nMaxHour < nMinHour)
        nMaxTime = (nMaxHour + 24) *60  + nMaxMinute;
    else
        nMaxTime = nMaxHour *60  + nMaxMinute;
    nMinTime = nMinHour *60  + nMinMinute;
    vTimeLine.clear();
    vTimeLine.push_back(Interval(nMinTime,nMaxTime));
    InitScrollers();
    bFirstStep = true;
  
}

void NGUITimeSelector::InitScrollers()
{

    for(unsigned i = eScrollHour;i < eScrollCount;++i)
        vScrollers[i]->Init(this);

    minuteScroller.Init(0,60,nMinutesStep,eScrollMinute);
    hourScroller.Init(nMinHour,nMaxHour,1,eScrollHour);
    timelineScroller.Init(nMinTime,nMaxTime,1,eTimeLineScroller);
    if(bAmPm)
    {
        if(nHour < 12)
            nAmPmData = 0;
        else
            nAmPmData = 1;
    }
    InitScrollersView();
    dateScroller.Build(nDay,nMonth,nYear);
    NTSSetTimeScript(nHour,nMinute);
}

 void NGUITimeSelector::InitScrollersView()
 {
     float widthElement = width;
     float x = 0;
     if(bTimeLine)
     {
         minuteScroller.SetPosSize(0,0);
         hourScroller.SetPosSize(0,0);
         ampmScroller.SetPosSize(0,0);
         dateScroller.SetPosSize(0,0);
         timelineScroller.SetPosSize(0,widthElement);
     }
     else
     {
         timelineScroller.SetPosSize(0,0);
         if(bAllowDate)
         {
             x = widthElement = NGUIService::instance->h_mul_ScreenToRef * font.font->GetWidth("Wwx Wwx 00");
             dateScroller.SetPosSize(0,widthElement);
             widthElement = width - widthElement;
         }
         else
             dateScroller.SetPosSize(0,0);
         float stepX =  widthElement / 2.f;
         if(bAmPm)
             stepX = widthElement / 3.f;
         hourScroller.SetPosSize(x,stepX);
         x += stepX;
         minuteScroller.SetPosSize(x,stepX);
         x += stepX;
         if(bAmPm)
             ampmScroller.SetPosSize(x,stepX);
         else
             ampmScroller.SetPosSize(0,0);
     }

 }

void NGUITimeSelector::PrintText(const Scroller* pScroller,float x,float y,const char* text,bool bVailed)
{
    Color color = selectedColor;
    int selectedIndex = nElementsCount / 2;
    float elementPos = y - pScroller->LimitMin();
    if(elementPos + nElementHeigth <= selectedIndex * nElementHeigth || elementPos  >= (selectedIndex + 1) * nElementHeigth )
    {
        if(bVailed)
            color = choiceColor;
        else
            color = notVailedTimeColor;
        color.a = 0.2f + (0.4f - fabs(elementPos - selectedIndex  * nElementHeigth) / ( 2.5f *  selectedIndex * nElementHeigth) );
    }
    else
    {
        color.a = 0.6f + (0.4f - fabs(elementPos - selectedIndex * nElementHeigth)  / (2.5f * nElementHeigth));
        if(bVailed)
        {
            float k = fabs(elementPos - selectedIndex * nElementHeigth)  /  nElementHeigth;
            color.r = selectedColor.r + ( choiceColor.r - selectedColor.r) * k;
            color.g = selectedColor.g + ( choiceColor.g - selectedColor.g) * k;
            color.b = selectedColor.b + ( choiceColor.b - selectedColor.b) * k;
        }
        else
            color = notVailedTimeColor;
    }
    float yPos  =  NGUIService::instance->v_mul_RefToScreen * (y + (nElementHeigth - font.height) / 2 - 3);
    AddPrintCall(font.font,NGUIService::instance->h_mul_RefToScreen * x,yPos,scr_w,NGUIService::instance->v_mul_RefToScreen * nElementHeigth, null, 1,color,text);

}

void NGUITimeSelector::RenderWidget()
{
    if(bFirstStep)
    {
        NTSSetTimeScript(nHour,nMinute);
        bFirstStep = false;
    }
    if (background.tex.texture || slices[0].tex_entity)
    {
        Color backColor = clr;
        if (slices[0].tex_entity)
            TextureEntry::Draw(this, 0,0, scr_w, scr_h, clr, &slices[0].tex_entity->slice);
        else
            TextureEntry::Draw(this, 0,0, scr_w, scr_h, clr, &background);
    }
    if (slectedBackground.tex.texture || slices[1].tex_entity)
    {
        float y = (height /2 - nElementHeigth/2) * NGUIService::instance->v_mul_RefToScreen; 
        if (slices[1].tex_entity)
            TextureEntry::Draw(this, 0,y, scr_w, nElementHeigth * NGUIService::instance->v_mul_RefToScreen, clr, &slices[1].tex_entity->slice);
        else
            TextureEntry::Draw(this, 0,y, scr_w, nElementHeigth * NGUIService::instance->v_mul_RefToScreen, clr, &slectedBackground);
    }

    if(bTimeLine)
        timelineScroller.Draw(this);
    else
    {
        if(bAllowDate)
            dateScroller.Draw(this);
        hourScroller.Draw(this);
        minuteScroller.Draw(this);
        if(bAmPm)
            ampmScroller.Draw(this);
    }
    
}

bool NGUITimeSelector::Start()
{	
	ScriptFunction::Find1P(&callback_select_time, Scene()->ScriptModule(), onSelectFunc.c_str(), Desc()->GetName());

	return NGUIWidget::Start();
}

void NGUITimeSelector::Finish()
{
	callback_select_time.Reset();
}


void NGUITimeSelector::Release()
{
	DelUpdate((TaskHolder::Task)&NGUITimeSelector::CheckPress);
    RELEASE(font.font)	
	NGUIWidget::Release();
}
//////////////////////////////////////////////////////////////////////////
CALLWRAPPERARG2(NGUITimeSelector, NTSSetDayTextScript, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(NGUITimeSelector, NTSSetMonthTextScript, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(NGUITimeSelector, NTSSetAmPmTextScript, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(NGUITimeSelector, NTSSetTimeScript, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUITimeSelector, NTSSetDateScript, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETPARAM(int,arg2),CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUITimeSelector, NTSSetAmPmModeScript, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUITimeSelector, NTSSetDateModeScript, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())

CALLWRAPPER(NGUITimeSelector,     NTSClearTimeIntervalsScript, CALLWRAPPERNORET())
CALLWRAPPERARG4(NGUITimeSelector, NTSAddTimeIntervalScript, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETPARAM(int,arg2),CALLWRAPPERGETPARAM(int,arg3),CALLWRAPPERGETPARAM(int,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG2(NGUITimeSelector, NTSSetMaxTimeScript, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(NGUITimeSelector, NTSSetMinTimeScript, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUITimeSelector, NTSSetMinuteStepScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())

CALLWRAPPERARG3(NGUITimeSelector, NTSGetSelectedDateScript, CALLWRAPPERGETADRESS(int,arg1), CALLWRAPPERGETADRESS(int,arg2), CALLWRAPPERGETADRESS(int,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG2(NGUITimeSelector, NTSGetTimeScript, CALLWRAPPERGETADRESS(int,arg1),CALLWRAPPERGETADRESS(int,arg2), CALLWRAPPERNORET())

CALLWRAPPERARG2(NGUITimeSelector, NTSIsTimeValid, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERRET(bool))
CALLWRAPPERARG3(NGUITimeSelector, NTSSetTimesColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUITimeSelector, NTSSetSelectedTimeColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUITimeSelector, NTSSetNotValidTimeColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())

void CALLWRAPPERNAME(NTSScriptSetDelegate)(asIScriptGeneric *gen)
{
	NGUITimeSelector* obj = (NGUITimeSelector*)gen->GetObject();
	obj->NTSScriptSetDelegate(gen);	
}

void NGUITimeSelector::MethodsToScript(const char* script_class_name)
{ 
    NGUIWidget::MethodsToScript(script_class_name);
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDayText(int index, string&in text)", CALLWRAPPERNAMEAS(NTSSetDayTextScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetMonthText(int index, string&in text)", CALLWRAPPERNAMEAS(NTSSetMonthTextScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetAmPmText(int index, string&in text)", CALLWRAPPERNAMEAS(NTSSetAmPmTextScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTime(int hour,int minute)", CALLWRAPPERNAMEAS(NTSSetTimeScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void ClearTimeIntervals()", CALLWRAPPERNAMEAS(NTSClearTimeIntervalsScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void AddTimeInterval(int hourMin,int minuteMin,int hourMax,int minuteMax)", CALLWRAPPERNAMEAS(NTSAddTimeIntervalScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetMaxTime(int hour,int minute)", CALLWRAPPERNAMEAS(NTSSetMaxTimeScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetMinTime(int hour,int minute)", CALLWRAPPERNAMEAS(NTSSetMinTimeScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDate(int day,int month,int year)", CALLWRAPPERNAMEAS(NTSSetDateScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetMinuteStep(int step)", CALLWRAPPERNAMEAS(NTSSetMinuteStepScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetAmPmMode(bool bSet)", CALLWRAPPERNAMEAS(NTSSetAmPmModeScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDateMode(bool bSet)", CALLWRAPPERNAMEAS(NTSSetDateModeScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetSelectedDate(int&out day, int&out month, int&out year)", CALLWRAPPERNAMEAS(NTSGetSelectedDateScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetSelectedTime(int&out hour, int&out minute)", CALLWRAPPERNAMEAS(NTSGetTimeScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool IsTimeValid()", CALLWRAPPERNAMEAS(NTSIsTimeValid));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTimesColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NTSSetTimesColorScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetSelectedTimeColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NTSSetSelectedTimeColorScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetNotValidTimeColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NTSSetNotValidTimeColorScript));	

	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterface("NGUITimeSelectorDelegate");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUITimeSelectorDelegate", "void OnSelectTime(NGUITimeSelector@, int)");	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDelegate(NGUITimeSelectorDelegate@ delegate)", CALLWRAPPERNAMEAS(NTSScriptSetDelegate));
}

void NGUITimeSelector::NTSScriptSetDelegate(asIScriptGeneric *gen)
{
	asIScriptObject* scr_object = (asIScriptObject*)gen->GetArgObject(0);	

	asITypeInfo* tp_info = scr_object->GetObjectType();	

	asIScriptFunction* scr_func;

	scr_func = tp_info->GetMethodByDecl("void OnSelectTime(NGUITimeSelector@)", false);
	callback_select_time.Set(scr_func, scr_object);	
}

void NGUITimeSelector::NTSSetMinuteStepScript(int step)
{
    nMinutesStep = step;
    if(bTimeLine)
        timelineScroller.SetStep(nMinutesStep);
    else
        minuteScroller.SetStep(nMinutesStep);
}

void NGUITimeSelector::NTSAddTimeIntervalScript(int minHour,int minMinute,int maxHour,int maxMinute)
{
    MathClip(maxHour,24,0)
    MathClip(maxMinute,59,0)
    MathClip(minHour,24,0)
    MathClip(minMinute,59,0)


    int newMinTime = minHour * 60  + minMinute;
    int newMaxTime = 0;
    if(maxHour < minHour)
        newMaxTime = (maxHour + 24) * 60  + maxMinute;
    else
        newMaxTime = maxHour * 60  + maxMinute;

    if(newMinTime < nMinTime)
    {
        nMinMinute = minMinute;
        nMinHour = minHour;
        nMinTime = newMinTime;
    }
    if(newMaxTime > nMaxTime)
    {
        nMaxMinute = maxMinute;
        nMaxHour = maxHour;
        nMaxTime = newMaxTime;
    }

    if(vTimeLine.empty())
        vTimeLine.push_back(Interval(nMinTime,nMaxTime));
    else
    {   
        unsigned i = 0;
        TIntervals::iterator it = vTimeLine.begin();
        for(; i < vTimeLine.size();++i,it++)
        {
            if(i + 1 < vTimeLine.size() && newMinTime > vTimeLine[i].nMax && newMaxTime < vTimeLine[i + 1].nMin )
            {
                vTimeLine.insert(it,Interval(newMinTime,newMaxTime));
                break;
            }
            else
                if(newMaxTime > vTimeLine[i].nMin && newMaxTime < vTimeLine[i].nMax)
                {
                    vTimeLine[i].nMin = newMinTime;
                    break;
                }
                else
                    if(newMinTime > vTimeLine[i].nMin && newMinTime < vTimeLine[i].nMax)
                    {
                        vTimeLine[i].nMax = newMaxTime;
                        break;
                    }

        }
        if(i == vTimeLine.size())
        {
            if(newMaxTime < vTimeLine[0].nMin)
                vTimeLine.insert(vTimeLine.begin(),Interval(newMinTime,newMaxTime));
            else
                if(newMinTime > vTimeLine.back().nMax)
                    vTimeLine.push_back(Interval(newMinTime,newMaxTime));
        }
    }
    if(bTimeLine)
        timelineScroller.AddTimeInterval(minHour * 60 + minMinute,maxHour * 60 + maxMinute);
    else 
        hourScroller.AddTimeInterval(minHour,maxHour);


}

void NGUITimeSelector::NTSClearTimeIntervalsScript()
{
    nMaxMinute = nMaxHour = nMinHour = nMinMinute = 0;
    nMaxTime = 0;
    nMinTime = 2147483647;
    if(bTimeLine)
        timelineScroller.ClearTimeIntervals();
    else
        hourScroller.ClearTimeIntervals();
    vTimeLine.clear();
   
}

void NGUITimeSelector::NTSSetMaxTimeScript(int hour,int minute)
{
    nMaxHour = hour;
    nMaxMinute = minute;
    MathClip(nMaxHour,24,0)
    MathClip(nMaxMinute,59,0)

    if(nMaxHour < nMinHour)
        nMaxTime = (nMaxHour + 24) *60  + nMaxMinute;
    else
        nMaxTime = nMaxHour *60  + nMaxMinute;

    if(bTimeLine)
        timelineScroller.SetMinMax(nMinHour * 60 + nMinMinute,nMaxHour * 60 + nMaxMinute);
    else
        hourScroller.SetMinMax(nMinHour,nMaxHour);
    vTimeLine.clear();
    vTimeLine.push_back(Interval(nMinTime,nMaxTime));
    NTSSetTimeScript(nHour,nMinute);

}

void NGUITimeSelector::NTSSetMinTimeScript(int hour,int minute)
{
    nMinHour = hour;
    nMinMinute = minute;
    MathClip(nMinHour,24,0)
    MathClip(nMinMinute,59,0)
    nMinTime = nMinHour *60  + nMinMinute;
    if(bTimeLine)
        timelineScroller.SetMinMax(nMinHour * 60 + nMinMinute,nMaxHour * 60 + nMaxMinute);
    else
        hourScroller.SetMinMax(nMinHour,nMaxHour);
    vTimeLine.clear();
    vTimeLine.push_back(Interval(nMinTime,nMaxTime));
    NTSSetTimeScript(nHour,nMinute);
}

void NGUITimeSelector::NTSGetTimeScript(int& hour, int& minute)
{
    hour = nHour;
    minute = nMinute;
}

void NGUITimeSelector::NTSGetSelectedDateScript(int& day, int& month, int& year)
{
    day = nDay;
    month = nMonth;
    year = nYear;
}

void NGUITimeSelector::NTSSetDayTextScript(int index, string& text)
{
    if (index<0 || index>6) return;
    String::Copy(vDaysText[index], 64, text.c_str());	
}

void NGUITimeSelector::NTSSetMonthTextScript(int index, string& text)
{
    if (index<0 || index>11) return;
    String::Copy(vMonthsText[index], 64, text.c_str());
}

void NGUITimeSelector::NTSSetAmPmModeScript(bool bSet)
{
    if(bAmPm != bSet && !bTimeLine)
    {
        bool bOldFlag = bAmPm;
        bAmPm = bSet;
        InitScrollersView();
        if(!bOldFlag)
            NTSSetTimeScript(nHour,nMinute);
    }
}

void NGUITimeSelector::NTSSetTimeScript(int hour,int minute)
{
    nMinute = minute;
    nHour = hour;
    float newTime = nHour * 60 + nMinute;
    if(newTime < vTimeLine[0].nMin)
    {
        nHour =  vTimeLine[0].nMin / 60;
        nMinute = 0;
    }
    else
        if(newTime > vTimeLine.back().nMax)
        {
             nHour =  vTimeLine.back().nMax / 60;
             nMinute = 0;
        }
        else
        {
            for(unsigned i = 0;i < vTimeLine.size() - 1;++i)
                if( newTime  > vTimeLine[i].nMax && newTime < vTimeLine[i + 1].nMin)
                {
                    nHour =   vTimeLine[i + 1].nMin / 60;
                    if(nMinute < vTimeLine[i + 1].nMin - nHour * 60)
                        nMinute = vTimeLine[i + 1].nMin - nHour * 60;
                    break;
                }
        }
    if(bTimeLine)
        timelineScroller.SetTime(nHour * 60 + nMinute);
    else
    {
        minuteScroller.SetTime(nMinute);
        ampmScroller.Build();
        hourScroller.SetTime(nHour);
    }
}

void NGUITimeSelector::NTSSetDateModeScript(bool bSet)
{
    if(bAllowDate != bSet && !bTimeLine)
    {
        bAllowDate = bSet;
        InitScrollersView();
    }
}

void NGUITimeSelector::NTSSetDateScript(int day,int month,int year)
{
   nDay = day;
   nMonth = month;
   nYear = year;
   if(bAllowDate)
        dateScroller.Build(nDay,nMonth,nYear);
}

void NGUITimeSelector::NTSSetAmPmTextScript(int index, string& text)
{
    if (index<0 || index>1) return;
    String::Copy(vAmPmText[index], 3, text.c_str());	
}


//////////////////////////////////////////////////////////////////////////

#ifdef PC

void NGUITimeSelector::DataFromGizmo()
{
    NGUIWidget::DataFromGizmo();
    InitScrollers();
}

#endif

const char* NGUITimeSelector::GetDayOfWeek(int day, int month, int year)
{
    int a = (14 - month) / 12;
    int y = year - a;
    int m = month + 12 * a - 2;
    int week_day = (7000 + (day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12)) % 7 - 1;
    if (week_day < 0) week_day = 6;
    return vDaysText[week_day];
}

NGUITimeSelector::EScrollType NGUITimeSelector::CheckScrollMode(float x)
{
    if(bTimeLine)
    {
        if(vScrollers[eTimeLineScroller]->IsIn(x))
            return eTimeLineScroller;
    }
    else
    {
        if(bAllowDate && vScrollers[eScrollDate]->IsIn(x))
            return eScrollDate;
        if(vScrollers[eScrollHour]->IsIn(x))
            return eScrollHour;
        if(vScrollers[eScrollMinute]->IsIn(x))
            return eScrollMinute;
        if(bAmPm && vScrollers[eScrollAmPm]->IsIn(x))
            return eScrollAmPm;
    }
    return eScrollNone;
}

void NGUITimeSelector::CheckPress(float dt, int level)
{
    if (Scene()->IsRunning() && IsVisible() && IsActive() )
    {
        if(eDragged == eScrollNone)
        {
            for(unsigned i = eScrollHour;i < eScrollCount;++i)
                if(vScrollers[i]->NeedAuto() || vScrollers[i]->IsDraged())
                    vScrollers[i]->Step(dt);

            if (IsHitted(ms_pos_x, ms_pos_y, 0, 0, scr_w, scr_h) )
            {
                if(core->Controls()->CheckTouch(0, ControlsService::Activated, this))
                {
                    eDragged = CheckScrollMode(ms_pos_x - scr_x);
                    if(eDragged != eScrollNone)
                        vScrollers[eDragged]->StartDrag();
                }
            }

        }
        else
        {
            for(unsigned i = eScrollHour;i < eScrollCount;++i)
                if(vScrollers[i]->NeedAuto())
                    vScrollers[i]->Step(dt);
            if(core->Controls()->CheckTouch(0, ControlsService::Active,this) && IsHitted(ms_pos_x, ms_pos_y, 0, 0, scr_w, scr_h))
                vScrollers[eDragged]->Step(dt,Convert2AbsVert(ms_pos_y - ms_prev_pos_y));
            else
                eDragged = eScrollNone;
        }

    }
}

bool NGUITimeSelector::ChaeckTimeLine(int& dir,int& hour,int newTime)
{

    if(newTime < vTimeLine[0].nMin)
    {
        hour++;
        dir = -1;
        return true;
    }
    else
        if(newTime > vTimeLine.back().nMax)
        {
            --nHour;
            dir = 1;
            if(nHour < 0)
            {
                if(bAmPm)
                {
                    if(!nAmPmData)
                    {
                        nHour = nMinHour;
                        dir = -1;
                    }
                }
                else
                    nHour = 23;
            }
            return true;
        }
        else
        {
            unsigned i = 0;
            for(;i < vTimeLine.size() - 1;++i)
                if( newTime  > vTimeLine[i].nMax && newTime < vTimeLine[i + 1].nMin)
                {
                    if(newTime >  vTimeLine[i + 1].nMin - 60)
                    {
                        hour++;
                        dir = -1;
                        return true;
                    }
                    else
                        if(newTime  <  vTimeLine[i].nMax  + 60)
                        {
                            --nHour;
                            dir = 1;
                            return true;
                        }
                }
        }

    return false;
}

void NGUITimeSelector::SetTime(int nHour,int nMinute)
{
    this->nHour = nHour;
    this->nMinute = nMinute;

	core->Script()->Machine()->AddPtrArg(this);							
	core->Script()->Machine()->Call(&callback_select_time);		    
}

void NGUITimeSelector::CheckTime(int time,EScrollType type)
{
    if(type == eScrollMinute)
    {
        if(nMinute == time)return;
        nMinute = time;
    }
    else
        if(type == eScrollHour)
        {
            if(bAmPm && nAmPmData)time += 12;
            if(nHour == time)return;
            nHour = time;
        }
    int newTime = 0;
    if(nHour < nMinHour && nHour <= nMaxHour)
        newTime = (nHour  + 24) * 60 + nMinute;
    else
        newTime = nHour * 60 + nMinute;

    int dir = 1;
    if(ChaeckTimeLine(dir,nHour,newTime))
    {
        if(bAmPm && nAmPmData)
            hourScroller.GoTo(dir,nHour - 12);
        else
            hourScroller.GoTo(dir,nHour);
    }
    if(type == eScrollHour)
    {
        if(nHour < nMinHour && nHour <= nMaxHour)
        {
            if(!bNextDay)
            {
                ++nDay;
                if(bAllowDate)dateScroller.GoTo(-1,nDay);
                bNextDay = true;
            }
        }
        else
        {
            if(bNextDay)
            {
                --nDay;
                if(bAllowDate)dateScroller.GoTo(1,nDay);
                bNextDay = false;
            }

        }
    }

	if (Scene()->IsRunning())
	{
		core->Script()->Machine()->AddPtrArg(this);							
		core->Script()->Machine()->Call(&callback_select_time);		    
	}
}

void NGUITimeSelector::SetAmPm(int data)
{
     nAmPmData = data;
     hourScroller.SetListIndex(data + 1,nHour);
}

void NGUITimeSelector::SelectDate(int day, int month, int year)
{
    if(nDay == day && nMonth == month && nYear == year)return;
    nDay = day;
    nMonth = month;
    nYear = year;

	if (Scene()->IsRunning())
	{
		core->Script()->Machine()->AddPtrArg(this);							
		core->Script()->Machine()->Call(&callback_select_time);		    
	}
}

void NGUITimeSelector::NTSSetTimesColorScript(float r, float g, float b)
{
    choiceColor.r = r;
    choiceColor.g = g;
    choiceColor.b = b;
}

void NGUITimeSelector::NTSSetSelectedTimeColorScript(float r, float g, float b)
{
    selectedColor.r = r;
    selectedColor.g = g;
    selectedColor.b = b;
}

void NGUITimeSelector::NTSSetNotValidTimeColorScript(float r, float g, float b)
{
    notVailedTimeColor.r = r;
    notVailedTimeColor.g = g;
    notVailedTimeColor.b = b;
}

bool NGUITimeSelector::NTSIsTimeValid(int hour,int minute)
{
    int nTime = hour * 60 + minute;
    if(vTimeLine.empty())return true;
    for(unsigned i =0;i < vTimeLine.size();++i)
       if(nTime >= vTimeLine[i].nMin && nTime <= vTimeLine[i].nMax)return true;
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NGUITimeSelector::Scroller::Init()
{
    bIsDraged = bNeedAuto = bAutoSelect = false;
    nTarget = nVelocity = nTimeAccum = nOffsetAccum = 0;
    nOffset = nLimitMin;
}

void NGUITimeSelector::Scroller::Init(NGUITimeSelector* owner)
{
    pOwner = owner;
    nSelectedIndex = pOwner->nElementsCount / 2;
    SetLimits((int)(pOwner->height /2 - Heigth()/2  - nSelectedIndex  * Heigth()),(int)(pOwner->height /2 + Heigth()/2  + nSelectedIndex  * Heigth() + 0.5f));
    Init();
}

void NGUITimeSelector::Scroller::Step(float dt)
{
    if(bIsDraged)
    {
        bNeedAuto = true;
        if (bAuto)
        {
            nTarget = nOffset + nVelocity;
            bAutoSelect = false;
            nTimeAccum = 0.0f;
        }
        else
        {
            bAutoSelect = false;
            if(AutoSelect(dt))
               Init();
            else
               bAutoSelect = true;
        }
    }
    bIsDraged = false;
    if (bNeedAuto)
    {
        if(bAutoSelect)
        {
            if(AutoSelect(dt))
                Init();
        }
        else
        {
       
            nTimeAccum += dt;
            float nAutoDelta = static_cast<float>(-nVelocity * exp(-static_cast<double>(nTimeAccum) * 2.0));
            if (fabs(nAutoDelta) > 1.5f)
            {
                float oldOffset = nOffset;
                nOffset = nTarget + nAutoDelta;
                Scroll(nOffset - oldOffset);
            }
            else
            {
                bAutoSelect = false;
                if(AutoSelect(dt))
                    Init();
                else
                    bAutoSelect = true;
            }
        }
    }
}

void NGUITimeSelector::Scroller::Step(float dt,float delta)
{
    if (fabs(delta) != 0)
    {
        if (SIGN(delta) != SIGN(nVelocity))
            nVelocity = SIGN(delta) * fabs(nVelocity) * 0.6f;
    }
    float v = delta / dt;
    nVelocity += v * 0.1f;
    nOffsetAccum += delta;
    nTimeAccum += dt;
    if (nTimeAccum > 0.05f)
    {
        float v = nOffsetAccum / nTimeAccum;
        if (fabs(nOffsetAccum) < 45.0f)
        {
            nVelocity = nVelocity * 0.15f + v * 0.85f;
            if (fabs(nVelocity) > 500.f)
                nVelocity = SIGN(nVelocity) * 500.f;
        }
        nOffsetAccum = 0.0f;
        nTimeAccum = 0.0f;
    }
    if (fabs(nVelocity) > 500.f)
        nVelocity = SIGN(nVelocity) * 500.f;
    if(CanScroll(delta))
    {
        nOffset += delta;
        Scroll(delta);
    }
}

bool NGUITimeSelector::Scroller::IsIn(float scr_pos)
{
    return scr_pos > NGUIService::instance->h_mul_RefToScreen * nX &&  scr_pos < NGUIService::instance->h_mul_RefToScreen * nX + NGUIService::instance->h_mul_RefToScreen * nWidth;
}

void NGUITimeSelector::TimeScroller::Init(NGUITimeSelector* owner)
{
    Scroller::Init(owner);
    nTextWidth =  NGUIService::instance->h_mul_ScreenToRef * pOwner->font.font->GetWidth("00");
}

void NGUITimeSelector::TimeScroller::Init(int min,int max,int step,EScrollType type)
{
    eType = type;
    nStep = step;
    SetMinMax(min,max);
    ResetPositions();
 
}

void NGUITimeSelector::TimeScroller::ResetPositions()
{
    vData.clear();
    float pos = nLimitMin;
    for(int i = 0;i < pOwner->nElementsCount;++i,pos += Heigth())
        vData.push_back(Element(pos));
}

void  NGUITimeSelector::TimeScroller::BuildTimeData(int min,int max,NGUITimeSelector::TimeScroller::TTimes& vTimes)
{
    vTimes.clear(); float realMax = 24;
    if(eType == eScrollMinute)
    {
        vTimes.resize(1);
        realMax = 60;
    }
    else
        vTimes.resize(3);
    if(max > min)
    {
        int count = max;
        if(max < realMax)++count;
        for(int i = min; i < count;i += nStep)
            vTimes[0].push_back(i);
    }
    else
        if(max < min)
        {
            for(int i = min; i < realMax;i += nStep)
                vTimes[0].push_back(i);
            for(int i = 0; i <= max;i += nStep)
                vTimes[0].push_back(i);
        }
}

void  NGUITimeSelector::TimeScroller::CopyTail(TTimeData& dest,const TTimeData& sourse)
{
    int max = dest.back();
    for(unsigned  i = 0; i < sourse.size();++i)
        if(sourse[i] > max)
        {
            for(unsigned  j = i; j < sourse.size();++j)
                dest.push_back(sourse[j]);
            return ;
        }
}

void  NGUITimeSelector::TimeScroller::AddTimeInterval(int min,int max)
{
    TTimes vNewInterval;
    TTimes vMemory = vTimes;
    BuildTimeData(min,max,vNewInterval);
    vTimes.clear();
    vTimes.resize(vNewInterval.size());
    if(vIntervals.empty())
    {
       for(unsigned  j = 0; j < vNewInterval[0].size();++j)vTimes[0].push_back(vNewInterval[0][j]);
    }
    else
    {
        if(vNewInterval[0][0] < vMemory[0][0])
        {
            for(unsigned  j = 0; j < vNewInterval[0].size();++j)vTimes[0].push_back(vNewInterval[0][j]);
            CopyTail(vTimes[0],vMemory[0]);
        }
        else
            if(vNewInterval[0][vNewInterval.size() - 1] > vMemory[0][vMemory.size() - 1])
            {
                for(unsigned  j = 0; j < vMemory[0].size();++j)vTimes[0].push_back(vMemory[0][j]);
                CopyTail(vTimes[0],vNewInterval[0]);
            }
    }

    if(eType == eScrollHour)
    {
        vTimes[1].clear();vTimes[2].clear();
        for(unsigned  i = 0; i < vTimes[0].size();++i)
        {
            if(vTimes[0][i] < 12)
                vTimes[1].push_back(vTimes[0][i]);
            else
                vTimes[2].push_back(vTimes[0][i] - 12);
        }
    }
    vIntervals.push_back(Interval(min,max));
    nMax = 0;
    for(unsigned i = 0;i < vIntervals.size();++i)
        if(nMax < vIntervals[i].nMax)
            nMax = vIntervals[i].nMax;
}

void NGUITimeSelector::TimeScroller::SetMinMax(int min,int max)
{
    vIntervals.clear();
    vIntervals.push_back(Interval(min,max));
    nMax = max;
    BuildTimeData(min,max,vTimes);
    if(eType == eScrollHour)
    {
        for(unsigned  i = 0; i < vTimes[0].size();++i)
        {
            if(vTimes[0][i] < 12)
                vTimes[1].push_back(vTimes[0][i]);
            else
                vTimes[2].push_back(vTimes[0][i] - 12);
        }
    }
}

unsigned  NGUITimeSelector::TimeScroller::GetIndex(int value)
{
    int max = 1000;
    unsigned minIndex = 0;
    for(unsigned i  = 0 ;i < vTimes[nCurList].size();++i)
    {
        if(vTimes[nCurList][i] == value)return i;
        else
            if(abs(vTimes[nCurList][i] - value) < max)
            {
                max = abs(vTimes[nCurList][i] - value);
                minIndex = i;
            }
    }
    return minIndex;
}

void NGUITimeSelector::TimeScroller::ResetData(unsigned index)
{
    unsigned  i = 0;
    for(std::vector<Element>::iterator iter = vData.begin();iter != vData.end();++iter,++i)
    {
        Element& element = (*iter);
        int data = (index + (i - nSelectedIndex));
        if(data < 0)data = vTimes[nCurList].size() + data;
        else
            if(data >=  (int)vTimes[nCurList].size())data -= vTimes[nCurList].size();
        element.nData = data;
    }
}

void NGUITimeSelector::TimeScroller::SetListIndex(unsigned index,int time)
{
    if(index < vTimes.size() && vTimes[index].size() > 0 )
    {
       time = ((int)(time / nStep)) * nStep;
       nCurList = index;
       if(nCurList == 2)
       {
            time -= 12;
            if(time  < 0)time = 0;
       }
       ResetData(GetIndex(time));
       unsigned  i = 0;
       for(std::vector<Element>::iterator iter = vData.begin();iter != vData.end();++iter,++i)
       {
           if(i == nSelectedIndex)
           {
               pOwner->CheckTime(vTimes[nCurList][iter->nData],eType);
               break;
           }
       }
       
    }
}

void NGUITimeSelector::TimeScroller::SetTime(int value)
{
    value = ((int)(value / nStep)) * nStep;
    nCurList = 0;
    if(eType == eScrollHour && pOwner->bAmPm)
    {
        if(value < 12) nCurList = 1;
        else 
        {
            nCurList = 2;
            value -= 12;
        }
    }
    ResetData(GetIndex(value));
    pOwner->CheckTime(value,eType);
}

void NGUITimeSelector::TimeScroller::Scroll(float delta)
{
    for(std::vector<Element>::iterator i = vData.begin();i != vData.end();++i)
        (*i).nPos += delta;

    if(vData.front().nPos < nLimitMin - Heigth())
    {
        while(vData.front().nPos < nLimitMin- Heigth())
        {
            for(unsigned i = 0;i < vData.size() - 1;++i)
            {
                vData[i] = vData[i + 1];
                if(pAutoSelectElement == &vData[i + 1])
                    pAutoSelectElement = &vData[i];
            }
            unsigned data = vData.back().nData + 1;
            if(data >= vTimes[nCurList].size())
                data = 0;
            vData.back().nData = data;
            vData.back().nPos += Heigth();
        }
    }
    else
        while(vData.back().nPos > nLimitMax)
        {
            for(unsigned i = vData.size() - 1;i > 0 ;--i)
            {
                vData[i] = vData[i - 1];
                if(pAutoSelectElement == &vData[i - 1])
                    pAutoSelectElement = &vData[i];
            }
            int data = vData.front().nData - 1;
            if(data < 0 )
                data = vTimes[nCurList].size() - 1;
            vData.front().nData = data;
            vData.front().nPos  -=  Heigth();
        }

}

void NGUITimeSelector::TimeScroller::Draw(NGUITimeSelector* pOwner) const
{
    float x  = nX + (nWidth - nTextWidth)*0.5f;
    for(std::vector<Element>::const_iterator i = vData.begin();i != vData.end();++i)
    {
        char sText[4];
        const Element& element = (*i);
        if(element.nData < vTimes[nCurList].size())
        {
            if(vTimes[nCurList][element.nData] < 10)
                String::Printf(sText, 4, " 0%i", vTimes[nCurList][element.nData]);
            else
                String::Printf(sText, 4, " %i", vTimes[nCurList][element.nData]);
            pOwner->PrintText(this,x ,element.nPos,sText);
        }
    }
}


bool NGUITimeSelector::TimeScroller::AutoSelect(float dt)
{
    if(!bAutoSelect)
    {
      pAutoSelectElement = CalcalateVelocityForAutoSelect<Element>(vData);
      if(pAutoSelectElement)
      {
          if(bCheckTime)
             pOwner->CheckTime(vTimes[nCurList][pAutoSelectElement->nData],eType);
          else
             OnStopScroll(vTimes[nCurList][pAutoSelectElement->nData]);
      }
      return false;
    }
    else 
        if(nVelocity == 0)return true;
    if(pAutoSelectElement)
    {
        float elementPos = pAutoSelectElement->nPos - nLimitMin + nVelocity * dt;
        float nextElementDPos = elementPos - nSelectedIndex *  Heigth();
        //core->TraceTo("Debug"," target pos = %f  pAutoSelectElement->nPos = %f  time = %d ",nSelectedIndex *  Heigth(),pAutoSelectElement->nPos,vTimes[nCurList][pAutoSelectElement->nData]);
        float max = fabs(nVelocity * dt);
        if(max < Heigth() * 0.1f )max = Heigth() * 0.1f;
        if(fabs(nextElementDPos) <= max)
        {
            int index  = pAutoSelectElement->nData;
            ResetPositions();
            ResetData(index);
            return  true;
        }
        else
            Scroll(nVelocity * dt);
        return false;
    }
    else
        return true;
}

void NGUITimeSelector::TimeScroller::GoTo(int dir,int data)
{
   // if(!bNeedAuto && !bIsDraged)
    {
        nVelocity = dir * Heigth() * AUTO_SELECT_SPEED;
        bNeedAuto = true;
        bAutoSelect = true;
        float selectedPos = 0;
        if(pAutoSelectElement)
            selectedPos = pAutoSelectElement->nPos;
        else
            selectedPos = vData[nSelectedIndex].nPos;
        pAutoSelectElement = NULL;
        for(TDataList::const_iterator iter = vData.begin();iter != vData.end();++iter)
        {
            if( dir == SIGN(selectedPos - iter->nPos) && iter->nData >= 0 && iter->nData < (int)vTimes[nCurList].size() && vTimes[nCurList][iter->nData] == data)
            {
                pAutoSelectElement = &(*iter);
                return;
            }
        }
    }
}

void NGUITimeSelector::TimeScroller::SetStep(int step)
{
    nStep = step;
    TIntervals   vMemory = vIntervals;
    vIntervals.clear();
    for(unsigned i = 0;i < vMemory.size();++i)
        AddTimeInterval(vMemory[i].nMin,vMemory[i].nMax);
    if(eType == eScrollHour)SetTime(pOwner->nHour);
    else SetTime(pOwner->nMinute);
}

void NGUITimeSelector::AmPmScroller::Build()
{
    float pos   =  (nSelectedIndex - 1) * Heigth() + nLimitMin;
    if(pOwner->nHour < 12)
        pos += Heigth();
    vData.clear();
    vData.push_back(Element(0,pos));
    vData.push_back(Element(1,pos + Heigth()));
 }

bool NGUITimeSelector::AmPmScroller::CanScroll(float delta)
{
      float elementPos = vData.front().nPos - nLimitMin + delta;
      return (elementPos >= (nSelectedIndex - 1) * Heigth()  &&   elementPos  <= nSelectedIndex * Heigth()); 
}

void NGUITimeSelector::AmPmScroller::Scroll(float delta)
{
    for(std::vector<Element>::iterator i = vData.begin();i != vData.end();++i)
          (*i).nPos += delta;
}

void NGUITimeSelector::AmPmScroller::Init(NGUITimeSelector* owner)
{
    Scroller::Init(owner);
    nTextWidth = NGUIService::instance->h_mul_ScreenToRef * pOwner->font.font->GetWidth(pOwner->vAmPmText[0]);
}

void NGUITimeSelector::AmPmScroller::Init()
{
    bIsDraged = bNeedAuto = bAutoSelect = false;
    nTarget = nVelocity = nTimeAccum = nOffsetAccum = 0;
    nOffset = nLimitMin;
}

void NGUITimeSelector::AmPmScroller::Draw(NGUITimeSelector* pOwner) const
{
    float x  = nX + (nWidth - nTextWidth) * 0.5f;
    pOwner->PrintText(this,x,vData.front().nPos,pOwner->vAmPmText[vData.front().nData]);
    pOwner->PrintText(this,x,vData.back().nPos,pOwner->vAmPmText[vData.back().nData]);
}

bool NGUITimeSelector::AmPmScroller::AutoSelect(float dt)
{
    if(nVelocity == 0)return true;
    if(!bAutoSelect)
        CalcalateVelocityForAutoSelect<Element>(vData);
    for(std::vector<Element>::iterator i = vData.begin();i != vData.end();++i)
        (*i).nPos += nVelocity * dt;
    bool bRet = false;
    if(nVelocity < 0)
    {
         if(fabs((vData.back().nPos - nLimitMin) - nSelectedIndex *  Heigth()) <= fabs(nVelocity * dt) )
         {
             bRet = true;
             pOwner->SetAmPm(vData.back().nData);
         }
    }
    else
    {
        if(fabs((vData.front().nPos - nLimitMin) - nSelectedIndex *  Heigth()) <= fabs(nVelocity * dt) )
        {
            bRet = true;
            pOwner->SetAmPm(vData.front().nData);
        }
    }
    if(bRet)
    {
        float pos;
        if(nVelocity < 0)
            pos  =  (nSelectedIndex - 1) * Heigth() + nLimitMin;
        else
            pos =  nSelectedIndex *  Heigth() + nLimitMin;
        vData.front().nPos = pos;
        vData.back().nPos = pos + Heigth();
        nVelocity = 0;
        
    }
    return bRet;
}

int NGUITimeSelector::DateScroller::GetDaysInMonth(int month, int year)
{
    if (month < 1)
    {
        month = 12;
        year--;
    }
    int days = 31;
    if (month >= 8)
        days = 30;

    if (month % 2 == 0)
    {
        if (month >= 8)
            days = 31;
        else
            days = 30;

        if (month == 2)
        {
            if (year % 4 == 0)
                days = 29;
            else
                days = 28;
        }
    }
    return days;
}

void NGUITimeSelector::DateScroller::Incriment(int& day, int& month, int& year)
{
    ++day;
    if(day > GetDaysInMonth(month,year))
    {
        ++month;
        if(month > 12)
        {
            month = 1;
            ++year;
        }
        day = 1;
    }
}

void NGUITimeSelector::DateScroller::Decriment(int& day, int& month, int& year)
{
    --day;
    if(day == 0)
    {
        --month;
        if(month == 0)
        {
            month = 12;
            --year;
        }
        day = GetDaysInMonth(month,year);
    }
}

void NGUITimeSelector::DateScroller::Build(int day, int month, int year)
{
    vData.clear();
    float pos = nLimitMin;
    int nDay = day;
    int nMonth = month;
    int nYear = year;
    for(unsigned  i =  0; i < nSelectedIndex;++i)
        Decriment(nDay,nMonth,nYear);
    for(int  i = 0; i < pOwner->nElementsCount;++i)
    {
        vData.push_back(Element(nDay,nMonth,nYear,pos));
        Incriment(nDay,nMonth,nYear);
        pos += Heigth();
    }
}

void NGUITimeSelector::DateScroller::Scroll(float delta)
{
    for(std::vector<Element>::iterator i = vData.begin();i != vData.end();++i)
        (*i).nPos += delta;

    if(vData.front().nPos < nLimitMin - Heigth())
    {
        while(vData.front().nPos < nLimitMin - Heigth())
        {
            for(unsigned i = 0;i < vData.size() - 1;++i)vData[i] = vData[i + 1];
            Element& element = vData.back();
            element.nPos += Heigth();
            Incriment(element.nDay,element.nMonth,element.nYear);
        }
    }
    else
        while(vData.back().nPos > nLimitMax)
        {
            for(unsigned i = vData.size() - 1;i > 0 ;--i)vData[i] = vData[i - 1];
            Element& element = vData.front();
            element.nPos -= Heigth();
            Decriment(element.nDay,element.nMonth,element.nYear);
        }

}

void NGUITimeSelector::DateScroller::Draw(NGUITimeSelector* pOwner) const 
{
    char sText[64];
    for(std::vector<Element>::const_iterator i = vData.begin();i != vData.end();++i)
    {
        const Element&  element = (*i);
        String::Printf(sText,64,"%s %s %i",pOwner->GetDayOfWeek(element.nDay,element.nMonth,element.nYear),pOwner->vMonthsText[element.nMonth - 1],element.nDay);
        float width = NGUIService::instance->h_mul_ScreenToRef * pOwner->font.font->GetWidth(sText);
        pOwner->PrintText(this,nX + (nWidth - width),element.nPos,sText);
    }
}

bool NGUITimeSelector::DateScroller::AutoSelect(float dt)
{
    if(!bAutoSelect)
    {
        pAutoSelectElement = CalcalateVelocityForAutoSelect<Element>(vData);
        return false;
    }
    else 
        if(nVelocity == 0)return true;
    if(pAutoSelectElement)
    {
        float elementPos = pAutoSelectElement->nPos - nLimitMin + nVelocity * dt;
        float nextElementDPos = elementPos - nSelectedIndex *  Heigth();
        if(fabs(nextElementDPos) <= fabs(nVelocity * dt))
        {
            pOwner->SelectDate(pAutoSelectElement->nDay,pAutoSelectElement->nMonth,pAutoSelectElement->nYear);
            Build(pAutoSelectElement->nDay,pAutoSelectElement->nMonth,pAutoSelectElement->nYear);
            return true;
        }
        else
            Scroll(nVelocity * dt);
        return false;
    }
    else
        return true;

}

void NGUITimeSelector::DateScroller::GoTo(int dir,int data)
{
    nVelocity = dir * Heigth() * AUTO_SELECT_SPEED;
    bNeedAuto = true;
    bAutoSelect = true;
    pAutoSelectElement = NULL;
    for(TDataList::const_iterator iter = vData.begin();iter != vData.end();++iter)
    {
        if(iter->nDay == data)
        {
            pAutoSelectElement = &(*iter);
            return;
        }
    }
}


void NGUITimeSelector::TimeLineScroller::Init(NGUITimeSelector* owner)
{
    Scroller::Init(owner);
    nTextWidth =  NGUIService::instance->h_mul_ScreenToRef * pOwner->font.font->GetWidth("00 : 00");
}

void NGUITimeSelector::TimeLineScroller::SetMinMax(int min,int max)
{
    vIntervals.clear();
    vIntervals.push_back(Interval(min,max));
    nMax = max;
    nMin = min;
    BuildTimeData(min,max,vTimes);
}

void NGUITimeSelector::TimeLineScroller::BuildTimeData(int min,int max,TTimes& vTimes)
{
    vTimes.clear(); 
    vTimes.resize(1);
    if(max > min)
    {
        for(int i = min; i < 1440;i += nStep)
            vTimes[0].push_back(i);
    }
    else
        if(max < min)
        {
            for(int i = min; i < 1440;i += nStep)
                vTimes[0].push_back(i);
            for(int i = 0; i <= max;i += nStep)
                vTimes[0].push_back(i);
        }
}

void NGUITimeSelector::TimeLineScroller::Draw(NGUITimeSelector* pOwner) const
{
    float x  = nX + (nWidth - nTextWidth)*0.5f;
    for(std::vector<Element>::const_iterator i = vData.begin();i != vData.end();++i)
    {
        char sText[4];
        const Element& element = (*i);
        if(element.nData < vTimes[nCurList].size())
        {
            bool bValide = false;
            for(unsigned i = 0;i < vIntervals.size();++i)
                if( vTimes[nCurList][element.nData] >= vIntervals[i].nMin &&  vTimes[nCurList][element.nData] <= vIntervals[i].nMax)
                {
                    bValide = true;
                    break;
                }
            int nHour = vTimes[nCurList][element.nData] / 60;
            int nMinute = vTimes[nCurList][element.nData] - nHour * 60;
            if(nHour < 10)
                String::Printf(sText, 4, "0%i",nHour);
            else
                String::Printf(sText, 4, "%i",nHour);
            string sResult = sText;
            sResult += " : ";
            if(nMinute < 10)
                String::Printf(sText, 4, "0%i",nMinute);
            else
                String::Printf(sText, 4, "%i",nMinute);
            sResult += sText;
            pOwner->PrintText(this,x ,element.nPos,sResult.c_str(),bValide);
        }
    }
}


void NGUITimeSelector::TimeLineScroller::SetStep(int step)
{
    nStep = step;
    vIntervals.clear();
    BuildTimeData(nMin,nMax,vTimes);
    SetTime(pOwner->nHour * 60 + pOwner->nMinute);
}

void NGUITimeSelector::TimeLineScroller::OnStopScroll(int time)
{
    int hour = time / 60;
    pOwner->SetTime(hour,time - hour * 60);
}
