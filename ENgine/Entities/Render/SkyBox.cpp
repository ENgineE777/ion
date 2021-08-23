
#include "SkyBox.h"
#include "Editor/Helpers/Gizmo.h"

ENTITY(SkyBox)
ENTITY_PARAMS(SkyBox,"SkyBox","Environment","SkyBox")

void SkyBox::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG(side0)
	PROP_REG(side1)
	PROP_REG(side2)
	PROP_REG(side3)
	PROP_REG(side4)
	PROP_REG(side5)			
}

void SkyBox::Init()
{	
	power = 1.0f;

	float k=600;	
	float k1=0.0f,k2=1.0f;

	skyboxBuffer = en_core.Render()->CreateVertexBuffer(36 * sizeof(Mesh::Vertex), sizeof(Mesh::Vertex), _FL_);			

	skyboxBuffer->StartDesc();
	
	skyboxBuffer->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");		
	skyboxBuffer->AddDescElem(VertexBuffer::normal,VertexBuffer::float3, "normal");	
	skyboxBuffer->AddDescElem(VertexBuffer::texcoord,VertexBuffer::float2, "tex");	
	
	skyboxBuffer->EndDesc();	

	Mesh::Vertex* v = (Mesh::Vertex*)skyboxBuffer->Lock();

	v[1].pos = Vector(-1.0f*k,-1.0f*k*0.75f, 1.0f*k );
	v[1].uv  = Vector2(k2, k2);
	v[0].pos = Vector(-1.0f*k, 1.0f*k*0.75f, 1.0f*k );
	v[0].uv  = Vector2(k2, k1);
	v[2].pos = Vector( 1.0f*k, 1.0f*k*0.75f, 1.0f*k );
	v[2].uv  = Vector2(k1, k1);
	v[4].pos = Vector( 1.0f*k, 1.0f*k*0.75f, 1.0f*k );
	v[4].uv  = Vector2(k1, k1);
	v[3].pos = Vector( 1.0f*k,-1.0f*k*0.75f, 1.0f*k );
	v[3].uv  = Vector2(k1, k2);
	v[5].pos = Vector(-1.0f*k,-1.0f*k*0.75f, 1.0f*k );
	v[5].uv  = Vector2(k2, k2);
	
	v[7].pos = Vector(-1.0f*k, 1.0f*k*0.75f,-1.0f*k );
	v[7].uv  = Vector2(k1, k1);
	v[6].pos = Vector(-1.0f*k,-1.0f*k*0.75f,-1.0f*k );
	v[6].uv  = Vector2(k1, k2);
	v[8].pos = Vector( 1.0f*k, 1.0f*k*0.75f,-1.0f*k );
	v[8].uv  = Vector2(k2, k1);
	v[9].pos = Vector( 1.0f*k,-1.0f*k*0.75f,-1.0f*k );
	v[9].uv  = Vector2(k2, k2);
	v[11].pos = Vector( 1.0f*k, 1.0f*k*0.75f,-1.0f*k );
	v[11].uv  = Vector2(k2, k1);
	v[10].pos = Vector(-1.0f*k,-1.0f*k*0.75f,-1.0f*k );
	v[10].uv  = Vector2(k1, k2);


	v[13].pos = Vector(-1.0f*k,-1.0f*k*0.75f,-1.0f*k );
	v[13].uv  = Vector2(k2, k2);
	v[12].pos = Vector(-1.0f*k, 1.0f*k*0.75f,-1.0f*k );
	v[12].uv  = Vector2(k2, k1);
	v[14].pos = Vector(-1.0f*k, 1.0f*k*0.75f, 1.0f*k );
	v[14].uv  = Vector2(k1, k1);
	v[15].pos = Vector(-1.0f*k, 1.0f*k*0.75f, 1.0f*k );
	v[15].uv  = Vector2(k1, k1);
	v[17].pos = Vector(-1.0f*k,-1.0f*k*0.75f, 1.0f*k );
	v[17].uv  = Vector2(k1, k2);
	v[16].pos = Vector(-1.0f*k,-1.0f*k*0.75f,-1.0f*k );
	v[16].uv  = Vector2(k2, k2);


	v[19].pos = Vector(1.0f*k, 1.0f*k*0.75f,-1.0f*k );
	v[19].uv  = Vector2(k1, k1);
	v[18].pos = Vector(1.0f*k,-1.0f*k*0.75f,-1.0f*k );
	v[18].uv  = Vector2(k1, k2);
	v[20].pos = Vector(1.0f*k, 1.0f*k*0.75f, 1.0f*k );
	v[20].uv  = Vector2(k2, k1);
	v[21].pos = Vector(1.0f*k, 1.0f*k*0.75f, 1.0f*k );
	v[21].uv  = Vector2(k2, k1);
	v[23].pos = Vector(1.0f*k,-1.0f*k*0.75f,-1.0f*k );
	v[23].uv  = Vector2(k1, k2);
	v[22].pos = Vector(1.0f*k,-1.0f*k*0.75f, 1.0f*k );
	v[22].uv  = Vector2(k2, k2);

	v[24].pos = Vector(-1.0f*k, 1.0f*k*0.75f,-1.0f*k );
	v[24].uv  = Vector2(k1, k1);
	v[26].pos = Vector( 1.0f*k, 1.0f*k*0.75f,-1.0f*k );
	v[26].uv  = Vector2(k1, k2);
	v[25].pos = Vector( 1.0f*k, 1.0f*k*0.75f, 1.0f*k );
	v[25].uv  = Vector2(k2, k2);
	v[27].pos = Vector( 1.0f*k, 1.0f*k*0.75f, 1.0f*k );
	v[27].uv  = Vector2(k2, k2);
	v[29].pos = Vector(-1.0f*k, 1.0f*k*0.75f, 1.0f*k );
	v[29].uv  = Vector2(k2, k1);
	v[28].pos = Vector(-1.0f*k, 1.0f*k*0.75f,-1.0f*k );
	v[28].uv  = Vector2(k1, k1);


	v[31].pos = Vector( 1.0f*k,-1.0f*k*0.75f,-1.0f*k );
	v[31].uv  = Vector2(k2, k2);
	v[30].pos = Vector(-1.0f*k,-1.0f*k*0.75f,-1.0f*k );
	v[30].uv  = Vector2(k1, k2);
	v[32].pos = Vector( 1.0f*k,-1.0f*k*0.75f, 1.0f*k );
	v[32].uv  = Vector2(k2, k1);
	v[33].pos = Vector( 1.0f*k,-1.0f*k*0.75f, 1.0f*k );
	v[33].uv  = Vector2(k2, k1);
	v[35].pos = Vector(-1.0f*k,-1.0f*k*0.75f,-1.0f*k );
	v[35].uv  = Vector2(k1, k2);
	v[34].pos = Vector(-1.0f*k,-1.0f*k*0.75f, 1.0f*k );
	v[34].uv  = Vector2(k1, k1);

	for (int i=0;i<36;i++)
	{
		v[i].normal = 0.5f;

#if defined(IOS) || defined(ANDROID)
		v[i].uv.y = 1-v[i].uv.y;
#endif
	}
	skyboxBuffer->Unlock();	

	//tech = en_core.Render()->CreateRenderTechnique("RTM_Diffuse");

	for (int i=0;i<6;i++) sides[i] = null;
	
	en_core.Render()->Tasks()->AddTask("geometry", this, Scene(), (TaskHolder::Task)&SkyBox::Draw, -10);
	
	angle = 0.0f;
}

