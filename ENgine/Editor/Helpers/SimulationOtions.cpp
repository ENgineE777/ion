#ifdef EDITOR
#include "SimulationOtions.h"
#include "Platforms/PC/GUI/WindowWidget.h"
#include "Platforms/PC/GUI/SizerWidget.h"
#include "Platforms/PC/GUI/ListBoxWidget.h"
#include "Platforms/PC/GUI/ButtonWidget.h"
#include "Platforms/PC/GUI/LabelWidget.h"
#include "Platforms/PC/GUI/CheckBoxWidget.h"
#include "Platforms/PC/GUI/ComboBoxWidget.h"
#include "Platforms/PC/GUI/TextBoxWidget.h"
#include "Core/ICore.h"
#include "Core/Script/ScriptModuleCore.h"


char SimulationOtions::Device::sBuffer[512]  = {0};

const char* SimulationOtions::Device::GetResolution()
{
    String::Printf(sBuffer,512,"Resolution: %d x %d scale = %.2f",nWidth,nHeight,nScale);
    return sBuffer;
}

const char* SimulationOtions::Device::ToStirng()
{
    String::Printf(sBuffer,512,"{%s,%d,%d,%f}",sName.c_str(),nWidth,nHeight,nScale);
    return sBuffer;
}



SimulationOtions::~SimulationOtions()
{
    SaveDevices();
    if(pWindow)
        delete pWindow;
    if(pAddWindow)
        delete pAddWindow;
}

void SimulationOtions::Show(bool bFlag)
{
    if(pWindow)
        pWindow->Show(bFlag);
}

SimulationOtions::SimulationOtions() :pWindow(NULL),pDevicesList(NULL),pTimeSimulationFlag(NULL),pCurrentDivice(NULL),pYear(NULL),pMonth(NULL),pMinute(NULL),pDay(NULL),pHour(NULL),pDPI(NULL)
{
    Init();
    pAddWindow = new AddDeviceWindow();
}

static char* vMonths[] = {"January","February","March","April","May","June","July","August","September","October","November","December"};

