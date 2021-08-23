
#include "Gizmo.h"
#include "Core/Core.h"
#include "Entities/NGUI/NGUIService.h"
#include "Entities/NGUI/NGUIWidget.h"

#ifdef PC

Gizmo::Gizmo()
{
    bViewPortResized = false;
	mode = 0;
    selAxis = -1;            
    enabled = true;
    useLocalSpace = false;
    scale = 1.0f;
    colored = true;
	type = 0;
	transform_started = false;
	transform.Identity();
	abs_units = false;
	rotation = 0.0f;

	anchorn_tex = en_core.Render()->LoadTexture("Editor/anchorn.png", false, _FL_);
	anchorn_tex->SetFilters(Texture::filterPoint, Texture::filterPoint);

	dot_tex = en_core.Render()->LoadTexture("Editor/dot.png", false, _FL_);
	dot_tex->SetFilters(Texture::filterPoint, Texture::filterPoint);

	cr_normal = LoadCursor(NULL, IDC_ARROW);
	cr_move = LoadCursor(NULL, IDC_SIZEALL); 
	cr_resize = LoadCursor(NULL, IDC_SIZENWSE); 
	cr_rotate = LoadCursor(NULL, IDC_CROSS);
}

Color Gizmo::CheckColor(int axis)
{
	 Color color;

      if (axis == selAxis)
      {
		  color = COLOR_YELLOW;

		  if (!colored) color = Color(0.4f, 0.4f, 0.0f);
      }
      else
      {
		  if (axis == 0)
          {
			  color = COLOR_RED;
			  if (!colored) color = Color(0.4f, 0.0f, 0.0f);
          }
          else
          if (axis == 1)                        
          {
			  color = COLOR_GREEN;
			  if (!colored) color = Color(0.0f, 0.4f, 0.0f);
          }
          else
          {
			  color = COLOR_BLUE;
			  if (!colored) color = Color(0.0f, 0.0f, 0.4f);
          }
     }

     return color;
}

void Gizmo::DrawAxis(int axis)
{	
	Vector tr = transform.Pos();
            
	Color color = CheckColor(axis);
    Vector dir;
	dir.Set(0.0f);

    if (axis == 0)
    {
		dir.x = scale;
    }
    else
    if (axis == 1)                    
    {
		dir.y = scale;
    }
    else
    {
		dir.z = scale;
    }            

    if (useLocalSpace)
    {
		dir = transform.MulNormal(dir);
    }

    dir += tr;

    en_core.Render()->Debug()->DrawLine(dir, color, tr, color, !colored);

    float hgt = 0.85f * scale;
    float r = 0.05f * scale;
    int nums = 32;

    for (int i = 0; i < nums; i++)
	{
		float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * r;
        float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * r;

		Vector pos;

        if (axis == 0)
        {
			pos.Set(hgt, dx, dz); 
		}
        else
        if (axis == 1)
        {
			pos.Set(dx, hgt, dz);
        }
        else
        {
			pos.Set(dx, dz, hgt);
        }

		if (useLocalSpace)
        {
			pos = transform.MulNormal(pos);
        }

        en_core.Render()->Debug()->DrawLine(dir, color, pos + tr, color, !colored);
	}
}

void Gizmo::DrawCircle(int axis)
{
	Vector tr = transform.Pos();

    Color color = CheckColor(axis);
    Color color_gray = Color(0.4f, 0.4f, 0.4f);

    if (!colored) color_gray = color;
    
	Matrix trans(true);
	if (useLocalSpace)
	{
		trans = transform;
	}
	else
	{
		trans.Pos() = transform.Pos();
	}

	en_core.Render()->Debug()->DrawCircle(axis,trans,scale,color, color_gray);	
}

