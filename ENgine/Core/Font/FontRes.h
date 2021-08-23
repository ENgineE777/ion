#pragma once

#include "Core/Resources/Resources.h"

class  Texture;

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <freetype/ftstroke.h>

#include "FontCharmap.h"

class FontInstance;
class IFileBuffer;

class FontRes : public BaseResource
{	
public:

	struct Glyph
	{		
		int   width;
		int   height;
		float x_offset;
		float y_offset;
		float x_advance;
		float u;
		float v;
		float du;
		float dv;
		int   skip;
	};

	int cur_row_gap;

	struct LineData
	{
		int index;
		float offset;
	};

protected:	

	Texture* tex;
	int      cur_tex_x;
	int      cur_tex_y;
	int      cur_tex_row_height;

#ifdef PC
	byte*    tex_buffer;
#endif

	int height;	
	float outline_thin;

	int tex_w;
	int tex_h;

	FT_Stroker stroker;
	FT_Face face;
	FT_Library library;		
	
	bool need_update_tex;	
	IFileBuffer* font_fb;

    bool  IsSeparator(const wchar_t symbol);

public: 	
	
	FontRes(FT_Library library, const char* fl_name, int hgt, float outline_thin);
	virtual bool Load();
	Glyph* GenerateChar(wchar_t ch);
	int  GenerateCharHeight(wchar_t ch);
	FontInstance* CreateReference(float set_gap);
		
	void Print(Matrix& transform, ClipRect* rect, ClipRect* clip_rect, std::vector<LineData>& lines_data, float font_scale, Color color, TextAlign align,  const char* text);

	int GetHeight();
	int GetCharHeight();
	float GetRowGap();


	int GetWidth(const char* str, int startIndex, bool break_on_whitespace);	

	int GetHeight(const char* text, ClipRect* rect, float font_scale, TextAlign align);	

	int FindNextWhitespace(const char* text, int start_index, float scr_x, float x2, float font_scale);
	
	void CalcLinesData(const char* text, std::vector<LineData>& lines_data, int width, float font_scale, TextAlign align);
	int  GetCharPosition(const char* text, float& pos_x, int width, int start_pos, float cur_pos_x, TextAlign align);	

	Glyph* GetCharGlyph(int ch);

	void SetRowGap(float set_gap);

	void Reset();	
	void Restore();	

	virtual void Release();
	
#ifdef PC
	void UpdateTexture();
#endif

	vector<Glyph> glyphs;
	FontCharmap glyphList;	
	Glyph* CheckGlyph(wchar_t characterCode);

    static unsigned vTextSeparatorsSize;
    static char     vTextSeparators[];
};
