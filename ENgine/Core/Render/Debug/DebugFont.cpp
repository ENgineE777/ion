#include "Core/Core.h"

DebugFont::DebugFont()
{
	font = null;
	texts.reserve(32);
}

DebugFont::~DebugFont()
{	
}

bool DebugFont::Init()
{	
	en_core.Render()->Tasks()->AddTask("debug_font", this, this, (TaskHolder::Task)&DebugFont::Draw, 0);

	return true;
}

void DebugFont::Print(float x, float y, ElemAlign corner, Color color, const char* text)
{
	if (!font)
	{
		font = en_core.Font()->CreateFont("helveticaneuecur", false, false, 15, 1.5f, 0.15f);		
        
		if (!font)
		{
			return;
		}
	}

	if (texts.size()>1000) return;

	texts.push_back(Text());

	Text* txt = &texts[texts.size()-1];

	txt->pos = Vector(x,y);
	txt->dist = 0.0f;
	txt->is3d = false;
	txt->color = color;
	String::Copy(txt->text, 256, text);
	
	if (!font)
		return;
	if (corner == 1 || corner == 2)
	{		
		txt->pos.x = en_core.Render()->Display()->GetWidth() - txt->pos.x - font->GetWidth(txt->text);
	}

	if (corner == 2 || corner == 3)
	{
		txt->pos.y = en_core.Render()->Display()->GetHeight() - txt->pos.y - font->GetHeight();
	}
}

void DebugFont::Print3D(Vector pos, float dist, Color color, const char* text)
{
	if (!font) return;

	if (texts.size()>1000) return;

	texts.push_back(Text());

	Text* txt = &texts[texts.size()-1];

	txt->pos = pos;	
	txt->dist = dist;
	txt->is3d = true;
	txt->color = color;
	String::Copy(txt->text, 256, text);
}

void DebugFont::Draw(float dt, int level)
{
	Matrix mat;

	for (int i=0;i<texts.size();i++)
	{
		Text* txt = &texts[i];

		if (txt->is3d)
		{
			Vector pos = en_core.Render()->Display()->TransformToScreen(txt->pos, 1);

			if (pos.z>0 && pos.z<txt->dist)
			{		
				mat.Pos() = Vector(pos.x * en_core.Render()->Display()->GetWidth(), pos.y * en_core.Render()->Display()->GetHeight(), 0);
				font->Print(mat, null, line_indices, 1.0f, txt->color, text_left, txt->text);
			}
		}		
	}

	for (int i=0;i<texts.size();i++)
	{
		Text* txt = &texts[i];

		if (!txt->is3d)
		{
			mat.Pos() = Vector(txt->pos.x, txt->pos.y, 0);
			font->Print(mat, null, line_indices, 1.0f, txt->color, text_left, txt->text);
		}		
	}

	texts.clear();
}

void DebugFont::Release()
{
	if (font)
	{
		RELEASE(font)		
	}
}