bool Gizmo::CheckInersection(Vector pos, Vector pos2,
							 float mx, float my,
							 Vector trans, bool check_trans,
							 Matrix view, Matrix view_proj)
{
	if (useLocalSpace)
	{
		pos = transform.MulNormal(pos);
        pos2 = transform.MulNormal(pos2);
	}

	Vector tr = transform.Pos();

    pos += tr;
    pos2 += tr;            

    Vector pos_post = pos * view;
    Vector pos2_post = pos2 * view;

    bool proceed = true;
    if (check_trans)
    {
		if (pos_post.z > trans.z && pos2_post.z > trans.z)
        {
			proceed = false;
        }
    }

	if (proceed)
	{
		pos_post = en_core.Render()->Display()->TransformToScreen(pos, 1);
		pos2_post = en_core.Render()->Display()->TransformToScreen(pos2, 1);
		
        float x1 = min(pos_post.x , pos2_post.x );
        float x2 = max(pos_post.x , pos2_post.x );

        float y1 = max(pos_post.y , pos2_post.y );
        float y2 = min(pos_post.y , pos2_post.y );

		float sz = 0.025f;
        x1 += -sz;
        x2 +=  sz;
        y1 += -sz;
        y2 +=  sz;

        if (x1 < mx && mx < x2 &&
            y1 < my && my < y2)
        {
			ms_dir.Set(pos2_post.x - pos_post.x, pos2_post.y - pos_post.y);
            ms_dir.Normalize();

			return true;
        }
	}

	return false;
}

bool Gizmo::MouseProcess(int axis, float mx, float my)
{            		
	Matrix view;
	en_core.Render()->Display()->GetView( view );

    Matrix view_proj;
	en_core.Render()->Display()->GetProj( view_proj );
	view_proj = view * view_proj;

    if (mode == 0)
    {
		Vector dir;
		dir.Set(0.0f);

        if (axis == 0)
        {
			dir.x = scale;
        }
        else
        if (axis == 1)
        {
			dir.y = scale;
        }                
        else
        {
			dir.z = scale;
        }                

		for (int i=0; i<7; i++)
        {                                        
			Vector pos; pos = dir * (float)i * (1.0f / 7.0f);
            Vector pos2; pos2 = dir * (float)(i+1) * (1.0f / 7.0f);
			Vector tr; tr.Set(0.0f);

            if (CheckInersection(pos, pos2,
                                 mx, my, tr, false,
                                 view, view_proj))
			{
				return true;
            }
		}                                
	}
    else
	if (mode == 1)
    {
		Vector trans = transform.Pos() * view;

        float r = scale;
                
		float last_dx = -r * 2;
		float last_dz = -r * 2;        
        int nums = 32;

        for (int i = 0; i < nums + 1; i++)
        {
			float dx = (float)sinf(2.0f * 3.14f / (float)nums * (float)i) * r;
            float dz = (float)cosf(2.0f * 3.14f / (float)nums * (float)i) * r;

            if (last_dx > -r * 1.5f)
            {
				Vector pos;
                Vector pos2;

                if (axis == 0)
				{
					pos.Set(0.0f, last_dx, last_dz);
                    pos2.Set(0.0f, dx, dz);
                }
                else
                if (axis == 1)
                {
					pos.Set(last_dx, 0.0f, last_dz);
                    pos2.Set(dx, 0.0f, dz);
                }
                else
                {
					pos.Set(last_dx, last_dz, 0.0f);
                    pos2.Set(dx, dz, 0.0f);
                }
                        
                if (CheckInersection(pos, pos2,
                                     mx, my, trans, true,
                                     view, view_proj))
                {					
					return true;
				}
			}

			last_dx = dx;
			last_dz = dz;
		}
	}

	return false;
}