SkyBox::~SkyBox()
{
	en_core.Render()->Tasks()->DelTask("geometry", this);
}

void SkyBox::ReInit()
{
	RELEASE(sides[0])
	sides[0] = en_core.Render()->LoadTexture(side0.c_str(), false, _FL_);

	RELEASE(sides[1])
	sides[1] = en_core.Render()->LoadTexture(side1.c_str(), false, _FL_);

	RELEASE(sides[2])
	sides[2] = en_core.Render()->LoadTexture(side2.c_str(), false, _FL_);

	RELEASE(sides[3])
	sides[3] = en_core.Render()->LoadTexture(side3.c_str(), false, _FL_);

	RELEASE(sides[4])
	sides[4] = en_core.Render()->LoadTexture(side4.c_str(), false, _FL_);

	RELEASE(sides[5])
	sides[5] = en_core.Render()->LoadTexture(side5.c_str(), false, _FL_);

	for (int i=0;i<6;i++)
	{
		if (sides[i])		
		{
			sides[i]->SetAdress(Texture::addressClamp);
		}
	}
}

void SkyBox::Begin()
{
	init_pow = 1.0f;
	needed_pow = 1.0f;
	chgspd_pow = 1.0f;
}

void SkyBox::End()
{
	//power = init_pow;	
}

