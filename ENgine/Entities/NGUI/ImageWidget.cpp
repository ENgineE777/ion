
#include "ImageWidget.h"

struct ImageHolder
{
	ImageWidget* widget;
	WNDPROC prevProc;
};

std::vector<ImageHolder> img_holders;

LRESULT CALLBACK ImageProc( HWND hwnd, dword msg, WPARAM wParam, LPARAM lParam )
{
	for (int i=0;i<img_holders.size();i++)
	{
		if (img_holders[i].widget->GetHandle() == hwnd)
		{		
			if (msg == WM_VSCROLL || msg == WM_HSCROLL)
			{
				for (int j=0;j<img_holders[i].widget->GetChildsCount();j++)
				{
					img_holders[i].widget->GetChild(j)->ProcessWidget(msg, wParam, lParam);
				}
			}		

			if (msg == WM_ERASEBKGND)
			{
				return (LRESULT)1; // Say we handled it.
			}

			if (msg == WM_LBUTTONDOWN)
			{
				img_holders[i].widget->OnMouseDown();
			}

			if (msg == WM_RBUTTONDOWN)
			{
				img_holders[i].widget->OnRightMouseDown();
			}

			if (msg == WM_MOUSEMOVE)
			{
				img_holders[i].widget->OnMouseMove();
			}

			if (msg == WM_LBUTTONUP)
			{
				img_holders[i].widget->OnMouseUp();
			}

			if (msg == WM_RBUTTONUP)
			{
				img_holders[i].widget->OnRightMouseUp();
			}

			if (msg == WM_PAINT)
			{
				img_holders[i].widget->Draw();					
			}

			if (msg == WM_KEYDOWN)
			{
				img_holders[i].widget->Draw();					
			}			

			//if (msg == WM_DRAWITEM) core->Trace("labelproc - %s %i", lb_holders[i].widget->GetText(), i);
			//lb_holders[i].widget->ProcessWidget( msg, wParam, lParam );
			return CallWindowProc(img_holders[i].prevProc, hwnd, msg, wParam, lParam);	
		}
	}
	
	return true;	
}

ImageWidget::ImageWidget(int set_id, BaseWidget* prnt, const char* txt, float set_x, float set_y, float w, float h,EAutoSizeType sizerType) : BaseWidget(set_id, prnt, txt, sizerType)
{	
	x = set_x;
	y = set_y;
	width = w;
	height = h;
	
	handle = CreateWindow("STATIC","Static", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | SS_NOTIFY,x,y,width,height,parent->GetHandle(),NULL, NULL, NULL);
			
	SetWindowLong(handle, GWL_ID, id);

	Static_SetText(handle, text.c_str());

	color = Color(0.8f, 0.8f, 0.8f);

	img_holders.push_back(ImageHolder());
	ImageHolder* holder = &img_holders[img_holders.size()-1];

	holder->prevProc = (WNDPROC)SetWindowLong(handle, GWL_WNDPROC, (LONG)ImageProc);
	holder->widget = this;	

	img_width = 0;
	img_height = 0;
	
	image = null;

	drag_type = 0;

	scale = 1.0f;

	origin.x = 40;
	origin.y = height - 40;

	sel_rect = -1;
	cur_type = 0;
	sel_corner = -1;

	scale_type = 0;
	vert_align = 0;
	horz_align = 0;
	slice_in_ref = false;

	pixel_density = 1.0f;

	texMode = 0;
	texFilter = 1;
	texName[0] = 0;

	delta_mouse = 0.0f;

	tex_mode = 0;
}

ImageWidget::~ImageWidget()
{
	for (int i=0;i<img_holders.size();i++)
	{
		if (img_holders[i].widget == this)
		{
			img_holders.erase(img_holders.begin() + i);
			break;
		}
	}
}

void ImageWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
	if (WM_COMMAND != msg) return;
	if (id != LOWORD(wParam)) return;
}