void Gizmo::MouseProcess(float mx, float my, float scr_mx, float scr_my)
{
	if (!enabled) return;

	selAxis = -1;

	if (type == 1)
	{	
		float width = transform._11;
		float height = transform._22;
		float x = transform._41;
		float y = transform._42;	

		if (abs_units)
		{			
			width = width * NGUIService::instance->h_mul_RefToScreen;
			x = x * NGUIService::instance->h_mul_RefToScreen;

			height = height * NGUIService::instance->v_mul_RefToScreen;
			y = y * NGUIService::instance->v_mul_RefToScreen;
		}

		x = x * transform._12 + transform._43;
		y = y * transform._13 + transform._44;

		if (x+width-10<scr_mx && scr_mx<x+width &&
			y+height-10<scr_my && scr_my<y+height)
		{
			selAxis = 1;
		}
		else
		if (x<scr_mx && scr_mx<x+width &&
			y<scr_my && scr_my<y+height)
		{
			selAxis = 0;
		}		

		return;
	}	
	else
	if (type == 2)
	{
		prev_ms = Vector2(scr_mx, scr_my);

		for (int i=0; i<8; i++)
		{
			if (ancorns[i].x - 10 < scr_mx && scr_mx < ancorns[i].x + 10 &&
				ancorns[i].y - 10 < scr_my && scr_my < ancorns[i].y + 10)
			{
				SetCursor(cr_resize);
				selAxis = i + 1;
				break;
			}
		}

		if (selAxis == -1)
		{
			if (IsPointInsideTriangle(Vector2(scr_mx, scr_my), ancorns[0], ancorns[1], ancorns[2]) ||
				IsPointInsideTriangle(Vector2(scr_mx, scr_my), ancorns[0], ancorns[2], ancorns[3]))		
			{		
				SetCursor(cr_move);	
				selAxis = 0;
			}
			else
			{				
				SetCursor(cr_rotate);	
				selAxis = 9;				
			}
		}

		return;
	}

    if (MouseProcess(0, mx, my)) selAxis = 0;
    if (MouseProcess(1, mx, my)) selAxis = 1;
    if (MouseProcess(2, mx, my)) selAxis = 2;
}

bool Gizmo::MouseMove(float mx, float my, float scr_mx, float scr_my)
{			
	if (selAxis == -1 || !enabled) return false;
             
	if (type == 1)
	{
		if (abs_units)
		{			
			scr_mx = scr_mx * NGUIService::instance->h_mul_ScreenToRef;
			scr_my = scr_my * NGUIService::instance->v_mul_ScreenToRef;			
		}

		scr_mx *= transform._12;
		scr_my *= transform._13;

		if (selAxis == 0)
		{
			transform._41 += scr_mx;
			transform._42 += scr_my;
		}
		else
		if (selAxis == 1)
		{
			float y_scale = transform._22 / transform._11;

			transform._11 += scr_mx;

			if (core->Controls()->ControlState(DIK_LSHIFT, IControlsService::Active))
			{
				transform._22 += scr_mx * y_scale;
			}
			else
			{
				transform._22 += scr_my;
			}

			if (transform._11 < 10)
			{
				transform._11 = 10;

				if (core->Controls()->ControlState(DIK_LSHIFT, IControlsService::Active))
				{
					if (y_scale > 1.0f)
					{
						transform._22 = 10 * y_scale;
					}
				}
			}

			if (transform._22 < 10)
			{
				transform._22 = 10;

				if (core->Controls()->ControlState(DIK_LSHIFT, IControlsService::Active))
				{
					if (y_scale < 1.0f)
					{
						transform._11 = 10 / y_scale;
					}
				}
			}
		}

		return true;
	}
	else
	if (type == 2)
	{
		Vector2 ms = Vector2(mx, my);

		if (selAxis == 0)
		{
			transform._41 += scr_mx;
			transform._42 += scr_my;
		}
		else
		if (selAxis == 9)
		{												
			Vector2 p1 = ms - origin;
			p1.Normalize();

			Vector2 p2 = prev_ms - origin;
			p2.Normalize();					
			

			float k = p1.x * p2.x + p1.y * p2.y;

			if (fabs(k) < 1.0f)
			{				
				k = acosf(k);

				Vector2 p3(-p2.y,p2.x);
			
				float k2 = p1.x * p3.x + p1.y * p3.y;

				if (k2 < 0)
				{
					k *= -1.0f;
				}			
				
				rot_matrix.RotateZ(k);
			}			
		}
		else		
		if (selAxis > 0)
		{			
			Vector2 p1 = ms - prev_ms;

			float dist = p1.Length();

			if (dist > 0.01f)
			{
				p1.Normalize();			

				float k1 = 1.0f;
				float k2 = 1.0f;

				if (selAxis == 5 || selAxis == 7)
				{
					k1 = 0.0f;
				}

				if (selAxis == 6 || selAxis == 8)
				{
					k2 = 0.0f;
				}

				float dot1 = 0.0f;
				float dot2 = 0.0f;

				dot1 = p1.x * transform.Vx().x + p1.y * transform.Vx().y;
				dot2 = p1.x * transform.Vy().x + p1.y * transform.Vy().y;				

				width += dist * k1 * dot1;
				height += dist * k2 * dot2;			
			}
		}
		
		prev_ms = ms;

		return true;
	}

    Vector cur_dir; cur_dir.Set(mx,my);
    float da = cur_dir.Length();
    cur_dir.Normalize();            
            
	da *= cur_dir | ms_dir;

    if (mode == 0)
    {
		da *= 0.045f * scale;

        if (selAxis == 0)
        {
			transform._41 += da;
        }
        else
        if (selAxis == 1)
        {
			transform._42 += da;
        }
        else
		{
			transform._43 += da;
        }
	}
    else
    if (mode == 1)
	{
		da *= -0.01f;

        Matrix rot;

        if (selAxis == 0)
		{
			rot.RotateX(da);
        }
        else
		if (selAxis == 1)
		{
			rot.RotateY(-da);
        }
        else
        {
			rot.RotateZ(da);
        }

        if (useLocalSpace)
		{
			transform = rot * transform;
        }
        else
        {
			Vector tr = transform.Pos();		
			transform = transform * rot;
			transform.Pos() = tr;
        }
	}

	return true;
}