void SkyBox::Draw(float fDelta,int level)
{			
	if (!tech) return;

	en_core.Render()->SetRenderTechnique(tech);

#ifdef IOS
	en_core.Render()->State()->SetCulling(RenderService::cullNone);
#endif
	
	if (Scene()->IsRunning())
	{	
		//SliceChangeValue(power, needed_pow, chgspd_pow, fDelta);
	}

	Matrix view_proj;
	en_core.Render()->Display()->GetViewProj(view_proj);

	tech->VS_SetMatrix("modelViewProj", view_proj);


	en_core.Render()->SetVertexBuffer(0, skyboxBuffer);	

	float f[4];
	f[0]=f[1]=f[2]=1.0f;
	f[3]=power;

	//tech->SetFloatArray("Color",f,4);	

	for (int i=0;i<6;i++)
	{
		tech->PS_SetTexture("slot0", sides[i]);

		en_core.Render()->DrawPrimitive( RenderService::trianglesList, 6*i , 2);

		/*if (power==1.0f)
		{
			en_core.Render()->DrawPrimitive( RenderService::trianglesList, 6*i , 2);
		}
		else
		{
			en_core.Render()->DrawPrimitive( RenderService::trianglesList, 6*i , 2);
			Render()->DrawPrimitive( RPT_TriangleList, 6*i , 2, tech, "SkyBoxBlend" );
		}*/
	}
}

#ifdef PC
void SkyBox::DataFromGizmo()
{
	gizmo->transform.Pos() = transform.Pos();
	transform=gizmo->transform;
	gizmo->transform.Pos() = 0.0f;

}

void SkyBox::DataToGizmo()
{
	gizmo->transform = transform;	
	gizmo->transform.Pos() = 0.0f;
}
#endif

void SkyBox::Command(const char* command,int NumParams,const char** params)
{
	return;
	
	/*if (stricmp(command,"appear")==0)
	{
		if (NumParams>=1)
		{
			needed_pow = 1.0f;

			chgspd_pow = 0.0f;

			char str[64];

			strcpy(str,params[0]);
			sscanf (str, "%f", &chgspd_pow);

			if (chgspd_pow>0.0f)
			{
				chgspd_pow = fabs(needed_pow-power)/chgspd_pow;
			}
			else
			{
				chgspd_pow = 1.0f;
			}			
		}
	}
	else
	if (stricmp(command,"vanish")==0)
	{
		if (NumParams>=1)
		{
			needed_pow = 0.0f;

			chgspd_pow = 0.0f;

			char str[64];		

			strcpy(str,params[0]);
			sscanf (str, "%f", &chgspd_pow);

			if (chgspd_pow>0.0f)
			{
				chgspd_pow = fabs(needed_pow-power)/chgspd_pow;
			}
			else
			{
				chgspd_pow = 1.0f;
			}	
		}
	}
	else
	if (stricmp(command,"setPower")==0)
	{
		if (NumParams>=2)
		{
			char str[64];

			needed_pow = 0.0f;
			strcpy(str,params[0]);
			sscanf (str, "%f", &needed_pow);

			chgspd_pow = 0.0f;
			strcpy(str,params[1]);
			sscanf (str, "%f", &chgspd_pow);

			if (chgspd_pow>0.0f)
			{
				chgspd_pow = fabs(needed_pow-power)/chgspd_pow;
			}
			else
			{
				chgspd_pow = 1.0f;
			}		
		}
	}
	else	
	{	
		TNode::Command(command,NumParams,params);
	}*/
}