void ImageWidget::SetImage(const char* img)
{
	if (image) DeleteObject(image);
	
	String::Copy(texName, 512, img);
	String::Copy(texture_data.fl_name, 512, img);
		
	img_width = 1;
	img_height = 1;	


	char ext[16];
	String::ExtractExctention(img, ext, 15);
		
	if (String::IsEqual(ext, "atx"))		
	{
		IFileBuffer* fl = core->Files()->LoadFile(img);

		if (fl)
		{
			char path[256];
			String::ExtractPath(img, path, true);

			int len = fl->GetSize();
			byte* ptr = fl->GetBuffer(); 

			int index = 0;
				
			bool first_line = true;

			while (index<len)
			{
				int sub_index = 0;
				char frame[128];

				while (ptr[index] != ' ')
				{
					frame[sub_index] = ptr[index];
					sub_index++;
					index++;
				}

				frame[sub_index] = 0;
				index++;

				sub_index = 0;
				char time[128];
				
				while (ptr[index] != '|')
				{
					time[sub_index] = ptr[index];
					sub_index++;
					index++;
				}

				time[sub_index] = 0;
				index++;
				
				float tm  = atof(time);

				char file_name[512];
				String::Copy(file_name, 511, path);
				String::Cat(file_name, 511, frame);

				if (first_line)
				{
					if (String::IsEqual(frame, "NeedStream"))
					{							
					}
					else
					if (String::IsEqual(frame, "Looped"))
					{							
					}
					else
					{	
						String::Copy(texture_data.fl_name, 512, file_name);						
						break;						
					}
				}
				else
				{
					String::Copy(texture_data.fl_name, 512, file_name);	
					break;	
				}			

				index+=2;
			}

			fl->Release();
		}
	}



	if (!texture_data.GetInfo())
	{	
		return;
	}
		 
	img_width = texture_data.width;
	img_height = texture_data.height;	

	texture_data.Load();
	
    BITMAPINFO bminfo;
    ZeroMemory(&bminfo, sizeof(bminfo));
    bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bminfo.bmiHeader.biWidth = img_width;
    bminfo.bmiHeader.biHeight = ((LONG)img_height);
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;
 
    // create a DIB section that can hold the image
    byte* pvImageBits = NULL;
    HDC hdcScreen = GetDC(NULL);
    image = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, (void**)&pvImageBits, NULL, 0);
    ReleaseDC(NULL, hdcScreen);
     	
	Color back_colors[2];
	back_colors[0] = COLOR_WHITE;
	back_colors[1] = COLOR_LIGHT_GRAY;

	for(int j=0; j < img_height;j++)
	{
		byte* rows = &pvImageBits[(img_height - j - 1) * img_width * 4];				

		for(int i=0; i < img_width;i++)
		{
			int sel_back_colors = (int)(i / 4.0f) % 2 + (int)(j / 4.0f) % 2;

			if (sel_back_colors == 2)
			{
				sel_back_colors = 0;
			}

			byte* color = &texture_data.data[(i + j * img_width) * texture_data.bytesPerPixel];
			
			if (texture_data.bytesPerPixel == 4)
			{
				float k = color[3] / 255.0f;			

				rows[i*4 + 0] = color[2] * k + back_colors[sel_back_colors].b * 255 * (1 - k);		
				rows[i*4 + 1] = color[1] * k + back_colors[sel_back_colors].g * 255 * (1 - k);				
				rows[i*4 + 2] = color[0] * k + back_colors[sel_back_colors].r * 255 * (1 - k);		
				rows[i*4 + 3] = 255;
			}
			else
			{
				rows[i*4 + 0] = color[2];		
				rows[i*4 + 1] = color[1];
				rows[i*4 + 2] = color[0];
				rows[i*4 + 3] = 255;
			}			
		}		
	}	

	free(texture_data.data);

	rects[0].x = 0;
	rects[0].y = img_height;
	rects[0].width = img_width;
	rects[0].height = img_height;

	rects[1].x = 0;
	rects[1].y = img_height;
	rects[1].width = 10;
	rects[1].height = img_height;

	rects[2].x = 10;
	rects[2].y = img_height;
	rects[2].width = img_width - 20;
	rects[2].height = img_height;

	rects[3].x = img_width - 10;
	rects[3].y = img_height;
	rects[3].width = 10;
	rects[3].height = img_height;

	rects[4].x = 0;
	rects[4].y = img_height;
	rects[4].width = img_width;
	rects[4].height = 10;

	rects[5].x = 0;
	rects[5].y = img_height - 10;
	rects[5].width = img_width;
	rects[5].height = img_height - 20;

	rects[6].x = 0;
	rects[6].y = 10;
	rects[6].width = img_width;
	rects[6].height = 10;

	rects[7].x = 0;
	rects[7].y = img_height;
	rects[7].width = 10;
	rects[7].height = 10;

	rects[8].x = 10;
	rects[8].y = img_height;
	rects[8].width = img_width - 20;
	rects[8].height = 10;

	rects[9].x = img_width - 10;
	rects[9].y = img_height;
	rects[9].width = 10;
	rects[9].height = 10;

	rects[10].x = 0;
	rects[10].y = img_height-10;
	rects[10].width = 10;
	rects[10].height = img_height - 20;

	rects[11].x = 10;
	rects[11].y = img_height-10;
	rects[11].width = img_width - 20;
	rects[11].height = img_height - 20;

	rects[12].x = img_width - 10;
	rects[12].y = img_height-10;
	rects[12].width = 10;
	rects[12].height = img_height - 20;

	rects[13].x = 0;
	rects[13].y = 10;
	rects[13].width = 10;
	rects[13].height = 10;

	rects[14].x = 10;
	rects[14].y = 10;
	rects[14].width = img_width - 20;
	rects[14].height = 10;

	rects[15].x = img_width - 10;
	rects[15].y = 10;
	rects[15].width = 10;
	rects[15].height = 10;

	FitImage();
}