bool Gizmo::KeyboardProcess()
{
	if (type == 1 || type == 2)
	{
		if ( core->Controls()->ControlState(DIK_LEFT, IControlsService::Activated) )
		{
			transform.Pos().x -= 1.0f;
			return true;
		}

		if ( core->Controls()->ControlState(DIK_RIGHT, IControlsService::Activated) )
		{
			transform.Pos().x += 1.0f;
			return true;
		}

		if ( core->Controls()->ControlState(DIK_UP, IControlsService::Activated) )
		{
			transform.Pos().y -= 1.0f;
			return true;
		}

		if ( core->Controls()->ControlState(DIK_DOWN, IControlsService::Activated) )
		{
			transform.Pos().y += 1.0f;
			return true;
		}
	}

	return false;
}

void Gizmo::Render()
{
	if (!enabled) return;

	if (type == 1)
	{

		float width = transform._11;
		float height = transform._22;
		float x = transform._41;
		float y = transform._42;	
		
		if (abs_units)
		{
			width = width * NGUIService::instance->h_mul_RefToScreen;
			x = x * NGUIService::instance->h_mul_RefToScreen;

			height = height * NGUIService::instance->v_mul_RefToScreen;
			y = y * NGUIService::instance->v_mul_RefToScreen;
		}		

		x = x * transform._12 + transform._43;
		y = y * transform._13 + transform._44;

		en_core.Render()->Debug()->DrawLine2D(Vector2(x,y), COLOR_GREEN,Vector2(x+width,y), COLOR_GREEN);
		en_core.Render()->Debug()->DrawLine2D(Vector2(x+width,y), COLOR_GREEN,Vector2(x+width,y+height), COLOR_GREEN);
		en_core.Render()->Debug()->DrawLine2D(Vector2(x+width,y+height), COLOR_GREEN,Vector2(x,y+height), COLOR_GREEN);
		en_core.Render()->Debug()->DrawLine2D(Vector2(x,y+height), COLOR_GREEN,Vector2(x,y), COLOR_GREEN);

		en_core.Render()->Debug()->DrawLine2D(Vector2(x+width-10,y+height-10), COLOR_GREEN,Vector2(x+width,y+height-10), COLOR_GREEN);
		en_core.Render()->Debug()->DrawLine2D(Vector2(x+width,y+height-10), COLOR_GREEN,Vector2(x+width,y+height), COLOR_GREEN);
		en_core.Render()->Debug()->DrawLine2D(Vector2(x+width,y+height), COLOR_GREEN,Vector2(x+width-10,y+height), COLOR_GREEN);
		en_core.Render()->Debug()->DrawLine2D(Vector2(x+width-10,y+height), COLOR_GREEN,Vector2(x+width-10,y+height-10), COLOR_GREEN);

		return;
	}

	if (type == 2)
	{
		Vector p1, p2;		

		for (int phase=1; phase<=2;phase++)
		{
			for (int i=0; i < 4 * phase; i++)
			{
				if (i==0)
				{
					p1 = Vector(0, 0, 0.0f);
					p2 = Vector(width, 0, 0.0f);
				}
				else
				if (i==1)
				{
					p1 = Vector(width, 0, 0.0f);
					p2 = Vector(width, height, 0.0f);
				}
				else
				if (i==2)
				{
					p1 = Vector(width, height, 0.0f);
					p2 = Vector(0, height, 0.0f);
				}
				else
				if (i==3)
				{
					p1 = Vector(0, height, 0.0f);
					p2 = Vector(0, 0, 0.0f);
				}				
				else
				if (i==4)
				{
					p1 = Vector(width * 0.5f, 0, 0.0f);					
				}
				else
				if (i==5)
				{
					p1 = Vector(width, height * 0.5f, 0.0f);
				}
				else
				if (i==6)
				{
					p1 = Vector(width * 0.5f, height, 0.0f);
				}
				else
				if (i==7)
				{
					p1 = Vector(0, height * 0.5f, 0.0f);
				}		

				p1 = p1 * transform;

				if (phase == 1)
				{					
					p2 = p2 * transform;				

					en_core.Render()->Debug()->DrawLine2D(Vector2(p1.x + NGUIWidget::editor_offset_x,p1.y + NGUIWidget::editor_offset_y), COLOR_WHITE,
														  Vector2(p2.x + NGUIWidget::editor_offset_x, p2.y + NGUIWidget::editor_offset_y), COLOR_WHITE);
				}
				else
				{
					p1.x += NGUIWidget::editor_offset_x;
					p1.y += NGUIWidget::editor_offset_y;

					ancorns[i] = Vector2(p1.x,p1.y);

					en_core.Render()->Debug()->DrawSprite(anchorn_tex, Vector2(p1.x - 4, p1.y - 4), 8.0f, up_left, COLOR_WHITE, 0.0f);
				}				
			}
			
			p1 = Vector(0.0f, 0.0f, 0.0f);

			p1 = p1 * center_transform;

			origin = Vector2(p1.x, p1.y);			

			en_core.Render()->Debug()->DrawSprite(dot_tex, Vector2(origin.x - 4 + NGUIWidget::editor_offset_x, origin.y - 4 + NGUIWidget::editor_offset_y), 8.0f, up_left, COLOR_WHITE, 0.0f);			
		}
		
		return;
	}

	Matrix view;
	en_core.Render()->Display()->GetView( view );

	Matrix view_proj;
	en_core.Render()->Display()->GetProj( view_proj );
	view_proj = view * view_proj;
	
	Vector pos = transform.Pos();
	float z=pos.x*view_proj._13+pos.y*view_proj._23+pos.z*view_proj._33+view_proj._43;    

	scale = 0.1f * (1.0f + z);
	scale = fabs(scale);

	if (mode == 0)
    {	
		colored = false;
		DrawAxis(0);
        DrawAxis(1);
        DrawAxis(2);

		colored = true;
		DrawAxis(0);
		DrawAxis(1);
		DrawAxis(2);
    }
    else
    {
		colored = false;
		DrawCircle(0);
		DrawCircle(1);
		DrawCircle(2);

		colored = true;
		DrawCircle(0);
        DrawCircle(1);
        DrawCircle(2);
    } 
}

#endif