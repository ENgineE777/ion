#include "Core/Core.h"

DebugLines::DebugLines()
{
	vbuffer = null;
	tech = null;
}

DebugLines::~DebugLines()
{
}

bool DebugLines::Init()
{	
	vbuffer = en_core.Render()->CreateVertexBuffer(2 * 5000 * sizeof(DebugVertex), sizeof(DebugVertex), _FL_);			

	vbuffer->StartDesc();
	
	vbuffer->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");		
	vbuffer->AddDescElem(VertexBuffer::color,VertexBuffer::tp_color, "color");	
	
	vbuffer->EndDesc();	
								
	tech = en_core.Render()->CreateRenderTechnique("DebugLine");	
	techNoZ = en_core.Render()->CreateRenderTechnique("DebugLineNoZ");

	en_core.Render()->Tasks()->AddTask("debug_render", this, this, (TaskHolder::Task)&DebugLines::Draw, 0);

	return true;
}

void DebugLines::AddLine(const Vector& p1, Color color1, const Vector& p2, Color color2, bool noZ)
{
	return;

	Line* line;

	if (noZ)
	{
		if (linesNoZ.size()>50000)
		{
			core->TraceTo("render", "TOOOOOOO MANY LINES (NoZ) !!!!!!!");
			return;
		}

		linesNoZ.push_back(Line());
		line = &linesNoZ[linesNoZ.size()-1];
	}
	else
	{	
		if (lines.size()>50000)
		{
			core->TraceTo("render", "TOOOOOOO MANY LINES !!!!!!!");
			return;
		}

		lines.push_back(Line());
		line = &lines[lines.size()-1];
	}

	line->p1 = p1;	
	line->color1 = color1.Get();

	line->p2 = p2;
	line->color2 = color2.Get();	
	
}

void DebugLines::DrawLines(RenderTechnique* rt, std::vector<Line>& ln)
{	
	if (ln.size()==0) return;	
	
	if (!rt)
	{
		ln.clear();
		return;
	}
	
	en_core.Render()->SetRenderTechnique(rt);

	Matrix view_proj;
	en_core.Render()->Display()->GetViewProj(view_proj);		

	rt->VS_SetMatrix("modelViewProj", view_proj);	
		
	DebugVertex* v = (DebugVertex*)vbuffer->Lock();	    

	int index = 0;

	for (int i=0;i<ln.size();i++)
	{
		Line* line = &ln[i];		

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

	ln.clear();
}

void DebugLines::Draw(float dt, int level)
{
	DrawLines(techNoZ, linesNoZ);	
	DrawLines(tech, lines);
}

void DebugLines::Release()
{
	if (vbuffer)
	{
		vbuffer->Release();
		vbuffer = null;
	}
}