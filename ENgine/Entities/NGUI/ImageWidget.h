
#pragma once

#include "Core/Core.h"
#include "Platforms/PC/GUI/Widgets.h"

class DEF_EXPORT ImageWidget : public BaseWidget
{				
public:

	struct Rect
	{
		int x,y;
		int width, height;
	};
	
	Rect rects[16];

	int sel_rect;
	int sel_corner;
	
	int cur_type;
	bool slice_in_ref;
	int scale_type;
	int vert_align;	
	int horz_align;
	int tex_mode;

	int  texMode;
	int  texFilter;
	char texName[512];

	float pixel_density;

	RenderService::TextureData texture_data;

	HBITMAP image;

	Color color;

	Vector2 origin;
	
	int img_width;
	int img_height;

	float scale;

	int drag_type;
	POINT ms_point;

	Vector2 delta_mouse;

	ImageWidget(int id, BaseWidget* parent, const char* txt, float x, float y, float w, float h,EAutoSizeType sizerType = BaseWidget::eAutoSizeNone);
	virtual ~ImageWidget();

	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);
		
	void SetImage(const char* img);
	void SelectStartEndRect(int& start_rect, int& end_rect);

	virtual void Draw();

	virtual void OnMouseDown();	
	virtual void OnMouseUp();	

	virtual void OnRightMouseDown();	
	virtual void OnRightMouseUp();	

	virtual void OnMouseMove();

	void ActualPixels();
	void FitImage();

	void MoveRect(Vector2 delta, int drag_type);
	void Update();
};