void ImageWidget::SelectStartEndRect(int& start_rect, int& end_rect)
{
	start_rect = 0;
	end_rect = 1;

	if (cur_type == 1)
	{		
		start_rect = 1;
		end_rect = 4;
	}
	else
	if (cur_type == 2)
	{		
		start_rect = 4;
		end_rect = 7;
	}
	else	
	if (cur_type == 3)
	{		
		start_rect = 7;
		end_rect = 16;
	}	
}

void ImageWidget::Draw()
{	
	PAINTSTRUCT ps;
	BeginPaint(handle, &ps);

	RECT rc;
    HDC hdcMem;
    HBITMAP hbmMem, hbmOld;
    HBRUSH hbrBkGnd;
    HFONT hfntOld;
    
    GetClientRect(handle, &rc);

	hdcMem = CreateCompatibleDC(ps.hdc);
    
    hbmMem = CreateCompatibleBitmap(ps.hdc, rc.right-rc.left, rc.bottom-rc.top);

    hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);


	HDC hdc = hdcMem;
			
	HBRUSH hbrBkgnd = CreateSolidBrush(RGB(color.r * 255,color.g * 255,color.b * 255));
	HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));
	HPEN pen_white = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	HPEN pen_green = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	HPEN pen_axis = CreatePen(PS_SOLID, 2, RGB(55, 55, 55));

	HPEN pre_pen;


		
	SelectObject(hdc, hbrBkgnd);
	Rectangle ( hdc, 0, 0, width, height );	
							
	MoveToEx(hdc, origin.x * pixel_density, (origin.y - 700) * pixel_density, 0);
	LineTo(hdc, origin.x * pixel_density, (origin.y + 700) * pixel_density);

	MoveToEx(hdc, (origin.x - 700) * pixel_density, origin.y * pixel_density, 0);
	LineTo(hdc, (origin.x + 700) * pixel_density, origin.y * pixel_density);	

	pre_pen = (HPEN)SelectObject(hdc, hbrBkgnd);


	SelectObject(hdc, pre_pen);

	HDC MemDCExercising = CreateCompatibleDC(hdc);	
	SelectObject(MemDCExercising, image);
	
	float cur_wdth = img_width * pixel_density;
	float cur_hgt = img_height * pixel_density;

	Vector2 pos = origin;
	pos.y -= img_height;
	
	StretchBlt(hdc, pos.x * pixel_density, pos.y * pixel_density, cur_wdth, cur_hgt, MemDCExercising, 0, 0, img_width, img_height, SRCCOPY);	

	int start_rect = 0;
	int end_rect = 1;	
	
	SelectStartEndRect(start_rect, end_rect);

	for (int i = start_rect; i<end_rect; i++)
	{
		Rect& rect  = rects[i];

		if (sel_rect == i)
		{
			pre_pen = (HPEN)SelectObject(hdc, pen_green);			
		}
		else
		{
			pre_pen = (HPEN)SelectObject(hdc, pen_white);			
		}		

		MoveToEx(hdc, (origin.x + rect.x) * pixel_density, (origin.y - rect.y) * pixel_density, 0);
		LineTo(hdc, (origin.x + rect.x) * pixel_density, (origin.y - rect.y + rect.height) * pixel_density);
		LineTo(hdc, (origin.x + rect.x + rect.width) * pixel_density, (origin.y - rect.y + rect.height) * pixel_density);
		LineTo(hdc, (origin.x + rect.x + rect.width) * pixel_density, (origin.y - rect.y) * pixel_density);
		LineTo(hdc, (origin.x + rect.x) * pixel_density, (origin.y - rect.y) * pixel_density);

		SelectObject(hdc, white);

		SelectObject(hdc, pre_pen);

		Rectangle ( hdc, (origin.x + rect.x) * pixel_density - 4, (origin.y - rect.y) * pixel_density - 4,
						 (origin.x + rect.x) * pixel_density + 4, (origin.y - rect.y) * pixel_density + 4);
		Rectangle ( hdc, (origin.x + rect.x) * pixel_density - 4, (origin.y - rect.y + rect.height) * pixel_density - 4,
						 (origin.x + rect.x) * pixel_density + 4, (origin.y - rect.y + rect.height) * pixel_density + 4);
		Rectangle ( hdc, (origin.x + rect.width + rect.x) * pixel_density - 4, (origin.y - rect.y + rect.height) * pixel_density - 4,
						 (origin.x + rect.width + rect.x) * pixel_density + 4, (origin.y - rect.y + rect.height) * pixel_density + 4);
		Rectangle ( hdc, (origin.x + rect.width + rect.x) * pixel_density - 4, (origin.y - rect.y) * pixel_density - 4,
						 (origin.x + rect.width + rect.x) * pixel_density + 4, (origin.y - rect.y) * pixel_density + 4);	
	}

	ReleaseDC(handle, hdc);
	DeleteObject(hbrBkgnd);
	DeleteObject(white);	
	DeleteObject(pen_white);
	DeleteObject(pen_green);
	DeleteObject(pen_axis);	

	DeleteDC(MemDCExercising);
	
    BitBlt(ps.hdc,
           rc.left, rc.top,
           rc.right-rc.left, rc.bottom-rc.top,
           hdcMem,
           0, 0,
           SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);

	EndPaint(handle, &ps);
}

