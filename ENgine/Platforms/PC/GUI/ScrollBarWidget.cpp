
#include "ScrollBarWidget.h"

ScrollBarWidget::ScrollBarWidget(int set_id, BaseWidget* prnt, bool set_horiz, float set_x, float set_y, float w, float h,EAutoSizeType sizerType) : BaseWidget(set_id, prnt, "horz_scroll", sizerType)
{
	x = set_x;
	y = set_y;
	width = w;
	height = h;

	dword flag = WS_CHILD | WS_VISIBLE;

	horiz = set_horiz;

	if (horiz)
	{
		flag |= SBS_HORZ;		
	}
	else
	{
		flag |= SBS_VERT;
		//flag |= WS_VSCROLL;
	}

	handle = CreateWindow( "SCROLLBAR", text.c_str(), flag,
						   x, y, width, height, parent->GetHandle(), NULL, NULL, NULL );
	SetWindowLong(handle, GWL_ID, id);


	MakeSubClassing();

	cur_pos = 0;
	SetLimit(10, 100);
}

ScrollBarWidget::~ScrollBarWidget()
{
}

void ScrollBarWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{		
	if (horiz && msg != WM_HSCROLL) return;
	if (!horiz && msg != WM_VSCROLL) return;

	int last_pos = cur_pos;

	switch (LOWORD(wParam))
	{
		case SB_TOP:
			cur_pos = 0;
		break;

		case SB_LINEUP:
			if (cur_pos > 0)
				cur_pos--;
		break;

		case SB_PAGEUP:
			cur_pos -= 10;
			if (cur_pos < 0) cur_pos = 0;
		break;

		case SB_THUMBPOSITION:
			cur_pos = HIWORD(wParam);
		break;

		case SB_THUMBTRACK:
			cur_pos = HIWORD(wParam);
		break;

		case SB_LINEDOWN:
			if (cur_pos < max_pos) cur_pos++;
		break;

		case SB_PAGEDOWN:
			cur_pos += 10;
			if (cur_pos > max_pos) cur_pos = max_pos;
			break;

		case SB_BOTTOM:
			cur_pos = max_pos;
		break;

		case SB_ENDSCROLL:
		break;
	}

	if (last_pos != cur_pos)
	{
		EventsWidget::SetEvent(id, EventsWidget::scrollbar_change);
	}

	SetScrollPos(handle, SB_CTL, cur_pos, true);
}

void ScrollBarWidget::SetPosition(int pos)
{
	SetScrollPos(handle, SB_CTL, cur_pos, true);
}

int ScrollBarWidget::GetPosition()
{
	return cur_pos;
}

void ScrollBarWidget::SetLimit(int page_sz, int max_ps)
{
	page_size = page_sz;
	max_pos = max_ps;

	if (cur_pos>max_pos)
	{
		cur_pos = max_pos-1;
	}

	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMin   = 0;
	si.nMax   = max_pos;
	si.nPage  = page_size;
	si.nPos   = cur_pos;
	SetScrollInfo(handle, SB_CTL, &si, true);
}