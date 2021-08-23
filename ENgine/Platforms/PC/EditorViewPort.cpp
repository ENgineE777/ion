
#ifdef EDITOR

#include "EditorViewPort.h"
#include "Core/Core.h"

EditorViewPort::EditorViewPort(int id, BaseWidget* parent, const char* txt, float set_x, float set_y, float w, float h) : BaseWidget(id, parent, txt, BaseWidget::eAutoSizeBoth)
{
	x = set_x;
	y = set_y;
	width = w;
	height = h;
	
	needShowActiveText = true;

	handle = CreateWindow("STATIC","Static",SS_LEFT|WS_CHILD | WS_VISIBLE| SS_NOTIFY, x, y, width,height,parent->GetHandle(),NULL, en_core.GetInstance(), NULL);

	SetWindowLong(handle, GWL_ID, id);
	Static_SetText(handle, text.c_str());

	move_gizmo = false;
	last_mx = last_my = 0;
	rb_pressed = false;
	md_pressed = false;

	gizmo = null;	
	view = null;

	need_reset_render = false;
    bDockResize = false;
    MakeSubClassing();
}

EditorViewPort::~EditorViewPort()
{
}

void EditorViewPort::Reset()
{
	need_reset_render = true;	
}

void EditorViewPort::OnLeftMouseDown(int mx, int my)
{	
	SetFocused();

	core_ed->MakeSelection(mx, my);

	//FIX ME!!!
	//core->Controls()->SetEnbaled(true);

	if (gizmo)
	{
		if (gizmo->selAxis != -1)
		{
			move_gizmo = true;		
			gizmo->transform_started = true;
		}		
	}
}

void EditorViewPort::OnLeftMouseUp(int mx, int my)
{	
	if (move_gizmo)
	{
		//gizmo->transform_finished = true;
	}	

	move_gizmo = false;	
}

void EditorViewPort::OnMouseMove(int mx, int my)
{			
	((ControlsService*)core->Controls())->SetTouchPos(mx,my);		

	float dx = (mx - last_mx) * 0.02f;
	float dy = (my - last_my) * 0.02f;

	if (!en_core.IsEditorMode() &&
		core->Controls()->ControlState(DIK_LCONTROL, IControlsService::Active) && 
		core->Controls()->ControlState(DIK_D, IControlsService::Active))
	{		
		((ControlsService*)core->Controls())->SetGyro(0, (mx - last_mx) * 0.3f);
		((ControlsService*)core->Controls())->SetGyro(1, (my - last_my) * 0.3f);
	}
	else
	{
		((ControlsService*)core->Controls())->SetGyro(0, 0);
		((ControlsService*)core->Controls())->SetGyro(1, 0);
	}

	((ControlsService*)core->Controls())->SetTouchPos(mx,my);

	if (rb_pressed || md_pressed)
	{
		if (view && IsFocused())
		{
			Matrix rot_mat;

			Vector pos = view->Pos();
			view->Pos() = 0.0f;

			if ( md_pressed )
			{
				rot_mat.BuildRotate(view->Vz(), -dx * 0.25f);
				dy = 0;
			}
			else
			{	
				rot_mat.BuildRotateY(dx * 0.25f);
			}

			(*view) = (*view) * rot_mat;		
	
			rot_mat.BuildRotateX(dy * 0.25f);
			(*view) = rot_mat * (*view);

			view->Pos() = pos;
		}		
	}

	if (gizmo)
	{
		if (move_gizmo)
		{		
			Vector dir(dx,dy);
			dir.Normalize();
			dir *= 2.5f;

			//gizmo->MouseMove(mx, my,0, 0);
			gizmo->MouseMove((float)(mx),
							 (float)(my),mx-last_mx, my-last_my);
		}
		else
		{
			gizmo->MouseProcess((float)mx/(float)width, (float)my/(float)height, mx, my);
		}
	}

	last_mx = mx;
	last_my = my;
}

void EditorViewPort::OnRightMouseDown(int mx, int my)
{
	rb_pressed = true;
}

void EditorViewPort::OnRightMouseUp(int mx, int my)
{
	rb_pressed = false;
}

void EditorViewPort::OnMiddleMouseDown(int mx, int my)
{
	md_pressed = true;
}

void EditorViewPort::OnMiddleMouseUp(int mx, int my)
{
	md_pressed = false;
}

void EditorViewPort::SetGizmo(Gizmo* set_gizmo)
{
	gizmo = set_gizmo;
}

void EditorViewPort::SetView(Matrix* v)
{
	view = v;
}

void EditorViewPort::Update()
{
	if (need_reset_render)
	{
		RenderService::VideoMode mode;
		mode.width = width;
		mode.height = height;

		if (width && height)
		{
			en_core.Render()->Display()->ApplyVideoMode(mode);
			need_reset_render = false;
            if(gizmo)
                gizmo->bViewPortResized = true;
            
		}
	}

	if (IsFocused() && gizmo)
	{
		gizmo->KeyboardProcess();
	}

	if (IsFocused() && needShowActiveText)
	{
		en_core.Render()->Debug()->PrintText(10,10,up_left, COLOR_WHITE, "active");
	}
	else
	{
		en_core.Render()->Debug()->PrintText(10,10,up_left, COLOR_WHITE, " ");
	}
}

void EditorViewPort::SetNeedShowActiveText(bool set)
{
	needShowActiveText = set;
}

void EditorViewPort::Resize()
{	
    if(!bDockResize)
	    need_reset_render = true;

	BaseWidget::Resize();
}

void EditorViewPort::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CHAR:core->Controls()->SetLastChar((char)wParam);break;
    case WM_MOUSEMOVE:OnMouseMove( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));break;
    case WM_LBUTTONDOWN:OnLeftMouseDown( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));break;
    case WM_RBUTTONDOWN:OnRightMouseDown( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));break;
    case WM_RBUTTONUP:OnRightMouseUp( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));break;
    case WM_MBUTTONDOWN:OnMiddleMouseDown( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));break;
    case WM_MBUTTONUP:OnMiddleMouseUp( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));break;
    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case DHN_START_RESIZE:bDockResize = true;en_core.SetSkipRender(true); break;
        case DHN_END_RESIZE:
            {
                if(bDockResize)
                {
                    en_core.SetSkipRender(false);
                    bDockResize = false;
                    RenderService::VideoMode mode;
                    mode.width = width;
                    mode.height = height;
                    if (width && height)
                    {
                        en_core.Render()->Display()->ApplyVideoMode(mode);
                        if(gizmo)
                            gizmo->bViewPortResized = true;
                    }
                }
                break;
            }
        }
        break;
    }
}

#endif
