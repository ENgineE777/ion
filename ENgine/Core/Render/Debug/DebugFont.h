
#pragma once

#include "Common/Color.h"
#include "Core/Font/FontRes.h"

class FontInstance;

class DebugFont : public TaskHoldersOwner
{	
	FontInstance* font;	

	struct Text
	{
		Vector pos;
		float dist;
		bool is3d;
		Color color;
		char text[256];
	};
	
	std::vector<Text> texts;

	std::vector<FontRes::LineData> line_indices;

public:
	
	DebugFont();
	virtual ~DebugFont();

	virtual bool Init();
	void Print(float x, float y, ElemAlign corner, Color color, const char* text);
	void Print3D(Vector pos, float dist, Color color, const char* text);
	void Draw(float dt, int level);
	void Release();
};
