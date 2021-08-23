
#pragma once

#include "Common/Color.h"

class DebugSphere : public TaskHoldersOwner
{		
	struct Sphere
	{
		Vector pos;
		Vector4 color;
		float radius;		
	};

	std::vector<Sphere> spheres;	

	VertexBuffer* vbuffer;
	IndexBuffer*  ibuffer;
	RenderTechnique* tech;	

	int ringsNum, numSides;

public:
	
	DebugSphere();
	virtual ~DebugSphere();

	virtual bool Init();
	void AddSphere(const Vector& pos, float radius, Color color);	
	void Draw(float dt, int level);
	void Release();
};