void SimulationOtions::Init()
{
    if(pWindow)
        delete pWindow;
    pWindow = new WindowWidget(SIMULATION_OPTIONS_WINDOW_ID, "Simulation Options", false, true, 100, 100, 350, 240);

    SizerWidget* left_sizer = new SizerWidget(1000, pWindow, false,-1);
    SizerWidget* right_sizer = new SizerWidget(1000, pWindow, false,150);

    SizerWidget* devices_top_sizer = new SizerWidget(1000, left_sizer, true,25);			
    SizerWidget* devices_midle_sizer = new SizerWidget(1000, left_sizer, true,-1);			
    SizerWidget* devices_bottom_sizer = new SizerWidget(1000, left_sizer, true, 30);

    pDevicesList = new ListBoxWidget(SIMULATION_OPTIONS_DEVICE_LIST_ID, devices_midle_sizer, "list", 0,0, 260, 210, false, false, BaseWidget::eAutoSizeBoth);

    SizerWidget* right_sizer_top = new SizerWidget(1000, right_sizer, true, -1);
    SizerWidget* right_sizer_bottom = new SizerWidget(1000, right_sizer, true, 30);

    new SizerWidget(1000, right_sizer_bottom, false, -1);
    SizerWidget* right_sizer_bottomL = new SizerWidget(1000, right_sizer_bottom, false, 60);

    new ButtonWidget(SIMULATION_OPTIONS_DEVICE_ADD_ID, devices_bottom_sizer, false, "Add", 10, 5, 50, 20);
    new ButtonWidget(SIMULATION_OPTIONS_EDIT_ID, devices_bottom_sizer, false, "Edit", 70, 5, 50, 20);
    new ButtonWidget(SIMULATION_OPTIONS_DEVICE_DELETE_ID, devices_bottom_sizer, false, "Delete", 130, 5, 50, 20);

    new ButtonWidget(SIMULATION_OPTIONS_OK_ID, right_sizer_bottomL, false, "Ok", 5, 5, 50, 20);

    

    pCurrentDivice = new  LabelWidget(445, devices_top_sizer, "Label", false,5, 5, 500, 20);	

    LabelWidget* pLabel = new  LabelWidget(1000, right_sizer_top, "Label", false,10,28,20,20);
    pLabel->SetText("DPI:");
    pDPI  = new TextBoxWidget(SIMULATION_OPTIONS_DPI_ID, right_sizer_top,"",35,25,35,18);


    pTimeSimulationFlag = new CheckBoxWidget(SIMULATION_OPTIONS_TIME_ID, right_sizer_top, "Time simulation",10,50, 200, 25);

    pYear = CreateComboBox("Year:",SIMULATION_OPTIONS_YEAR_ID,right_sizer_top,10,80);
    nYear = GetTime(5);
    int year =  static_cast<int>(nYear) + 50;
    char sBuffer[8];
    for(unsigned i = 0; i < 100;++i,--year)
    {
        String::Printf(sBuffer,8,"%d",year);
        pYear->AddString(sBuffer);
    }
    pYear->SetCurString(50);
    pMonth = CreateComboBox("Month:",SIMULATION_OPTIONS_MONTH_ID,right_sizer_top,10,105);
    for(int i = 0;i < 12;++i)
    {
        pMonth->AddString(vMonths[i]);
    }
    pDay = CreateComboBox("Day:",SIMULATION_OPTIONS_DAY_ID,right_sizer_top,10,130);
    nMonth = GetTime(4);
    nDay = GetTime(3);
    nHour = GetTime(0);
    nMinute = GetTime(1);
    pHour = CtreateTextBox("Hour:",SIMULATION_OPTIONS_HOUR_ID,right_sizer_top,10,155);
    pMinute = CtreateTextBox("Minute:",SIMULATION_OPTIONS_HOUR_ID,right_sizer_top,10,180);
    SetCurDate(nYear,nMonth,nDay);
    SetTime(pHour,nHour);
    SetTime(pMinute,nMinute);
    pWindow->Show(false);
    nDPI = ReadINI("settings.ini",160, "Simulation", "dpi");
    itoa(nDPI,sBuffer,10);
    pDPI->SetText(sBuffer);    
    string sDevises =  ReadINI("settings.ini", "", "Simulation", "Devises");
    nSelectedDevice = ReadINI("settings.ini", 0, "Simulation", "Selected");
    vDevices.clear();
    if(sDevises.empty())
        vDevices.push_back(Device("Default",384,682,1.));
    else
        ParseDevices(sDevises);

    InitDevices();

}

TextBoxWidget* SimulationOtions::CtreateTextBox(const char* sName,int nId,BaseWidget* pParent,int x,int y)
{
    LabelWidget* pLabel = new  LabelWidget(1000, pParent, "Label", false,x,y + 3,35,20);
    pLabel->SetText(sName);
    return new TextBoxWidget(nId, pParent, "",x + 38,y, 80,18, BaseWidget::eAutoSizeNone,TextBoxWidget::eUnsigned);
}
ComboBoxWidget* SimulationOtions::CreateComboBox(const char* sName,int nId,BaseWidget* pParent,int x ,int y)
{
    LabelWidget* pLabel = new  LabelWidget(1000, pParent, "Label", false,x,y + 3,35,20);
    pLabel->SetText(sName);
    return  new ComboBoxWidget(nId, pParent, "",x + 38,y,80,200,false);
}

void SimulationOtions::SetCurDate(float nYear,float nMonth,float nDay)
{
    pMonth->SetCurString(static_cast<int>(nMonth) - 1);
    char sBuffer[5];
    int nDaysCount = 0;
    float nYearTail = (nYear / 4.f) - static_cast<int>(nYear / 4.f);
    float nMonthTail = (nMonth / 2.f) - static_cast<int>(nMonth / 2.f);
    if(nMonthTail != 0)nDaysCount = 31;
    else
        nDaysCount = 30;
    if(nMonth == 2)
    {
        if(nYearTail == 0)nDaysCount = 29;
        else nDaysCount = 28;
    }
    pDay->ClearList();
    for(int i = 0;i < nDaysCount;++i)
    {
        String::Printf(sBuffer,5,"%d",i + 1);
        pDay->AddString(sBuffer);
    }
    if(nDay > nDaysCount)
    {
        this->nDay = nDaysCount;
        pDay->SetCurString(nDaysCount - 1);
    }
    else
      pDay->SetCurString(static_cast<int>(nDay - 1));
    core->Script()->GetCoreModule()->SetSimulationTime(nYear,nMonth,nDay,nHour,nMinute);
}

