#pragma once

#include "Common/TaskHolder.h"
#include "Core/Font/FontRes.h"

class DEF_EXPORT FontInstance	
{	
	FontRes* res;
	float    row_gap;

public: 
			
	FontInstance(FontRes* res, float set_gap);	

	void Print(Matrix& transform, ClipRect* rect, std::vector<FontRes::LineData>& lines_data, float font_scale, Color color, TextAlign align,  const char* text);	
	
	int GetHeight();
	int GetCharHeight();
	float GetRowGap();

	int GetWidth(const char* str);	
	int GetHeight(const char* str, ClipRect* rect, float font_scale, TextAlign align);		

	void CalcLinesData(const char* text, std::vector<FontRes::LineData>& lines_data, int width, float font_scale, TextAlign align);
	int  GetCharPosition(const char* text, float& pos_x, int width, int start_pos, float cur_pos_x, TextAlign align);	

	FontRes::Glyph* GetCharGlyph(wchar_t ch);

    FontInstance*  Clone(){return res->CreateReference(row_gap);}

	virtual void Release();
};
