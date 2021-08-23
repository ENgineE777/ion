#include "Core/Core.h"

DebugSphere::DebugSphere()
{
	vbuffer = null;
	tech = null;

	ringsNum = 16;
	numSides = 16;
}

DebugSphere::~DebugSphere()
{		
}

bool DebugSphere::Init()
{		
	unsigned int NumVerts = numSides * (ringsNum + 1);

    vbuffer = en_core.Render()->CreateVertexBuffer(sizeof(Vector)*NumVerts, sizeof(Vector),  _FL_);

    vbuffer->StartDesc();
    vbuffer->AddDescElem(VertexBuffer::position, VertexBuffer::float3, "position");  
    vbuffer->EndDesc();

    Vector* data = (Vector*)vbuffer->Lock();

    float ThetaAdd = PI / ringsNum;
    float PhiAdd = 2.0f * PI / numSides;

    float ThetaCur = 0.0f;

    unsigned int WriteIndex = 0;
    for (dword i = 0; i <= ringsNum; ++i)
    {
        float y = cosf(ThetaCur);

        float SinTheta = sinf(ThetaCur);
        float PhiCur = 0.0f;

        for (dword j = 0; j < numSides; ++j)
        {
            float x = SinTheta * sinf(PhiCur);
            float z = -SinTheta * cosf(PhiCur);

            data[WriteIndex].x = x;
            data[WriteIndex].y = y;
            data[WriteIndex].z = z;           
            WriteIndex++;

            PhiCur += PhiAdd;
        }

        ThetaCur += ThetaAdd;
    }

    vbuffer->Unlock();
    
    unsigned int NumPrimitives = 2 * ringsNum * numSides;

    ibuffer = en_core.Render()->CreateIndexBuffer(3 * NumPrimitives * sizeof(unsigned int), _FL_);

    unsigned int* pData = (unsigned int*)ibuffer->Lock();

    WriteIndex = 0;
    for (unsigned int i = 0; i < ringsNum; ++i)
    {
        int BaseTopIdx = (i+0) * numSides;
        int BaseBottomIdx = (i+1) * numSides;

        for (unsigned int j = 0; j < numSides; ++j)
        {
            pData[WriteIndex] = (BaseTopIdx + ((j + 0) % numSides));
            ++WriteIndex;
            pData[WriteIndex] = (BaseBottomIdx + ((j + 1) % numSides));
            ++WriteIndex;
            pData[WriteIndex] = (BaseBottomIdx + ((j + 0) % numSides));
            ++WriteIndex;

            pData[WriteIndex] = (BaseBottomIdx + ((j + 1) % numSides));
            ++WriteIndex;
            pData[WriteIndex] = (BaseTopIdx + ((j + 0) % numSides));
            ++WriteIndex;
            pData[WriteIndex] = (BaseTopIdx + ((j + 1) % numSides));
            ++WriteIndex;
        }
    }

    ibuffer->Unlock();

	//tech = en_core.Render()->CreateRenderTechnique("DebugSolid");		
	//en_core.Render()->Tasks()->AddTask("debug_sphere", this, this, (TaskHolder::Task)&DebugSphere::Draw, 0);

	return true;
}

void DebugSphere::AddSphere(const Vector& pos, float radius, Color color)
{
	Sphere* sphere;

	if (spheres.size()>1000) return;

	spheres.push_back(Sphere());
	sphere = &spheres[spheres.size()-1];

	sphere->pos = pos;
	sphere->radius = radius;
	sphere->color = (Vector4&)color;	
}

void DebugSphere::Draw(float dt, int level)
{
	if (spheres.size()==0) return;	
	
	if (!tech)
	{
		spheres.clear();
		return;
	}
	
	en_core.Render()->SetRenderTechnique(tech);

	en_core.Render()->SetVertexBuffer(0,vbuffer);
	en_core.Render()->SetIndexBuffer(ibuffer);

	Matrix view_proj;
	en_core.Render()->Display()->GetViewProj(view_proj);		
			
	int index = 0;

	en_core.Render()->State()->SetAlphaBlend(1);
	en_core.Render()->State()->SetBlendFunc(RenderService::argSrcAlpha, RenderService::argInvSrcAlpha);

	for (int i=0;i<spheres.size();i++)
	{
		Sphere* sphere = &spheres[i];

		Matrix mat;
		mat.Scale(sphere->radius);
		mat.Pos() = sphere->pos;

		mat *= view_proj;

		tech->VS_SetMatrix("modelViewProj", mat);	
		tech->PS_SetVector4("color", sphere->color);	
		tech->PS_SetVector4("fadingFactor", Vector4(1,1,1,1));

		en_core.Render()->DrawIndexedPrimitive(RenderService::trianglesList, 0, 2 * ringsNum * numSides);
	}	

	en_core.Render()->State()->SetAlphaBlend(0);

	spheres.clear();	
}

void DebugSphere::Release()
{
	if (vbuffer)
	{
		vbuffer->Release();
		vbuffer = null;
	}

	if (ibuffer)
	{
		ibuffer->Release();
		ibuffer = null;
	}	
}