bool SimulationOtions::ProcessEvents()
{
    if(EventsWidget::CheckEvent(SIMULATION_OPTIONS_WINDOW_ID, EventsWidget::window_close))
    {
        SaveDevices();
        Init();
        return true;
    }
    if(EventsWidget::CheckEvent(SIMULATION_OPTIONS_WINDOW2_ID, EventsWidget::window_close))
    {
        if(pAddWindow)
            pAddWindow->Init();
        return true;
    }
    if(EventsWidget::CheckEvent(SIMULATION_OPTIONS_TIME_ID, EventsWidget::checkbox_click))
    {
        core->Script()->GetCoreModule()->SetTimeSimulation(pTimeSimulationFlag->IsChecked());
        core->Script()->GetCoreModule()->SetSimulationTime(nYear,nMonth,nDay,nHour,nMinute);
        return true;
    }
    if (pYear && EventsWidget::CheckEvent(pYear->GetID(), EventsWidget::combobox_change))
    {
        nYear = atof(pYear->GetText());
        SetCurDate(nYear,nMonth,nDay);
        return true;
    }
    if (pMonth && EventsWidget::CheckEvent(pMonth->GetID(), EventsWidget::combobox_change))
    {
        nMonth = pMonth->GetIndex(pMonth->GetText()) + 1;
        SetCurDate(nYear,nMonth,nDay);
        return true;
    }
    if (pDay && EventsWidget::CheckEvent(pDay->GetID(), EventsWidget::combobox_change))
    {
        nDay = atof(pDay->GetText());
        core->Script()->GetCoreModule()->SetSimulationTime(nYear,nMonth,nDay,nHour,nMinute);
        return true;
    }
    if (pHour && EventsWidget::CheckEvent(pHour->GetID(), EventsWidget::textbox_change))
    {
        nHour = atof(pHour->GetText());
        core->Script()->GetCoreModule()->SetSimulationTime(nYear,nMonth,nDay,nHour,nMinute);
        return true;
    }
    if (pMinute && EventsWidget::CheckEvent(pMinute->GetID(), EventsWidget::textbox_change))
    {
        nMinute = atof(pMinute->GetText());
        core->Script()->GetCoreModule()->SetSimulationTime(nYear,nMonth,nDay,nHour,nMinute);
        return true;
    }
    if (pDPI && EventsWidget::CheckEvent(pDPI->GetID(), EventsWidget::textbox_change))
    {
        nDPI = atoi(pDPI->GetText());
        core->Render()->Display()->SetScreenDPI(nDPI);	
        WriteINI("settings.ini",nDPI, "Simulation", "dpi");
        return true;
    }
    if(EventsWidget::CheckEvent(SIMULATION_OPTIONS_DEVICE_ADD_ID, EventsWidget::button_click))
    {
        if(pAddWindow && !pAddWindow->IsVisible())
            pAddWindow->Show(true);
		return true;
    }
    if(EventsWidget::CheckEvent(SIMULATION_OPTIONS_CANCEL_ADD_ID, EventsWidget::button_click))
    {
        if(pAddWindow)
            pAddWindow->Show(false);
		return true;
    }
    if(EventsWidget::CheckEvent(SIMULATION_OPTIONS_OK_ID, EventsWidget::button_click))
    {
        SaveDevices();
        Show(false);
		return true;
    }
    if(EventsWidget::CheckEvent(SIMULATION_OPTIONS_DEVICE_REAL_ADD_ID, EventsWidget::button_click))
    {
        if(pAddWindow)
        {
            if(pAddWindow->IsValidInfo())
            {
				if(pAddWindow->IsEdit())
				{
					vDevices[nSelectedDevice].nWidth = pAddWindow->GetWidth();
					vDevices[nSelectedDevice].nHeight = pAddWindow->GetHeight();
					vDevices[nSelectedDevice].nScale = pAddWindow->GetScale();
					SelectDevice(nSelectedDevice);
				}
				else
				{
					vDevices.push_back(Device(pAddWindow->GetName(),pAddWindow->GetWidth(),pAddWindow->GetHeight(),pAddWindow->GetScale()));
					pDevicesList->AddString(pAddWindow->GetName().c_str(),0);
				}
                pAddWindow->Show(false);
            }
        }
		return true;

    }
    if(EventsWidget::CheckEvent(SIMULATION_OPTIONS_DEVICE_LIST_ID, EventsWidget::listbox_sel_change))
    {
       int nIndex = pDevicesList->GetSelectedIndex();	
       if(nIndex >= 0 && nIndex < vDevices.size())
       {
           SelectDevice(nIndex);
       }
	   return true;
    }
    if(EventsWidget::CheckEvent(SIMULATION_OPTIONS_DEVICE_DELETE_ID, EventsWidget::button_click))
    {
        int nIndex = pDevicesList->GetSelectedIndex();	
        if(nIndex >= 1 && nIndex < vDevices.size())
        {
            vDevices.erase(vDevices.begin() + nIndex);
            --nSelectedDevice;
            SelectDevice(nSelectedDevice);
            SaveDevices();
        }
		return true;
    }
	if(EventsWidget::CheckEvent(SIMULATION_OPTIONS_EDIT_ID, EventsWidget::button_click) || EventsWidget::CheckEvent(SIMULATION_OPTIONS_DEVICE_LIST_ID, EventsWidget::listbox_double_click))
	{
		if(pAddWindow)
			pAddWindow->Edit(vDevices[nSelectedDevice].sName,vDevices[nSelectedDevice].nWidth,vDevices[nSelectedDevice].nHeight,vDevices[nSelectedDevice].nScale);
		return true;
	}

    return false;
}

