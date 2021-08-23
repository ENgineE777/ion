
#include "BaseWidget.h"
//#include "../Editor/ResourceViewer.h"
#include "Common/Common.h"

int EventsWidget::id = 0;
int EventsWidget::evnt = 0;
void* EventsWidget::g_pEventData = NULL;
BaseWidget* EventsWidget::g_pEventSender = NULL;
BaseWidget* EventsWidget::modal_dialog = 0;
BaseWidget* EventsWidget::main_window = 0;

void EventsWidget::SetEvent(int set_id, int set_evnt,BaseWidget* pSender,void* pData)
{	
	if (evnt == modal_ok || evnt ==	modal_cancel) return;

	id = set_id;
	evnt = set_evnt;
    g_pEventData = pData;
    g_pEventSender = pSender;
}


bool EventsWidget::IsAnyEvent()
{
	return (id != 0);
}

bool EventsWidget::CheckEvent(int check_id, int check_evnt)
{
	if (id == check_id && evnt == check_evnt)
	{
		return true;
	}
	
	return false;
}

bool EventsWidget::AddEventHandler(int set_id, int set_evnt)
{
	return true;
}

void EventsWidget::SetMainWindow(BaseWidget* main_wnd)
{
	main_window = main_wnd;
}

void EventsWidget::OpenResourceBrowser(const char* res_type)
{
	/*ResourceViewer* res_viewer = new ResourceViewer(150, main_window, 780, 800);
	res_viewer->Init(res_type);
	res_viewer->Show(true);

	modal_dialog = res_viewer;*/
}

bool EventsWidget::IsResourceBrowserOpened()
{
	return modal_dialog;	
}

bool EventsWidget::IsResourceBrowserFinished(bool& succeced)
{	
	if (CheckEvent(150, modal_ok))
	{
		id = 0;
		evnt = 0;

		succeced = true;
		return true;
	}

	if (CheckEvent(150, modal_cancel))
	{
		id = 0;
		evnt = 0;

		succeced = false;
		return true;
	}

	succeced = false;
	return false;	
}

void EventsWidget::CloseModalDialog()
{
	modal_dialog = 0;
}

void EventsWidget::Update()
{	
	if (modal_dialog) modal_dialog->Update();

	// HACK
	if (evnt == modal_ok || evnt ==	modal_cancel) return;

	id = 0;
	evnt = 0;	
}

