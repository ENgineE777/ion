#include "DialogWidget.h"
#include "ButtonWidget.h"
#include "LabelWidget.h"
#include "Editor/Editor.h"


DialogWidget::DialogWidget(int id, const char* txt, float x, float y, float w, float h,int eStyle): BaseWidget(id, core_ed.pMainWindow, "", BaseWidget::eAutoSizeNone),pListener(NULL),pOkButton(NULL),pCloseButton(NULL),bActive(false)
{

    if(w < 150) w = 150;
    if(h < 100) h = 100;
    width = w;
    height = h;
    handle = CreateWindow("ENgineWindow","",WS_OVERLAPPED |  WS_CAPTION ,x, y, w, h,core_ed.pMainWindow->GetHandle(),NULL, NULL, NULL);
    SetWindowLong(handle, GWL_ID, id);
   // parent->AddChild(this);
    MakeSubClassing();
    SetWindowText(handle,(LPCTSTR)txt);
    RECT rect;
    GetClientRect(handle,&rect);
    hBack = CreateWindow("STATIC","",WS_CHILD | WS_VISIBLE ,rect.left,rect.top,rect.right,rect.bottom,handle,NULL, NULL, NULL);
    if((eStyle & DialogStyles::eButtonOk) && (eStyle & DialogStyles::eButtonCancel))
    {
        pOkButton    = new ButtonWidget(DIALOG_BUTTON_OK_ID, this, false,"Ok",rect.right  - 120,rect.bottom - 25, 55, 22);
        pCloseButton = new ButtonWidget(DIALOG_CLOSE_BUTTON_ID, this, false,"Cancel", rect.right  - 60 ,rect.bottom - 25, 55, 22);
    }
    else
        if(eStyle & DialogStyles::eButtonOk)
            pOkButton  = new ButtonWidget(DIALOG_BUTTON_OK_ID, this, false,"Ok",rect.right  - 60,rect.bottom - 25, 55, 22);
        else
             if(eStyle & DialogStyles::eButtonCancel)
                 pCloseButton = new ButtonWidget(DIALOG_CLOSE_BUTTON_ID, this, false,"Cancel", rect.right  - 60 ,rect.bottom - 25, 55, 22);

}

void DialogWidget::SetId(int _id)
{
  SetWindowLong(handle, GWL_ID, _id);
  id = _id;
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) 
{
       std::vector<HWND>* pList = reinterpret_cast<std::vector<HWND>*>(lParam);
       pList->push_back(hwnd);
       return TRUE;
}


void DialogWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
    if ((WM_COMMAND == msg && HIWORD(wParam)==BN_CLICKED))
    {
        if (pOkButton && pOkButton->GetID() == LOWORD(wParam) )
            Close(DIALOG_OK_PRESS);
        else
            if (pCloseButton && pCloseButton->GetID() == LOWORD(wParam) )
                Close(DIALOG_CANSEL_PRESS);
    }
   /* else
        if(msg == WM_KILLFOCUS && bActive)
        {
            if(wParam)
            {
                std::vector<HWND> vList;
                EnumChildWindows(handle, EnumChildProc,(LPARAM)&vList);
                for(unsigned i = 0;i < vList.size();++i)
                    if(vList[i] == (HWND)wParam)
                        return;
            }
            SetFocus(handle);
        }
        else
            if(msg == WM_SETFOCUS)
                bActive = true;*/
}

void DialogWidget::Close(int code)
{
    bActive  = false;
    if(pListener)
        SendMessage(pListener->GetHandle(),code,GetID(),0);
    Show(false);
}

void DialogWidget::SetSize(float w,float h)
{
    width = w;
    height = h;
    SetWindowPos(handle, 0,0, 0, w,h, SWP_NOMOVE | SWP_NOZORDER);
    RECT rect;
    GetClientRect(handle,&rect);
    SetWindowPos(hBack, 0,0, 0, rect.right,rect.bottom, SWP_NOZORDER);
    if(pOkButton && pCloseButton)
    {
        SetWindowPos(pOkButton->GetHandle(), 0,rect.right  - 120,rect.bottom - 25,0,0, SWP_NOSIZE | SWP_NOZORDER);
        SetWindowPos(pCloseButton->GetHandle(), 0,rect.right  - 60,rect.bottom - 25,0,0, SWP_NOSIZE | SWP_NOZORDER);
        InvalidateRect(pOkButton->GetHandle(),NULL,FALSE);
        InvalidateRect(pCloseButton->GetHandle(),NULL,FALSE);
    }
    else
        if(pOkButton)
        {
            SetWindowPos(pOkButton->GetHandle(), 0,rect.right  - 120,rect.bottom - 25,0,0, SWP_NOSIZE | SWP_NOZORDER);
            InvalidateRect(pOkButton->GetHandle(),NULL,FALSE);
        }
        else
            if(pCloseButton)
            {
                SetWindowPos(pCloseButton->GetHandle(), 0,rect.right  - 60,rect.bottom - 25,0,0, SWP_NOSIZE | SWP_NOZORDER);
                InvalidateRect(pCloseButton->GetHandle(),NULL,FALSE);
            }
}