void SimulationOtions::SetTime(TextBoxWidget* pBox,float nTime)
{
    char sBuffer[5];
    String::Printf(sBuffer,5,"%d",static_cast<int>(nTime));
    pBox->SetText(sBuffer);
}

void SimulationOtions::ParseDevices(const string& sDevices)
{
    string sBuffer;
    unsigned state = 0;
    Device device;
    for(unsigned i = 0; i < sDevices.size();++i)
    {
        switch(sDevices[i])
        {
            case '{':
            {
               state = 1;  
               sBuffer.clear();
               break;
            }
            case ',':
            {
                switch(state)
                {
                    case 1:device.sName = sBuffer;break;
                    case 2:device.nWidth = atoi(sBuffer.c_str());break;
                    case 3:device.nHeight = atoi(sBuffer.c_str());break;
                }
                sBuffer.clear();
                ++state;
                break;
            }
            case '}':
            {
                switch(state)
                {
                   case 3:device.nHeight = atoi(sBuffer.c_str());break;
                   case 4:device.nScale = atof(sBuffer.c_str());break;
                }
                state = 0;
                vDevices.push_back(device);
                break;
            }
            default:sBuffer += sDevices[i];
        }
    }
}

void SimulationOtions::InitDevices()
{
    pDevicesList->ClearList();
    for(unsigned i = 0;i < vDevices.size();++i)
        pDevicesList->AddString(vDevices[i].sName.c_str(),0);
    SelectDevice(nSelectedDevice);
}

void SimulationOtions::SelectDevice(int nIndex)
{
    if(nIndex < vDevices.size())
    {
        nSelectedDevice = nIndex;
        WriteINI("settings.ini", nSelectedDevice, "Simulation", "Selected");
        
    }
    pDevicesList->SelectText(vDevices[nSelectedDevice].sName);
    pCurrentDivice->SetText(vDevices[nSelectedDevice].GetResolution());
}

void SimulationOtions::SaveDevices()
{
    string sStr;
    for(unsigned i = 0;i < vDevices.size();++i)
        sStr += vDevices[i].ToStirng();
    WriteINI("settings.ini",sStr.c_str(), "Simulation", "Devises");
    WriteINI("settings.ini",nDPI, "Simulation", "dpi");
    WriteINI("settings.ini", nSelectedDevice, "Simulation", "Selected");
}

