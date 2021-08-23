
#include "SizerWidget.h"


SizerWidget::SizerWidget(int set_id, BaseWidget* prnt, bool is_vert_sizer, int sz) : BaseWidget(set_id, prnt, "Sizer", BaseWidget::eAutoSizeNone)
{
	vert_sizer = is_vert_sizer;

	size = sz;

	x = 0;
	y = 0;
	
	width = 0;
	height = 0;

	dword flag = 0;	

	handle = CreateWindow("STATIC","",SS_LEFT|WS_CHILD | WS_VISIBLE | flag| SS_NOTIFY,x,y,width,height,parent->GetHandle(),NULL, NULL, NULL);
			
	SetWindowLong(handle, GWL_ID, id);

	MakeSubClassing();
}

SizerWidget::~SizerWidget()
{

}

void SizerWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
	if (WM_COMMAND != msg) return;
	if (id != LOWORD(wParam)) return;
	
}

bool SizerWidget::IsVertical()
{
	return vert_sizer;
}

int  SizerWidget::GetSize()
{
	return size;
}

void SizerWidget::Resize()
{
	int set_width = parent->GetWidth();
	int set_height = parent->GetHeight();

	int num = 0;

	for (int i = 0; i < parent->GetChildsCount(); i++)
	{
		BaseWidget* wgt = parent->GetChild(i);

		if (String::IsEqual(wgt->GetText(), "Sizer"))
		{
			SizerWidget* wgt_sizer = (SizerWidget*)wgt;

			if (vert_sizer != wgt_sizer->IsVertical()) continue;
			
			if (wgt_sizer->GetSize() > 0)
			{
				if (vert_sizer)
				{
					set_height -= wgt_sizer->GetSize();										
				}
				else
				{
					set_width -= wgt_sizer->GetSize();										
				}

			}
			else
			{
				num++;
			}
		}
	}

	int elem_sz = 0;

	if (vert_sizer)
	{
		if (num > 0 && set_height > 0)
		{
			elem_sz = (int)((float)set_height/(float)num);
		}
	}
	else
	{
		if (num > 0 && set_width > 0)
		{
			elem_sz = (int)((float)set_width/(float)num);
		}
	}

	int set_x = parent->GetXOffset();
	int set_y = parent->GetYOffset();

	for (int i = 0; i < parent->GetChildsCount(); i++)
	{		
		BaseWidget* wgt = parent->GetChild(i);

		if (wgt == this) break;

		if (String::IsEqual(wgt->GetText(), "Sizer"))
		{
			SizerWidget* wgt_sizer = (SizerWidget*)wgt;

			if (vert_sizer != wgt_sizer->IsVertical()) continue;

			if (wgt_sizer->GetSize() > 0)
			{
				if (vert_sizer)
				{
					set_y += wgt_sizer->GetSize();										
				}
				else
				{
					set_x += wgt_sizer->GetSize();										
				}				
			}
			else
			{
				if (vert_sizer)
				{
					set_y += elem_sz;
				}
				else
				{
					set_x += elem_sz;
				}
			}
		}
	}

	if (size > 0)
	{
		if (vert_sizer)
		{
			set_height = size;
		}
		else
		{
			set_width = size;
		}
	}
	else
	{
		if (vert_sizer)
		{
			set_height = elem_sz;
		}
		else
		{
			set_width = elem_sz;
		}
	}

	SetPos(set_x, set_y);
	SetSize(set_width, set_height);	

	BaseWidget::Resize();
}
