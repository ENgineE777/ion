
#include "RolloutWidget.h"

RolloutWidget::RolloutWidget(int set_id, BaseWidget* prnt, const char* txt, float set_x, float set_y, float w, float h,EAutoSizeType sizerType) : BaseWidget(set_id, prnt, txt,sizerType)
{	
	x = set_x;
	y = set_y;
	width = w;
	height = h;

	
	handle = CreateWindow("STATIC", "Static", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | SS_NOTIFY, x, y, width, height, parent->GetHandle(), NULL, NULL, NULL);
			
	SetWindowLong(handle, GWL_ID, id);

	Static_SetText(handle, text.c_str());

	MakeSubClassing();

	color = Color(0.8f, 0.8f, 0.8f);

	hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(handle, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
}

RolloutWidget::~RolloutWidget()
{
}

void RolloutWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
}

void RolloutWidget::AddRolloutChild(const char* name, BaseWidget* widget)
{
	Rollout* rollout = null;

	for (int i = 0; i < rollouts.size(); i++)
	{
		if (String::IsEqual(rollouts[i].name, name))
		{
			rollout = &rollouts[i];
			break;
		}
	}

	if (!rollout)
	{
		rollouts.push_back(Rollout());
		rollout = &rollouts[rollouts.size() - 1];		
		String::Copy(rollout->name, 128, name);
		rollout->y = (rollouts.size() - 1) * 25;

		if (rollouts.size() == 1)
		{
			rollout->opened = true;
		}
		else
		if (rollouts.size() == 2)
		{
			rollouts[0].opened = false;
		}
	}
	
	rollout->childs.push_back(widget);
	widget->Show(rollout->opened);
	
	CalcPos();
}

void RolloutWidget::SetSkipChild(BaseWidget* widget, bool set)
{
	for (int i = 0; i < skipped_list.size(); i++)
	{
		if (skipped_list[i] == widget)
		{
			if (!set)
			{
				skipped_list.erase(skipped_list.begin() + i);
			}

			break;
		}
	}

	if (set)
	{
		skipped_list.push_back(widget);
	}
}

void RolloutWidget::OnLeftMouseUp(int mx, int my)
{
	 POINT point;
		
	if (GetCursorPos(&point))
	{	    			
		ScreenToClient(handle, &point);
	}

	for (int i = 0; i < rollouts.size(); i++)
	{
		if (rollouts[i].y < point.y && point.y < rollouts[i].y + 20)
		{
			rollouts[i].opened = !rollouts[i].opened;
			CalcPos();
			Redraw();

			break;
		}
	}
}
void RolloutWidget::CalculateHeigth()
{
    int offset = 0;

    for (int i = 0; i < rollouts.size(); i++)
    {
        Rollout& rollout = rollouts[i];

        rollout.y = offset;
        offset += 25;

        for (int j = 0; j < rollout.childs.size(); j++)		
        {			
            bool is_skipped = false;

            for (int k = 0; k < skipped_list.size(); k++)
            {
                if (skipped_list[k] == rollout.childs[j])
                {
                    is_skipped = true;
                    break;
                }
            }

            if (!is_skipped)
            {
                rollout.childs[j]->Show(rollout.opened);

                if (rollout.opened)
                {
                    rollout.childs[j]->SetPos(rollout.childs[j]->GetX(), offset);
                    offset += rollout.childs[j]->GetHeight() + 5;
                }		
            }
            else
            {
                rollout.childs[j]->Show(false);
            }
        }
    }
    height = offset;
}
void RolloutWidget::CalcPos()
{
    CalculateHeigth();
    SetWindowPos(handle, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

void RolloutWidget::Draw()
{
	HDC hdc = GetDC(handle);


  	HBRUSH hbrBkgnd = CreateSolidBrush(RGB(color.r * 255,color.g * 255,color.b * 255));
	SetBkMode( hdc, TRANSPARENT );
	
	SelectFont(hdc, hFont);

	for (int i = 0; i < rollouts.size(); i++)
	{
		Rollout& rollout = rollouts[i];

		SelectObject(hdc, hbrBkgnd);
		Rectangle ( hdc, 0, rollout.y, width, rollout.y + 20 );

		if (!rollouts[i].opened)
		{
			TextOut(hdc, 7, rollout.y + 3, "+", 1);
		}
		else
		{
			TextOut(hdc, 7, rollout.y + 3, "-", 1);
		}

		TextOut(hdc, 18, rollout.y + 3, rollouts[i].name,strlen(rollouts[i].name));
	}


	ReleaseDC(handle, hdc);
	DeleteObject(hbrBkgnd);	
}