bool SimulationOtions::AddDeviceWindow::IsValidInfo()
{
    if(pName && pWidth && pHeight && pScale)
    {
        sName = pName->GetText();
        if(!sName.empty()  && !String::IsEmpty(pWidth->GetText()) && !String::IsEmpty(pHeight->GetText()))
        {
            nWidth = atoi(pWidth->GetText());
            nHeight = atoi(pHeight->GetText());
            nScale = atof(pScale->GetText());
            return true;
        }
    }
    return false;
}

void SimulationOtions::AddDeviceWindow::Show(bool bFlag)
{
    if(bFlag)
    {
        if(!pWindow)
            Init();
		bEdit = false;
		pName->Enable(true);
		pName->SetText("");
		pWidth->SetText("");
		pHeight->SetText("");
		pScale->SetText("1.0");
		pButton->SetText("Add");
		pWindow->SetText("Add New Device");
    }
    if(pWindow)
        pWindow->Show(bFlag);
}

void SimulationOtions::AddDeviceWindow::Init()
{
    if(pWindow)
        delete pWindow;
    pWindow = new WindowWidget(SIMULATION_OPTIONS_WINDOW2_ID, "Add New Device", false, true, 250, 250, 450, 60);
    SizerWidget* devices_top_sizer = new SizerWidget(1000, pWindow, true,-1);			
    SizerWidget* devices_bottom_sizer = new SizerWidget(1000, pWindow, true, 30);

    SizerWidget* devices_topL_sizer = new SizerWidget(1000, devices_top_sizer, false,-1);
    SizerWidget* devices_topR_sizer = new SizerWidget(1000, devices_top_sizer, false,285);

    pName = CtreateTextBox("Name:",SIMULATION_OPTIONS_WINDOW2_ID,devices_topL_sizer,5,5,BaseWidget::eAutoSizeWidth,TextBoxWidget::eText);
    pWidth =  CtreateTextBox("Width:",SIMULATION_OPTIONS_WINDOW2_ID,devices_topR_sizer,5,5);
    pHeight =  CtreateTextBox("Height:",SIMULATION_OPTIONS_WINDOW2_ID,devices_topR_sizer,97,5);
    pScale =  CtreateTextBox("Scale:",SIMULATION_OPTIONS_WINDOW2_ID,devices_topR_sizer,190,5,BaseWidget::eAutoSizeNone,TextBoxWidget::eUFloat);

    new SizerWidget(1000, devices_bottom_sizer, false,-1);			
    SizerWidget* devices_bottomR_sizer = new SizerWidget(1000, devices_bottom_sizer, false, 130);
	
    pButton = new ButtonWidget(SIMULATION_OPTIONS_DEVICE_REAL_ADD_ID, devices_bottomR_sizer, false, "Add", 10, 5, 50, 20);
    new ButtonWidget(SIMULATION_OPTIONS_CANCEL_ADD_ID, devices_bottomR_sizer, false, "Cancel", 70, 5, 50, 20);
}

TextBoxWidget* SimulationOtions::AddDeviceWindow::CtreateTextBox(const char* sName,int nId,BaseWidget* pParent,int x,int y,int sizerType ,int eType)
{
    LabelWidget* pLabel = new  LabelWidget(1000, pParent, "Label", false,x,y + 3,35,20);
    pLabel->SetText(sName);
    return new TextBoxWidget(nId, pParent, "",x + 38,y,50,18,static_cast<BaseWidget::EAutoSizeType>(sizerType),static_cast<TextBoxWidget::EType>(eType));
}

bool SimulationOtions::AddDeviceWindow::IsVisible()
{
    if(pWindow)
        return pWindow->IsVisible();
    return false;
}

void SimulationOtions::AddDeviceWindow::Edit(const string& sName,int nWidth,int nHeight,float nScale)
{
	if(!pWindow)
		Init();

	pWindow->SetText("Edit Device");

	pButton->SetText("Ok");
	pName->SetText(sName.c_str());
	pName->Enable(false);

	static char sBuffer[32];

	String::Printf(sBuffer,32,"%i",nWidth);
	pWidth->SetText(sBuffer);

	String::Printf(sBuffer,32,"%i",nHeight);
	pHeight->SetText(sBuffer);

	String::Printf(sBuffer,32,"%.2f",nScale);
	pScale->SetText(sBuffer);

	bEdit = true;

	pWindow->Show(true);
}

#endif
