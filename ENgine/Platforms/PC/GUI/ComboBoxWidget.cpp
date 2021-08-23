
#include "ComboBoxWidget.h"

#include "Core/ICore.h"

ComboBoxWidget::ComboBoxWidget(int set_id, BaseWidget* prnt, const char* txt, float set_x, float set_y, float w, float h, bool sort,EAutoSizeType sizerType, bool bEdit) : BaseWidget(set_id, prnt, txt, sizerType)
{
	x = set_x;
	y = set_y;
	width = w;
	height = h;

	dword flag = CBS_DROPDOWNLIST;
    if(bEdit)
        flag = CBS_DROPDOWN | CBS_AUTOHSCROLL;
	if (sort)
		flag |= CBS_SORT;


	handle = CreateWindow( "COMBOBOX",txt,WS_CHILD|WS_BORDER | WS_VISIBLE | WS_VSCROLL | flag, x, y, width, height, parent->GetHandle(), NULL, NULL, NULL );
	SetWindowLong(handle, GWL_ID, id);


	MakeSubClassing();

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(handle, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));	
}

ComboBoxWidget::~ComboBoxWidget()
{
}

void ComboBoxWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
	int prm = LOWORD(wParam);

	if (msg == WM_COMMAND)
    {
        if(HIWORD(wParam) == CBN_SELCHANGE)
	    {		
		    if (id == LOWORD(wParam) || prm == 1000)
		    {
			    GetTextFromWidget();			
			    EventsWidget::SetEvent(id, EventsWidget::combobox_change);
		    }
	    }
        else
            if(HIWORD(wParam) == EN_CHANGE)
            {
               GetTextFromWidget();		
               EventsWidget::SetEvent(id, EventsWidget::combobox_change);		
            }

    }
   
}

void ComboBoxWidget::ClearList()
{
	ComboBox_ResetContent(handle);
}

void ComboBoxWidget::AddString(const char* str)
{
    int index = ComboBox_FindString(handle, -1, str);
	if (index == CB_ERR)
		ComboBox_AddString(handle, str );
}

void ComboBoxWidget::SetCurString(int index)
{
	ComboBox_SetCurSel(handle, index);
	GetTextFromWidget();
}

void ComboBoxWidget::SetCurString(const char* str)
{	
	ComboBox_SelectString(handle, 0, str);
	GetTextFromWidget();
}

int ComboBoxWidget::GetCurString()
{
	return ComboBox_GetCurSel(handle);
}

void ComboBoxWidget::SetText(const char* txt)
{
	int index = ComboBox_FindString(handle, -1, txt);

	if (index != CB_ERR)
		SetCurString(index);
}
void ComboBoxWidget::ClearText()
{
    ComboBox_SetText(handle,"");
    text = "";
}
void ComboBoxWidget::GetTextFromWidget()
{
	int len = ComboBox_GetTextLength(handle);
	//char* buffer = (char*)malloc(len + 10);

    text.resize(len + 10);
	ComboBox_GetText(handle,&text[0], len + 5);

//	text = buffer;
	
	//free(buffer);			
}

int ComboBoxWidget::GetIndex(const char* txt)
{
   return ComboBox_FindString(handle, -1, txt);
}

const char* ComboBoxWidget::GetText()
{
    GetTextFromWidget();
    return text.c_str();
}
