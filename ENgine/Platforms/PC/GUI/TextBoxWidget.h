
#pragma once

#include "BaseWidget.h"

class DEF_EXPORT TextBoxWidget : public BaseWidget
{
public:
    enum EType{eText,eUFloat,eFloat,eUnsigned,eInt};
    TextBoxWidget(int id, BaseWidget* parent, const char* name, float x, float y, float w, float h,EAutoSizeType sizerType = BaseWidget::eAutoSizeNone,EType type = eText);
    virtual ~TextBoxWidget();

    virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);
    virtual void SetText(const char* txt);
    virtual const char* GetText();
    virtual void SelectText(); 
    virtual inline EType Type(){return eType;}
private:
    EType eType;
    static LRESULT CALLBACK TextBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

};
