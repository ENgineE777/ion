
#pragma once
#include "Core/ICore.h"

#ifdef PC

class TextureInstance;

class Gizmo
{
public:
    bool   bViewPortResized;
	Matrix transform;
    int    mode;
    int    selAxis;
    bool   useLocalSpace;    
	Vector ms_dir;
    bool   enabled;
    bool   colored;        
    float  scale;
	bool   transform_started;
	bool   abs_units;
	int    type;
	float  width, height;
	Vector rotation;

	Vector2 origin;
	Vector2 ancorns[8];

	TextureInstance* anchorn_tex;
	TextureInstance* dot_tex;

	Matrix center_transform;
	Matrix rot_matrix;

	Vector2 prev_ms;

	HCURSOR cr_normal, cr_move, cr_resize, cr_rotate;

    Gizmo();
    Color CheckColor(int axis);        
	void DrawAxis(int axis);        
    void DrawCircle(int axis);        
    bool CheckInersection(Vector pos, Vector pos2,
                          float mx, float my,
                          Vector trans, bool check_trans,
                          Matrix view, Matrix view_proj);

    bool MouseProcess(int axis, float mx, float my);        
	void MouseProcess(float mx, float my, float scr_mx, float scr_my);
    bool MouseMove(float mx, float my, float scr_mx, float scr_my);	
	bool KeyboardProcess();

    void Render();        
};

#endif