#include "Core/Core.h"

FontInstance::FontInstance(FontRes* set_res, float set_gap)
{
	res = set_res;
	row_gap = set_gap;
}

void FontInstance::Print(Matrix& transform, ClipRect* rect, std::vector<FontRes::LineData>& lines_data, float font_scale, Color color, TextAlign align,  const char* text)
{
	if (!text[0]) return;

	res->SetRowGap(row_gap);
	res->Print(transform, rect, null, lines_data, font_scale, color, align, text);
}

int FontInstance::GetWidth(const char* str)
{
	res->SetRowGap(row_gap);

	return res->GetWidth(str, 0, false);
}

int FontInstance::GetHeight(const char* str, ClipRect* rect, float font_scale, TextAlign align)
{
	res->SetRowGap(row_gap);

	return res->GetHeight(str, rect, font_scale, align);
}

int FontInstance::GetHeight()
{
	return res->GetHeight();
}

int FontInstance::GetCharHeight()
{
	return res->GetCharHeight();
}

float FontInstance::GetRowGap()
{
	res->SetRowGap(row_gap);
	return res->GetRowGap();
}

void FontInstance::CalcLinesData(const char* text, std::vector<FontRes::LineData>& lines_data, int width, float font_scale, TextAlign align)
{
	res->SetRowGap(row_gap);
	res->CalcLinesData(text, lines_data, width, font_scale, align);
}

int FontInstance::GetCharPosition(const char* text, float& pos_x, int width, int start_pos, float cur_pos_x, TextAlign align)
{
	res->SetRowGap(row_gap);
	return res->GetCharPosition(text, pos_x, width, start_pos, cur_pos_x, align);
}

FontRes::Glyph* FontInstance::GetCharGlyph(wchar_t ch)
{
	return res->GetCharGlyph(ch);
}

void FontInstance::Release()
{
    res->DecRef();
	delete this;
}