void ImageWidget::OnMouseDown()
{		
	if (GetCursorPos(&ms_point))
	{	    			
		ScreenToClient(handle, &ms_point);
	}

	Vector2 ps(ms_point.x - origin.x * pixel_density, origin.y * pixel_density - ms_point.y);
	
	sel_rect = -1;
	sel_corner = -1;

	int start_rect = 0;
	int end_rect = 1;	
	
	SelectStartEndRect(start_rect, end_rect);

	for (int i = start_rect; i<end_rect; i++)	
	{
		Rect& rect  = rects[i];
		
		Vector2 corner_pt;

		for (int j = 0; j<4; j++)
		{
			if (j == 0)
			{
				corner_pt.x = rect.x;
				corner_pt.y = rect.y;
			}
			else
			if (j == 1)
			{
				corner_pt.x = rect.x + rect.width;
				corner_pt.y = rect.y;
			}
			else
			if (j == 2)
			{
				corner_pt.x = rect.x + rect.width;
				corner_pt.y = rect.y - rect.height;
			}
			else
			if (j == 3)
			{
				corner_pt.x = rect.x;
				corner_pt.y = rect.y - rect.height;
			}
			
			if (corner_pt.x * pixel_density - 7 < ps.x && ps.x < corner_pt.x * pixel_density + 7 &&
				corner_pt.y * pixel_density - 7 < ps.y && ps.y < corner_pt.y * pixel_density + 7)
			{
				sel_corner = j;
				sel_rect = i;

				break;
			}
		}		

		if (sel_rect != -1)
		{
			break;
		}

		int left_border = 0;
		int right_border = rect.width;

		if (rect.width < 0)
		{
			left_border = rect.width;
			right_border = 0;
		}

		int top_border = -rect.height;
		int bottom_border = 0;

		if (rect.height < 0)
		{
			top_border = 0;
			bottom_border = -rect.height;			
		}

		if ((rect.x + left_border) * pixel_density < ps.x && ps.x < (rect.x + right_border) * pixel_density &&
			(rect.y + top_border) * pixel_density < ps.y && ps.y < (rect.y + bottom_border) * pixel_density)
		{		
			sel_rect = i;
			break;
		}
	}	

	if (sel_rect != -1)
	{
		drag_type = 2;
		GetCursorPos(&ms_point);	
		SetCapture(handle);		
	}

	Redraw();
}

