
#pragma once

#include "Common/Common.h"

class DebugLines2D : public TaskHoldersOwner
{		
	struct Line
	{
		Vector p1;
		dword  color1;
		Vector p2;
		dword  color2;
	};

	struct DebugVertex
	{
		Vector pos;
		dword  color;
	};

	VertexBuffer* vbuffer;
	RenderTechnique* tech;

	std::vector<Line> lines;

	int requestedLineNumbers;

public:
	
	DebugLines2D();
	virtual ~DebugLines2D();

	virtual bool Init();
	void AddLine(const Vector2& p1, Color color1, const Vector2& p2, Color color2);
	void Draw(float dt, int level);
	void Release();
};
