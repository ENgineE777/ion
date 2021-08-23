#pragma once
#ifdef EDITOR
#include "Common/Utils.h"
class WindowWidget;
class ListBoxWidget;
class LabelWidget;
class ComboBoxWidget;
class BaseWidget;
class TextBoxWidget;
class CheckBoxWidget;
class ButtonWidget;

class SimulationOtions
{
    class AddDeviceWindow
    {
            WindowWidget*       pWindow;
            TextBoxWidget*      pName;
            TextBoxWidget*      pHeight;
            TextBoxWidget*      pWidth;
            TextBoxWidget*      pScale;
			ButtonWidget*		pButton;	
            int                 nWidth;
            int                 nHeight;
            float               nScale;
            string              sName;
			bool				bEdit;

            
            TextBoxWidget*      CtreateTextBox(const char* sName,int nId,BaseWidget* pParent,int x,int y,int sizerType = 0,int eType = 3);
    public:
        AddDeviceWindow():pWindow(NULL),pName(NULL),pHeight(NULL),pWidth(NULL),pScale(NULL),nWidth(0),nHeight(0),nScale(1),bEdit(false)
        {}
        ~AddDeviceWindow()
        {
            if(pWindow)
                delete pWindow;
        }
        void Show(bool bFlag);
		void Edit(const string& sName,int nWidth,int nHeight,float nScale);
        void Init();
        bool IsVisible();
		bool IsValidInfo();

		inline bool		IsEdit(){return bEdit;}
        inline int		GetWidth(){return nWidth;}
        inline int		GetHeight(){return nHeight;}
        inline float	GetScale(){return nScale;}

        inline const string&  GetName(){return sName;}

    };


    WindowWidget*   pWindow;
    ListBoxWidget*  pDevicesList;
    LabelWidget*    pCurrentDivice;
    ComboBoxWidget* pYear;
    ComboBoxWidget* pMonth;
    ComboBoxWidget* pDay;
    TextBoxWidget*  pMinute;
    TextBoxWidget*  pHour;
    CheckBoxWidget* pTimeSimulationFlag;
    TextBoxWidget*  pDPI;

    AddDeviceWindow* pAddWindow;


    ComboBoxWidget*  CreateComboBox(const char* sName,int nId,BaseWidget* pParent,int x ,int y);
    TextBoxWidget*   CtreateTextBox(const char* sName,int nId,BaseWidget* pParent,int x,int y);
    void             SetCurDate(float nYear,float nMonth,float nDay);
    void             SetTime(TextBoxWidget* pBox,float nTime);


    float  nYear;
    float  nMonth;
    float  nDay;
    float  nHour;
    float  nMinute;
    int    nDPI;

    struct Device
    {
        string sName;
        int    nHeight;
        int    nWidth;
        float  nScale;

        Device():nHeight(0),nWidth(0),nScale(1)
        {}
        Device(const string& _Name,int _Width,int _Height,float _Scale):nHeight(_Height),nWidth(_Width),sName(_Name),nScale(_Scale)
        {}
        Device(const Device& other){Copy(other);}
        const Device& operator = (const Device& other) {Copy(other); return *this;}
        const char* GetResolution();
        const char* ToStirng();

        inline int Width(){return static_cast<int>(nWidth * nScale);}
        inline int Height(){return static_cast<int>(nHeight * nScale);}
    private:
        void Copy(const Device& other)
        {
            sName = other.sName;
            nWidth = other.nWidth;
            nHeight = other.nHeight;
            nScale = other.nScale;
        }
        static char sBuffer[512];

    };
    typedef std::vector<Device> TDevices;

    TDevices  vDevices;
    int       nSelectedDevice;

    void ParseDevices(const string& sDevices);
    void InitDevices();
    void SelectDevice(int nIndex);
    void SaveDevices();

public:
    SimulationOtions();

    ~SimulationOtions();

    void Show(bool bFlag);

    void Init();

    bool ProcessEvents();

    inline int  GetCurrentWidth(){return vDevices[nSelectedDevice].Width();}
    inline int  GetCurrentHeight(){return vDevices[nSelectedDevice].Height();}

    
};

#endif