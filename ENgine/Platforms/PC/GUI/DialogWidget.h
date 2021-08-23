#pragma  once
#include "BaseWidget.h"
class LabelWidget;
class ButtonWidget;

#define DIALOG_OK_PRESS         WM_USER + 1
#define DIALOG_CANSEL_PRESS     WM_USER + 2 

class DEF_EXPORT DialogWidget : public BaseWidget
{	
    ButtonWidget*   pCloseButton;
    ButtonWidget*   pOkButton;
    BaseWidget*     pListener;
    HWND            hBack; 
    bool            bActive; 

    void Close(int code);

public:
    enum DialogStyles{eButtonOk = 1,eButtonCancel = 2};

    DialogWidget(int id, const char* txt, float x, float y, float w, float h,int eStyle = (DialogStyles::eButtonOk | DialogStyles::eButtonCancel));
    virtual ~DialogWidget(){}

    virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);
    void SetListener( BaseWidget* listener){pListener = listener;}
    void OkPress(){Close(DIALOG_OK_PRESS);}
    void CancelPress(){Close(DIALOG_CANSEL_PRESS);}
    void SetSize(float w,float h);
    void SetId(int id);
};