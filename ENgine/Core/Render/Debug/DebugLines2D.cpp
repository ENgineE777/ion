#include "Core/Core.h"

DebugLines2D::DebugLines2D()
{
	vbuffer = null;
	tech = null;
}

DebugLines2D::~DebugLines2D()
{
}

bool DebugLines2D::Init()
{	
	vbuffer = en_core.Render()->CreateVertexBuffer(2 * 5000 * sizeof(DebugVertex), sizeof(DebugVertex), _FL_);			

	vbuffer->StartDesc();
	
	vbuffer->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");		
	vbuffer->AddDescElem(VertexBuffer::color,VertexBuffer::tp_color, "color");	
	
	vbuffer->EndDesc();	
								
	tech = en_core.Render()->CreateRenderTechnique("DebugLine2D");	

	en_core.Render()->Tasks()->AddTask("debug_lines2d", this, this, (TaskHolder::Task)&DebugLines2D::Draw, 0);

	return true;
}

void DebugLines2D::AddLine(const Vector2& p1, Color color1, const Vector2& p2, Color color2)
{
	Line* line;

	if (lines.size()>50000)
	{
		core->TraceTo("render", "TOOOOOOO MANY LINES !!!!!!!");
		return;
	}

	lines.push_back(Line());
	line = &lines[lines.size()-1];

	line->p1.x = -1.0f + p1.x/en_core.Render()->Display()->GetWidth() * 2.0f;
	line->p1.y =  1.0f - p1.y/en_core.Render()->Display()->GetHeight() * 2.0f;
	line->p1.z = 0.5f;

	//line->color1 = Vector4(color1.r,color1.g,color1.b,color1.a);
	line->color1 = color1.Get();

	line->p2.x = -1.0f + p2.x/en_core.Render()->Display()->GetWidth() * 2.0f;
	line->p2.y =  1.0f - p2.y/en_core.Render()->Display()->GetHeight() * 2.0f;
	line->p2.z = 0.5f;

	//line->color2 = Vector4(color2.r,color2.g,color2.b,color2.a);
	line->color2 = color2.Get();
}

void DebugLines2D::Draw(float dt, int level)
{
	if (lines.size()==0) return;	
	
	if (!tech)
	{
		lines.clear();
		return;
	}
	
	en_core.Render()->SetRenderTechnique(tech);
	
	DebugVertex* v = (DebugVertex*)vbuffer->Lock();	    

	int index = 0;

	for (int i=0;i<lines.size();i++)
	{
		Line* line = &lines[i];		

		v[index*2 + 0].pos = line->p1;
		v[index*2 + 0].color = line->color1;		
		
		v[index*2 + 1].pos = line->p2;
		v[index*2 + 1].color = line->color2;		

		index++;

		if (index>4998)
		{
			vbuffer->Unlock();			

			en_core.Render()->SetVertexBuffer(0,vbuffer);
			en_core.Render()->DrawPrimitive(RenderService::linesList, 0, index);
			en_core.Render()->SetVertexBuffer(0,null);
			index = 0;			
			
			DebugVertex* v = (DebugVertex*)vbuffer->Lock();
		}
	}
	
	vbuffer->Unlock();	

	if (index>0)
	{
		en_core.Render()->SetVertexBuffer(0,vbuffer);
		en_core.Render()->DrawPrimitive(RenderService::linesList, 0, index);	
		en_core.Render()->SetVertexBuffer(0,null);
	}

	lines.clear();
}

void DebugLines2D::Release()
{
	if (vbuffer)
	{
		vbuffer->Release();
		vbuffer = null;
	}
}