void ImageWidget::OnMouseMove()
{
	POINT point;
	
	GetCursorPos(&point);
	delta_mouse.x += (ms_point.x - point.x) / pixel_density;
	delta_mouse.y += (ms_point.y - point.y) / pixel_density;

	Vector2 delta(0.0f);

	if (fabs(delta_mouse.x) > 1.0f)
	{
		delta.x = (int)delta_mouse.x;
		delta_mouse.x = 0.0f;
	}

	if (fabs(delta_mouse.y) > 1.0f)
	{
		delta.y = (int)delta_mouse.y;
		delta_mouse.y = 0.0f;
	}
	
	MoveRect(delta, drag_type);

	ms_point = point;
}

void ImageWidget::MoveRect(Vector2 delta, int drag_type)
{
	if (drag_type == 1)
	{		
		origin -= delta;		
		Redraw();
	}
	else
	if (drag_type == 2)
	{		
		if (cur_type == 1)
		{
			if (sel_corner == -1)
			{
				for (int i = 1; i<4; i++)
				{
					rects[i].x -= delta.x;
					rects[i].y += delta.y;
				}						
			}
			else
			{
				if (sel_rect == 1)
				{
					if (sel_corner == 0)
					{
						rects[1].x -= delta.x;
						rects[1].width += delta.x;

						for (int i = 1; i<4; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
					else
					if (sel_corner == 1)
					{									
						rects[1].width -= delta.x;
						
						rects[2].x -= delta.x;
						rects[2].width += delta.x;

						for (int i = 1; i<4; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
					else
					if (sel_corner == 2)
					{
						rects[1].width -= delta.x;						

						rects[2].x -= delta.x;
						rects[2].width += delta.x;

						for (int i = 1; i<4; i++)
						{
							rects[i].height -= delta.y;
						}
					}
					else
					if (sel_corner == 3)
					{			
						rects[1].x -= delta.x;
						rects[1].width += delta.x;

						for (int i = 1; i<4; i++)
						{
							rects[i].height -= delta.y;
						}
					}
				}
				else
				if (sel_rect == 2)
				{
					if (sel_corner == 1)
					{									
						rects[2].width -= delta.x;
						
						rects[3].x -= delta.x;
						rects[3].width += delta.x;

						for (int i = 1; i<4; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
					else
					if (sel_corner == 2)
					{
						rects[2].width -= delta.x;						

						rects[3].x -= delta.x;
						rects[3].width += delta.x;

						for (int i = 1; i<4; i++)
						{
							rects[i].height -= delta.y;
						}
					}
				}
				if (sel_rect == 3)
				{
					if (sel_corner == 1)
					{									
						rects[3].width -= delta.x;											

						for (int i = 1; i<4; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
					else
					if (sel_corner == 2)
					{
						rects[3].width -= delta.x;												

						for (int i = 1; i<4; i++)
						{
							rects[i].height -= delta.y;
						}
					}
				}
			}
		}
		else
		if (cur_type == 2)
		{
			if (sel_corner == -1)
			{
				for (int i = 4; i<7; i++)
				{
					rects[i].x -= delta.x;
					rects[i].y += delta.y;
				}						
			}
			else
			{
				if (sel_rect == 4)
				{
					if (sel_corner == 0)
					{
						rects[4].y += delta.y;
						rects[4].height += delta.y;
						
						for (int i = 4; i<7; i++)
						{
							rects[i].x -= delta.x;
							rects[i].width += delta.x;							
						}
					}
					else
					if (sel_corner == 1)
					{																					
						rects[4].y += delta.y;
						rects[4].height += delta.y;						

						for (int i = 4; i<7; i++)
						{
							rects[i].width -= delta.x;							
						}
					}
					else
					if (sel_corner == 2)
					{
						rects[4].height -= delta.y;
						
						rects[5].y += delta.y;
						rects[5].height += delta.y;

						for (int i = 4; i<7; i++)
						{
							rects[i].width -= delta.x;							
						}
					}
					else
					if (sel_corner == 3)
					{			
						rects[4].height -= delta.y;						

						rects[5].y += delta.y;
						rects[5].height += delta.y;

						for (int i = 4; i<7; i++)
						{
							rects[i].x -= delta.x;
							rects[i].width += delta.x;							
						}
					}
				}
				else
				if (sel_rect == 5)
				{
					if (sel_corner == 2)
					{
						rects[5].height -= delta.y;
						
						rects[6].y += delta.y;
						rects[6].height += delta.y;

						for (int i = 4; i<7; i++)
						{
							rects[i].width -= delta.x;							
						}
					}
					else
					if (sel_corner == 3)
					{			
						rects[5].height -= delta.y;						

						rects[6].y += delta.y;
						rects[6].height += delta.y;

						for (int i = 4; i<7; i++)
						{
							rects[i].x -= delta.x;
							rects[i].width += delta.x;							
						}
					}
				}
				else
				if (sel_rect == 6)
				{
					if (sel_corner == 2)
					{
						rects[6].height -= delta.y;												

						for (int i = 4; i<7; i++)
						{
							rects[i].width -= delta.x;							
						}
					}
					else
					if (sel_corner == 3)
					{			
						rects[6].height -= delta.y;

						for (int i = 4; i<7; i++)
						{
							rects[i].x -= delta.x;
							rects[i].width += delta.x;							
						}
					}
				}				
			}
		}
		else
		if (cur_type == 3)
		{
			if (sel_corner == -1)
			{
				for (int i = 7; i<16; i++)
				{
					rects[i].x -= delta.x;
					rects[i].y += delta.y;
				}						
			}
			else
			{
				if (sel_rect == 7)
				{
					if (sel_corner == 0)
					{
						rects[7].x -= delta.x;
						rects[7].width += delta.x;

						rects[10].x -= delta.x;
						rects[10].width += delta.x;

						rects[13].x -= delta.x;
						rects[13].width += delta.x;

						for (int i = 7; i<10; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
					else
					if (sel_corner == 1)
					{									
						rects[7].width -= delta.x;
						
						rects[8].x -= delta.x;
						rects[8].width += delta.x;
						
						rects[10].width -= delta.x;

						rects[11].x -= delta.x;
						rects[11].width += delta.x;
						
						rects[13].width -= delta.x;

						rects[14].x -= delta.x;
						rects[14].width += delta.x;

						for (int i = 7; i<10; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
					else
					if (sel_corner == 2)
					{
						rects[7].width -= delta.x;						

						rects[8].x -= delta.x;
						rects[8].width += delta.x;
						
						rects[10].width -= delta.x;

						rects[11].x -= delta.x;
						rects[11].width += delta.x;
						
						rects[13].width -= delta.x;

						rects[14].x -= delta.x;
						rects[14].width += delta.x;

						for (int i = 7; i<10; i++)
						{
							rects[i].height -= delta.y;
						}

						for (int i = 10; i<13; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
					else
					if (sel_corner == 3)
					{			
						rects[7].x -= delta.x;
						rects[7].width += delta.x;

						rects[10].x -= delta.x;
						rects[10].width += delta.x;

						rects[13].x -= delta.x;
						rects[13].width += delta.x;


						for (int i = 7; i<10; i++)
						{
							rects[i].height -= delta.y;
						}

						for (int i = 10; i<13; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
				}
				else
				if (sel_rect == 8)
				{
					if (sel_corner == 1)
					{									
						rects[8].width -= delta.x;
						
						rects[9].x -= delta.x;
						rects[9].width += delta.x;
						
						rects[11].width -= delta.x;

						rects[12].x -= delta.x;
						rects[12].width += delta.x;
						
						rects[14].width -= delta.x;

						rects[15].x -= delta.x;
						rects[15].width += delta.x;

						for (int i = 7; i<10; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
					else
					if (sel_corner == 2)
					{
						rects[8].width -= delta.x;
						
						rects[9].x -= delta.x;
						rects[9].width += delta.x;
						
						rects[11].width -= delta.x;

						rects[12].x -= delta.x;
						rects[12].width += delta.x;
						
						rects[14].width -= delta.x;

						rects[15].x -= delta.x;
						rects[15].width += delta.x;

						for (int i = 7; i<10; i++)
						{
							rects[i].height -= delta.y;
						}

						for (int i = 10; i<13; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
				}
				else
				if (sel_rect == 9)
				{
					if (sel_corner == 1)
					{									
						rects[9].width -= delta.x;
																		
						rects[12].width -= delta.x;
												
						rects[15].width -= delta.x;
						
						for (int i = 7; i<10; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
					else
					if (sel_corner == 2)
					{
						rects[9].width -= delta.x;
																		
						rects[12].width -= delta.x;
												
						rects[15].width -= delta.x;

						for (int i = 7; i<10; i++)
						{
							rects[i].height -= delta.y;
						}

						for (int i = 10; i<13; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
				}
				else
				if (sel_rect == 10)
				{
					if (sel_corner == 2)
					{
						rects[7].width -= delta.x;						

						rects[8].x -= delta.x;
						rects[8].width += delta.x;
						
						rects[10].width -= delta.x;

						rects[11].x -= delta.x;
						rects[11].width += delta.x;
						
						rects[13].width -= delta.x;

						rects[14].x -= delta.x;
						rects[14].width += delta.x;

						for (int i = 10; i<13; i++)
						{
							rects[i].height -= delta.y;
						}

						for (int i = 13; i<16; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
					else
					if (sel_corner == 3)
					{			
						rects[7].x -= delta.x;
						rects[7].width += delta.x;

						rects[10].x -= delta.x;
						rects[10].width += delta.x;

						rects[13].x -= delta.x;
						rects[13].width += delta.x;


						for (int i = 10; i<13; i++)
						{
							rects[i].height -= delta.y;
						}

						for (int i = 13; i<16; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
				}
				else
				if (sel_rect == 11)
				{
					if (sel_corner == 2)
					{
						rects[8].width -= delta.x;
						
						rects[9].x -= delta.x;
						rects[9].width += delta.x;
						
						rects[11].width -= delta.x;

						rects[12].x -= delta.x;
						rects[12].width += delta.x;
						
						rects[14].width -= delta.x;

						rects[15].x -= delta.x;
						rects[15].width += delta.x;

						for (int i = 10; i<13; i++)
						{
							rects[i].height -= delta.y;
						}

						for (int i = 13; i<16; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
				}
				else
				if (sel_rect == 12)
				{					
					if (sel_corner == 2)
					{
						rects[9].width -= delta.x;
																		
						rects[12].width -= delta.x;
												
						rects[15].width -= delta.x;

						for (int i = 10; i<13; i++)
						{
							rects[i].height -= delta.y;
						}

						for (int i = 13; i<16; i++)
						{
							rects[i].y += delta.y;
							rects[i].height += delta.y;
						}
					}
				}
				else
				if (sel_rect == 13)
				{
					if (sel_corner == 2)
					{
						rects[7].width -= delta.x;						

						rects[8].x -= delta.x;
						rects[8].width += delta.x;
						
						rects[10].width -= delta.x;

						rects[11].x -= delta.x;
						rects[11].width += delta.x;
						
						rects[13].width -= delta.x;

						rects[14].x -= delta.x;
						rects[14].width += delta.x;

						for (int i = 13; i<16; i++)
						{
							rects[i].height -= delta.y;
						}						
					}
					else
					if (sel_corner == 3)
					{			
						rects[7].x -= delta.x;
						rects[7].width += delta.x;

						rects[10].x -= delta.x;
						rects[10].width += delta.x;

						rects[13].x -= delta.x;
						rects[13].width += delta.x;

						for (int i = 13; i<16; i++)
						{
							rects[i].height -= delta.y;
						}						
					}
				}
				else
				if (sel_rect == 14)
				{
					if (sel_corner == 2)
					{
						rects[8].width -= delta.x;
						
						rects[9].x -= delta.x;
						rects[9].width += delta.x;
						
						rects[11].width -= delta.x;

						rects[12].x -= delta.x;
						rects[12].width += delta.x;
						
						rects[14].width -= delta.x;

						rects[15].x -= delta.x;
						rects[15].width += delta.x;

						for (int i = 13; i<16; i++)
						{
							rects[i].height -= delta.y;
						}						
					}
				}
				else
				if (sel_rect == 15)
				{					
					if (sel_corner == 2)
					{
						rects[9].width -= delta.x;
																		
						rects[12].width -= delta.x;
												
						rects[15].width -= delta.x;

						for (int i = 13; i<16; i++)
						{
							rects[i].height -= delta.y;
						}						
					}
				}
			}
		}
		else
		{
			if (sel_corner == -1)
			{
				rects[sel_rect].x -= delta.x;
				rects[sel_rect].y += delta.y;
			}
			else
			if (sel_corner == 0)
			{			
				rects[sel_rect].x -= delta.x;
				rects[sel_rect].y += delta.y;

				rects[sel_rect].width += delta.x;
				rects[sel_rect].height += delta.y;
			}
			else
			if (sel_corner == 1)
			{			
				rects[sel_rect].y += delta.y;

				rects[sel_rect].width -= delta.x;
				rects[sel_rect].height += delta.y;
			}
			else
			if (sel_corner == 2)
			{
				rects[sel_rect].width -= delta.x;
				rects[sel_rect].height -= delta.y;
			}
			else
			if (sel_corner == 3)
			{			
				rects[sel_rect].x -= delta.x;

				rects[sel_rect].width += delta.x;
				rects[sel_rect].height -= delta.y;
			}
		}
		
		Redraw();
	}	
}

void ImageWidget::Update()
{
	Vector2 delta(0.0f);

	if ( core->Controls()->ControlState(DIK_LEFT, IControlsService::Activated) )
	{
		delta.x += 1.0f;			
	}
	else
	if ( core->Controls()->ControlState(DIK_RIGHT, IControlsService::Activated) )
	{
		delta.x -= 1.0f;
	}
	else
	if ( core->Controls()->ControlState(DIK_UP, IControlsService::Activated) )
	{
		delta.y += 1.0f;			
	}
	else
	if ( core->Controls()->ControlState(DIK_DOWN, IControlsService::Activated) )
	{
		delta.y -= 1.0f;			
	}
		
	MoveRect(delta, 2);
}

void ImageWidget::OnMouseUp()
{
	drag_type = 0;
	ReleaseCapture(); 
}

void ImageWidget::OnRightMouseDown()
{		
	drag_type = 1;	
	GetCursorPos(&ms_point);	
	SetCapture(handle);
}

void ImageWidget::OnRightMouseUp()
{
	drag_type = 0;
	ReleaseCapture(); 
}

void ImageWidget::ActualPixels()
{
	if (!image) return;

	pixel_density = 1.0f;

	origin.x = (width - pixel_density * img_width) * 0.5f / pixel_density;
	origin.y = (height - (height - pixel_density * img_height) * 0.5f) / pixel_density;

	Redraw();

	delta_mouse = 0.0f;
}

void ImageWidget::FitImage()
{
	if (!image) return;
	
	pixel_density = (float)(width - 20) / (float)img_width;
	
	if ((float)img_height * pixel_density > height - 20)
	{
		pixel_density = (float)(height - 20) / (float)img_height;
	}

	origin.x = (width - pixel_density * img_width) * 0.5f / pixel_density;
	origin.y = (height - (height - pixel_density * img_height) * 0.5f) / pixel_density;

	Redraw();

	delta_mouse = 0.0f;
}