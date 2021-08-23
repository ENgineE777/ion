
#pragma once

#include "Common/Color.h"

class DebugLines : public TaskHoldersOwner
{		
	struct Line
	{
		Vector p1;
		dword color1;
		Vector p2;
		dword color2;
	};

	struct DebugVertex
	{
		Vector pos;
		dword color;
	};

	std::vector<Line> lines;
	std::vector<Line> linesNoZ;	

	VertexBuffer* vbuffer;
	RenderTechnique* tech;
	RenderTechnique* techNoZ;

public:
	
	DebugLines();
	virtual ~DebugLines();

	virtual bool Init();
	void AddLine(const Vector& p1, Color color1, const Vector& p2, Color color2, bool noZ);
	void DrawLines(RenderTechnique* rt, std::vector<Line>& ln);
	void Draw(float dt, int level);
	void Release();
};
