
#include "ListBoxWidget.h"
#include "Core\ICore.h"

ListBoxWidget::ListBoxWidget(int set_id, BaseWidget* prnt, const char* txt, float set_x, float set_y, float w, float h, bool alphabet_order, bool multi_select, EAutoSizeType sizerType) : BaseWidget(set_id, prnt, txt, sizerType)
{
	is_multi_select = multi_select;

	x = set_x;
	y = set_y;
	width = w;
	height = h;
	
	int style = 0;

	if (alphabet_order)
	{
		style |= LBS_SORT;
	}

	if (multi_select)
	{
		style |= LBS_MULTIPLESEL;
	}

	std::wstring wtext;
	String::Utf8toUtf16(wtext, txt);

	handle = CreateWindowW( L"ListBox",  wtext.c_str(), WS_CHILD| WS_VSCROLL | WS_VISIBLE | LBS_NOTIFY | style, x, y, width, height, parent->GetHandle(), NULL, NULL, NULL );
	
	SetWindowLong(handle, GWL_ID, id);

    MakeSubClassing();

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(handle, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

	sel_count = 0;
	sel_buffer = 0;
}

ListBoxWidget::~ListBoxWidget()
{
	if (sel_buffer)
	{
		delete[] sel_buffer;
	}
}

void ListBoxWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_COMMAND )
    {
        if (HIWORD (wParam) == LBN_SELCHANGE)
		{
            EventsWidget::SetEvent(id, EventsWidget::listbox_sel_change,this);
		}
        else
		{
            if (HIWORD (wParam) == LBN_DBLCLK)
			{
                EventsWidget::SetEvent(id, EventsWidget::listbox_double_click,this);
			}
		}
    }
}

void ListBoxWidget::ClearList()
{
	while (SendMessage(handle, LB_GETCOUNT, 0, 0) != 0)
	{
		SendMessage(handle, LB_DELETESTRING, 0, 0);
	}   
}

int ListBoxWidget::AddString(const char* str, int meta_data)
{

	std::wstring wtext;
	String::Utf8toUtf16(wtext, str); 

    int sel = SendMessageW(handle, LB_ADDSTRING, 0, (LPARAM)wtext.c_str()); 
    
	SendMessage(handle, LB_SETITEMDATA ,(WPARAM) sel, meta_data); 	
    SendMessage(handle, LB_SETTOPINDEX ,(WPARAM) sel, 0); 	
	return sel;
}

void ListBoxWidget::SelectText(const string& sText)
{
    unsigned count = SendMessage(handle, LB_GETCOUNT, 0, 0);
    
	for (unsigned i = 0; i < count; ++i)
    {
        char vBuffer[512];
        SendMessage(handle, LB_GETTEXT, (WPARAM)i , (LPARAM)vBuffer);
        if(sText == string(vBuffer))
        {
            SendMessage(handle, LB_SETCURSEL,(WPARAM)i,(LPARAM)i);
            break;
        }
    }
}

int ListBoxWidget::GetSelectedItemsCount()
{
	if (!is_multi_select)
	{
		if (ListBox_GetCurSel(handle) != -1)
		{
			return 1;
		}

		return 0;
	}

	return ListBox_GetSelCount(handle);
}

int ListBoxWidget::GetSelectedItem(int index, string& sText)
{
	int meta_data = 0;
	return GetSelectedItem(index, sText, meta_data);
}

int ListBoxWidget::GetSelectedItem(int index, string& sText, int& meta_data)
{	
	int item_index = -1;

	if (!is_multi_select)
	{
		item_index = ListBox_GetCurSel(handle);				
	}
	else
	{
		if (sel_count <= index)
		{
			if (sel_count > 0)
			{
				delete[] sel_buffer;
			}

			sel_count = index + 5;
			sel_buffer = new int[sel_count];
		}
		
		ListBox_GetSelItems(handle, sel_count, sel_buffer);

		item_index = sel_buffer[index];
	}

	if (item_index != -1)
	{
		char vBuffer[512];
		SendMessage(handle, LB_GETTEXT, (WPARAM)item_index , (LPARAM)vBuffer);
		meta_data = static_cast<int>(SendMessage(handle, LB_GETITEMDATA, (WPARAM)item_index , 0));		
		sText = vBuffer;
	}

	return item_index;
}

int ListBoxWidget::GetSelectedIndex(int index)
{
    int item_index = -1;

    if (!is_multi_select)
    {
        item_index = ListBox_GetCurSel(handle);				
    }
    else
    {
        if (sel_count <= index)
        {
            if (sel_count > 0)
            {
                delete[] sel_buffer;
            }

            sel_count = index + 5;
            sel_buffer = new int[sel_count];
        }

        ListBox_GetSelItems(handle, sel_count, sel_buffer);

        item_index = sel_buffer[index];
    }
    return item_index;
}
