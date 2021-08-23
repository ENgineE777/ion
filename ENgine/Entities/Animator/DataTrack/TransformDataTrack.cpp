
#include "TransformDataTrack.h"
#include "TransformDataTrackWidgets.h"
#include "Core/Core.h"

TransformDataTrack::TransformDataTrack(TrackPlayer* own, const char* nm,float rad,bool orinet,
									   KeyFunc key_Func, 
									   ChangeKeyFunc changeKey_Func,  bool _grabCameraOnAddKey,  Vector _defaultPos)
										 : TemplDataTrack<TransformTrackPoint>(own, nm, key_Func, changeKey_Func)
{	
	radius = rad;
	orient_by_spline = orinet;
	curve_count = 0;
	m_grabCameraOnAddKey = _grabCameraOnAddKey;
	m_defaultPos = _defaultPos;

	target = false;

#ifdef EDITOR
	wigets = NULL;
	invertSightInGizmo = false;
#endif	
}

TransformDataTrack::~TransformDataTrack()
{
	points.clear();

	if ( curve_count )
	{
		delete[] curve;
		curve = 0;
		curve_count = 0;
	}
}


void TransformDataTrack::Load(IStreamLoader* stream, int num)
{
	points.resize(num);

	for (int i=0;i<num;i++)
	{		
		if (!stream->EnterBlock("TransformData")) break;

		stream->Read("x",points[i].pos.x);
		stream->Read("y",points[i].pos.y);
		stream->Read("z",points[i].pos.z);

		stream->Read("qx",points[i].rot.x);
		stream->Read("qy",points[i].rot.y);
		stream->Read("qz",points[i].rot.z);
		stream->Read("qw",points[i].rot.w);

		stream->Read("time",points[i].time);

		points[i].needBlend = true;
		stream->Read("blend",points[i].needBlend);
		
		stream->LeaveBlock();
	}

	Prepare();
}

void TransformDataTrack::Save(IStreamSaver* stream)
{	
	for (int i=0;i<points.size();i++)
	{		
		stream->MarkBeginBlock("TransformData");

		stream->Write("x",points[i].pos.x);
		stream->Write("y",points[i].pos.y);
		stream->Write("z",points[i].pos.z);

		stream->Write("qx",points[i].rot.x);
		stream->Write("qy",points[i].rot.y);
		stream->Write("qz",points[i].rot.z);
		stream->Write("qw",points[i].rot.w);

		stream->Write("time",points[i].time);
		stream->Write("blend",points[i].needBlend);
		
		stream->MarkEndBlock("TransformData");
	}
}

void TransformDataTrack::BezierFunc(Vector& p1, Vector& p2,Vector& p3,Vector& p4, Vector& pos, float t)
{
	float s=1-t;
	float t2=t*t;
	float t3=t2*t;	

	pos = ((p1*s+3*t*p2)*s+3*t2*p3)*s+t3*p4;	
}

void TransformDataTrack::CubicFunc(int index, Vector& pos, float s)
{			
	pos = ((points[index].d * s + points[index].c) * s + points[index].b) * s + points[index].a;		
}

void TransformDataTrack::CubicFuncAxeX(int index, Vector& pos, float s)
{			
	pos = ((points[index].dX * s + points[index].cX) * s + points[index].bX) * s + points[index].aX;		
}

void TransformDataTrack::CubicFuncQuat(int index, Quaternion& quat, float s)
{			
	quat.x = ((points[index].dQ.x * s + points[index].cQ.x) * s + points[index].bQ.x) * s + points[index].aQ.x;		
	quat.y = ((points[index].dQ.y * s + points[index].cQ.y) * s + points[index].bQ.y) * s + points[index].aQ.y;		
	quat.z = ((points[index].dQ.z * s + points[index].cQ.z) * s + points[index].bQ.z) * s + points[index].aQ.z;		
	quat.w = ((points[index].dQ.w * s + points[index].cQ.w) * s + points[index].bQ.w) * s + points[index].aQ.w;		

}

void TransformDataTrack::GenerateKoef(int start_index, int end_index)
{		
	int n = end_index - start_index;

	if (n<1) return;

	Vector* gamma = new Vector[n + 1];
	Vector* delta = new Vector[n + 1];
	Vector* D = new Vector[n + 1];

	int i;
	
	//this builds the coefficients of the left matrix	
	gamma[0].x = 1.0f / 2.0f;
	gamma[0].y = 1.0f / 2.0f;
	gamma[0].z = 1.0f / 2.0f;

	for (i = 1; i < n; i++)
	{		
		gamma[i].x = 1.0f / (4.0f - gamma[i - 1].x);
		gamma[i].y = 1.0f / (4.0f - gamma[i - 1].y);
		gamma[i].z = 1.0f / (4.0f - gamma[i - 1].z);
	}
	
	gamma[n].x = 1.0f / (2.0f - gamma[n - 1].x);
	gamma[n].y = 1.0f / (2.0f - gamma[n - 1].y);
	gamma[n].z = 1.0f / (2.0f - gamma[n - 1].z);            
	
	delta[0].x = 3.0f * (points[start_index+1].dir.x - points[start_index].dir.x) * gamma[0].x;
	delta[0].y = 3.0f * (points[start_index+1].dir.y - points[start_index].dir.y) * gamma[0].y;
	delta[0].z = 3.0f * (points[start_index+1].dir.z - points[start_index].dir.z) * gamma[0].z;


	for (i = 1; i < n; i++)
	{                		
		delta[i].x = (3.0f * (points[start_index + i + 1].dir.x - points[start_index + i - 1].dir.x) - delta[i - 1].x) * gamma[i].x;
		delta[i].y = (3.0f * (points[start_index + i + 1].dir.y - points[start_index + i - 1].dir.y) - delta[i - 1].y) * gamma[i].y;
		delta[i].z = (3.0f * (points[start_index + i + 1].dir.z - points[start_index + i - 1].dir.z) - delta[i - 1].z) * gamma[i].z;                
	}
	
	delta[n].x = (3.0f * (points[start_index + n].dir.x - points[start_index + n - 1].dir.x) - delta[n - 1].x) * gamma[n].x;
	delta[n].y = (3.0f * (points[start_index + n].dir.y - points[start_index + n - 1].dir.y) - delta[n - 1].y) * gamma[n].y;
	delta[n].z = (3.0f * (points[start_index + n].dir.z - points[start_index + n - 1].dir.z) - delta[n - 1].z) * gamma[n].z;                

	D[n] = Vector(delta[n].x,delta[n].y,delta[n].z);

	for (i = n - 1; i >= 0; i--)
	{		
		D[i].x = delta[i].x - gamma[i].x * D[i + 1].x;
		D[i].y = delta[i].y - gamma[i].y * D[i + 1].y;
		D[i].z = delta[i].z - gamma[i].z * D[i + 1].z;
	}

	// now compute the coefficients of the cubics
	//Cubic[] C = new Cubic[n];

	for (i = 0; i < n; i++)
	{
		points[start_index + i].a = Vector(points[start_index + i].dir.x, points[start_index + i].dir.y, points[start_index + i].dir.z);
		points[start_index + i].b = Vector(D[i].x, D[i].y, D[i].z);
		points[start_index + i].c = Vector(3 * (points[start_index + i + 1].dir.x - points[start_index + i].dir.x) - 2 * D[i].x - D[i + 1].x,
										   3 * (points[start_index + i + 1].dir.y - points[start_index + i].dir.y) - 2 * D[i].y - D[i + 1].y,
										   3 * (points[start_index + i + 1].dir.z - points[start_index + i].dir.z) - 2 * D[i].z - D[i + 1].z);

		points[start_index + i].d = Vector(2 * (points[start_index + i].dir.x - points[start_index + i+1].dir.x) + D[i].x + D[i + 1].x,
										   2 * (points[start_index + i].dir.y - points[start_index + i+1].dir.y) + D[i].y + D[i + 1].y,
										   2 * (points[start_index + i].dir.z - points[start_index + i+1].dir.z) + D[i].z + D[i + 1].z);                
	}

	delete[] gamma;
	delete[] delta;
	delete[] D;		
}

void TransformDataTrack::GenerateKoefAxeX(int start_index, int end_index)
{		
	int n = end_index - start_index;

	if (n<1) return;

	Vector* gamma = new Vector[n + 1];
	Vector* delta = new Vector[n + 1];
	Vector* D = new Vector[n + 1];

	int i;

	//this builds the coefficients of the left matrix	
	gamma[0].x = 1.0f / 2.0f;
	gamma[0].y = 1.0f / 2.0f;
	gamma[0].z = 1.0f / 2.0f;

	for (i = 1; i < n; i++)
	{		
		gamma[i].x = 1.0f / (4.0f - gamma[i - 1].x);
		gamma[i].y = 1.0f / (4.0f - gamma[i - 1].y);
		gamma[i].z = 1.0f / (4.0f - gamma[i - 1].z);
	}

	gamma[n].x = 1.0f / (2.0f - gamma[n - 1].x);
	gamma[n].y = 1.0f / (2.0f - gamma[n - 1].y);
	gamma[n].z = 1.0f / (2.0f - gamma[n - 1].z);            

	delta[0].x = 3.0f * (points[start_index+1].dirAxeX.x - points[start_index].dirAxeX.x) * gamma[0].x;
	delta[0].y = 3.0f * (points[start_index+1].dirAxeX.y - points[start_index].dirAxeX.y) * gamma[0].y;
	delta[0].z = 3.0f * (points[start_index+1].dirAxeX.z - points[start_index].dirAxeX.z) * gamma[0].z;


	for (i = 1; i < n; i++)
	{                		
		delta[i].x = (3.0f * (points[start_index + i + 1].dirAxeX.x - points[start_index + i - 1].dirAxeX.x) - delta[i - 1].x) * gamma[i].x;
		delta[i].y = (3.0f * (points[start_index + i + 1].dirAxeX.y - points[start_index + i - 1].dirAxeX.y) - delta[i - 1].y) * gamma[i].y;
		delta[i].z = (3.0f * (points[start_index + i + 1].dirAxeX.z - points[start_index + i - 1].dirAxeX.z) - delta[i - 1].z) * gamma[i].z;                
	}

	delta[n].x = (3.0f * (points[start_index + n].dirAxeX.x - points[start_index + n - 1].dirAxeX.x) - delta[n - 1].x) * gamma[n].x;
	delta[n].y = (3.0f * (points[start_index + n].dirAxeX.y - points[start_index + n - 1].dirAxeX.y) - delta[n - 1].y) * gamma[n].y;
	delta[n].z = (3.0f * (points[start_index + n].dirAxeX.z - points[start_index + n - 1].dirAxeX.z) - delta[n - 1].z) * gamma[n].z;                

	D[n] = Vector(delta[n].x,delta[n].y,delta[n].z);

	for (i = n - 1; i >= 0; i--)
	{		
		D[i].x = delta[i].x - gamma[i].x * D[i + 1].x;
		D[i].y = delta[i].y - gamma[i].y * D[i + 1].y;
		D[i].z = delta[i].z - gamma[i].z * D[i + 1].z;
	}

	// now compute the coefficients of the cubics
	//Cubic[] C = new Cubic[n];

	for (i = 0; i < n; i++)
	{
		points[start_index + i].aX = Vector(points[start_index + i].dirAxeX.x, points[start_index + i].dirAxeX.y, points[start_index + i].dirAxeX.z);
		points[start_index + i].bX = Vector(D[i].x, D[i].y, D[i].z);
		points[start_index + i].cX = Vector(3 * (points[start_index + i + 1].dirAxeX.x - points[start_index + i].dirAxeX.x) - 2 * D[i].x - D[i + 1].x,
										    3 * (points[start_index + i + 1].dirAxeX.y - points[start_index + i].dirAxeX.y) - 2 * D[i].y - D[i + 1].y,
										    3 * (points[start_index + i + 1].dirAxeX.z - points[start_index + i].dirAxeX.z) - 2 * D[i].z - D[i + 1].z);

		points[start_index + i].dX = Vector(2 * (points[start_index + i].dirAxeX.x - points[start_index + i+1].dirAxeX.x) + D[i].x + D[i + 1].x,
											2 * (points[start_index + i].dirAxeX.y - points[start_index + i+1].dirAxeX.y) + D[i].y + D[i + 1].y,
											2 * (points[start_index + i].dirAxeX.z - points[start_index + i+1].dirAxeX.z) + D[i].z + D[i + 1].z);                
	}

	delete[] gamma;
	delete[] delta;
	delete[] D;		
}

void TransformDataTrack::GenerateKoefQuat(int start_index, int end_index)
{		
	int n = end_index - start_index;

	if (n<1) return;

	Quaternion* gamma = new Quaternion[n + 1];
	Quaternion* delta = new Quaternion[n + 1];
	Quaternion* D = new Quaternion[n + 1];

	int i;

	//this builds the coefficients of the left matrix	
	gamma[0].x = 1.0f / 2.0f;
	gamma[0].y = 1.0f / 2.0f;
	gamma[0].z = 1.0f / 2.0f;
	gamma[0].w = 1.0f / 2.0f;

	for (i = 1; i < n; i++)
	{		
		gamma[i].x = 1.0f / (4.0f - gamma[i - 1].x);
		gamma[i].y = 1.0f / (4.0f - gamma[i - 1].y);
		gamma[i].z = 1.0f / (4.0f - gamma[i - 1].z);
		gamma[i].w = 1.0f / (4.0f - gamma[i - 1].w);
	}

	gamma[n].x = 1.0f / (2.0f - gamma[n - 1].x);
	gamma[n].y = 1.0f / (2.0f - gamma[n - 1].y);
	gamma[n].z = 1.0f / (2.0f - gamma[n - 1].z);            
	gamma[n].w = 1.0f / (2.0f - gamma[n - 1].w);            

	delta[0].x = 3.0f * (points[start_index+1].rot.x - points[start_index].rot.x) * gamma[0].x;
	delta[0].y = 3.0f * (points[start_index+1].rot.y - points[start_index].rot.y) * gamma[0].y;
	delta[0].z = 3.0f * (points[start_index+1].rot.z - points[start_index].rot.z) * gamma[0].z;
	delta[0].w = 3.0f * (points[start_index+1].rot.w - points[start_index].rot.w) * gamma[0].w;


	for (i = 1; i < n; i++)
	{                		
		delta[i].x = (3.0f * (points[start_index + i + 1].rot.x - points[start_index + i - 1].rot.x) - delta[i - 1].x) * gamma[i].x;
		delta[i].y = (3.0f * (points[start_index + i + 1].rot.y - points[start_index + i - 1].rot.y) - delta[i - 1].y) * gamma[i].y;
		delta[i].z = (3.0f * (points[start_index + i + 1].rot.z - points[start_index + i - 1].rot.z) - delta[i - 1].z) * gamma[i].z;                
		delta[i].w = (3.0f * (points[start_index + i + 1].rot.w - points[start_index + i - 1].rot.w) - delta[i - 1].w) * gamma[i].w;                
	}

	delta[n].x = (3.0f * (points[start_index + n].rot.x - points[start_index + n - 1].rot.x) - delta[n - 1].x) * gamma[n].x;
	delta[n].y = (3.0f * (points[start_index + n].rot.y - points[start_index + n - 1].rot.y) - delta[n - 1].y) * gamma[n].y;
	delta[n].z = (3.0f * (points[start_index + n].rot.z - points[start_index + n - 1].rot.z) - delta[n - 1].z) * gamma[n].z;                
	delta[n].w = (3.0f * (points[start_index + n].rot.w - points[start_index + n - 1].rot.w) - delta[n - 1].w) * gamma[n].w;                

	D[n] = Quaternion(delta[n].x,delta[n].y,delta[n].z, delta[n].w);

	for (i = n - 1; i >= 0; i--)
	{		
		D[i].x = delta[i].x - gamma[i].x * D[i + 1].x;
		D[i].y = delta[i].y - gamma[i].y * D[i + 1].y;
		D[i].z = delta[i].z - gamma[i].z * D[i + 1].z;
		D[i].w = delta[i].w - gamma[i].w * D[i + 1].w;
	}

	// now compute the coefficients of the cubics
	//Cubic[] C = new Cubic[n];

	for (i = 0; i < n; i++)
	{
		points[start_index + i].aQ = Quaternion(points[start_index + i].rot.x, points[start_index + i].rot.y, points[start_index + i].rot.z, points[start_index + i].rot.w);
		points[start_index + i].bQ = Quaternion(D[i].x, D[i].y, D[i].z, D[i].w);
		points[start_index + i].cQ = Quaternion(3 * (points[start_index + i + 1].rot.x - points[start_index + i].rot.x) - 2 * D[i].x - D[i + 1].x,
												3 * (points[start_index + i + 1].rot.y - points[start_index + i].rot.y) - 2 * D[i].y - D[i + 1].y,
												3 * (points[start_index + i + 1].rot.z - points[start_index + i].rot.z) - 2 * D[i].z - D[i + 1].z,
												3 * (points[start_index + i + 1].rot.w - points[start_index + i].rot.w) - 2 * D[i].w - D[i + 1].w);

		points[start_index + i].dQ = Quaternion(2 * (points[start_index + i].rot.x - points[start_index + i+1].rot.x) + D[i].x + D[i + 1].x,
												2 * (points[start_index + i].rot.y - points[start_index + i+1].rot.y) + D[i].y + D[i + 1].y,
												2 * (points[start_index + i].rot.z - points[start_index + i+1].rot.z) + D[i].z + D[i + 1].z,
												2 * (points[start_index + i].rot.w - points[start_index + i+1].rot.w) + D[i].w + D[i + 1].w);                
	}

	delete[] gamma;
	delete[] delta;
	delete[] D;		
}

void TransformDataTrack::Prepare()
{		
	for (int i = 0; i < (int)points.size(); i++)
	{                		
		Matrix mat;
		points[i].rot.GetMatrix(mat);						

		points[i].dir.x = mat.Vz().x;
		points[i].dir.y = mat.Vz().y;
		points[i].dir.z = mat.Vz().z;

		points[i].dirAxeX = mat.Vx();
		
		
		points[i].b_pt1 = points[i].b_pt2 = points[i].pos;
	}	

	int start_index = 0;
	int index = 0;	

	while (index<points.size())
	{
		if (!points[index].needBlend || index == points.size() - 1)
		{
			GenerateKoef(start_index, index);
			GenerateKoefAxeX(start_index, index);
			GenerateKoefQuat(start_index, index);

			if (index - start_index == 1)
			{
				Vector dir = points[1].pos - points[0].pos;
				float dst = dir.Normalize();
		
				points[0].b_pt1 = points[0].pos + dir * dst * 0.33f;				
				points[0].b_pt2 = points[0].pos + dir * dst * 0.66f;		

				dir = points[1].dir - points[0].dir;
				dst = dir.Normalize();		
			}
			else
			{
				for (int i = start_index; i < index - 1; i++)
				{
					Vector dir = points[i + 1].pos - points[i].pos;
					float dst = dir.Normalize();

					if (i < (int)points.size() - 2)
					{
						Vector tngt = points[i + 2].pos - points[i].pos;
						tngt.Normalize();
				
						points[i].b_pt2 = points[i + 1].pos - tngt * dst * 0.25f;				

						dir = points[i + 2].pos - points[i + 1].pos;				
						dst = dir.Normalize();
			
						points[i + 1].b_pt1 = points[i + 1].pos + tngt * dst * 0.25f;				

						if (i == 0)
						{
							dir = points[i].b_pt2 - points[i].pos;					
							dst = dir.Normalize();
					
							points[i].b_pt1 = points[i].pos + dir * dst * 0.5f;					
						}

						if (i == index - 2)
						{
							dir = points[i + 2].pos - points[i + 1].b_pt1;					
							dst = dir.Normalize();
					
							points[i + 1].b_pt2 = points[i + 1].b_pt1 + dir * dst * 0.5f;					
						}
					}			
				}               
			}

			start_index = index + 1;
		}
		
		index++;		
	}	

	if (points.size() > 1)
	{
		curve_count = (points.size() - 1) * 20 + 1;
		curve = new Vector[curve_count];
		curve[0] = points[0].pos;		

		for (int j = 0; j < (int)points.size() - 1; j++)
		{
			points[j].length = 0;		

			Vector prev_pt = points[j].pos;

			for (int i = 0; i < 20; i++)
			{
				float k = (float)(i+1) / 20.0f;

				BezierFunc(points[j].pos, points[j].b_pt1, points[j].b_pt2, points[j + 1].pos, curve[j * 20 + i + 1], k);

				prev_pt = curve[j * 20 + i + 1] - prev_pt;				

				points[j].length += prev_pt.Length();
				points[j].dst[i] = points[j].length;

				prev_pt = curve[j * 20 + i + 1];				
			}

			points[j].speed = points[j].length / (points[j+1].time - points[j].time);

			if (j==0)
			{
				points[j].prev_speed = points[j].speed;
			}
			else
			{
				points[j].prev_speed = points[j-1].speed;				
				if (points[j].prev_speed < 0.001f) points[j].prev_speed = points[j].speed;
			}                    
		}
	}
	else
	{
		curve = NULL;
	}		
}

void TransformDataTrack::CalcValue(int index, float blend)
{
	if (blend<-0.01f)
	{
		cur_val.pos = points[index].pos;
		cur_val.rot = points[index].rot;
	}
	else
	{		
		if ((points[index - 1].dir.x * points[index].dir.x +
			 points[index - 1].dir.y * points[index].dir.y +
			 points[index - 1].dir.z * points[index].dir.z) > 0.9f)
		{
			cur_val.rot.SLerp(points[index-1].rot, points[index].rot, blend);
		}
		else
		{
			Vector s_dir;
			CubicFunc(index - 1, s_dir, blend);

			Vector dirX;
			CubicFuncAxeX(index - 1, dirX, blend);

			//CubicFuncQuat(index - 1, cur_val.rot, blend);
			//cur_val.rot.Normalize();

			Vector up;
			up = s_dir ^ dirX;
			
			Matrix view;
			view.BuildView(Vector(0.0f), s_dir, up);
			view.Inverse();
			cur_val.rot.Set(view);
		}			

		float dst = 0.0f;

		int num = 50;
		static float lens[50 + 1];				
				
		float len = 0.0f;
		lens[0] = 0.0f;

		float weight = fabs(points[index-1].speed - points[index - 1].prev_speed);
		weight -= 3.0;
		if (weight<0.0f) weight = 0.0f;
		if (weight>10.0f) weight = 10.0f;
		weight = weight / 10.0f;

		for (int k=0; k<num; k++)
		{ 			
			float blnd = (float)(k+1)/(float)(num);
			//float blnd2 = 0.5f + (1.0f - cosf(blnd * PI)) * 0.5f;

			//blnd = lerp(blnd, blnd2, weight);

			float cur_v = fabs(points[index - 1].prev_speed + (points[index-1].speed - points[index - 1].prev_speed) * blnd);

			len += cur_v * 1.0f / (float)(num);
			lens[k+1] = len;
		}

		float tr = points[index - 1].length / len;

		for (int k = 0; k <= num; k++)
		{
			lens[k] *= tr;
		}

		int t = (int)(blend * (float)(num));
		float kb = (blend - t * 1.0f / (float)(num)) / (1.0f / (float)(num));

		dst = lens[t] + (lens[t+1] - lens[t]) * kb;						

		for (int sub_index = 0; sub_index < 20; sub_index++)
		{
			float k = points[index - 1].dst[sub_index];

			if (dst < k)
			{
				float k1 = 0;

				if (sub_index>0)
				{
					k1 = points[index - 1].dst[sub_index - 1];
				}

				blend = (dst - k1) / (k - k1);

				cur_val.pos.Lerp( curve[(index - 1) * 20 + sub_index],
								  curve[(index - 1) * 20 + sub_index + 1], blend);					

				if (orient_by_spline)
				{
					Matrix view;
					view.Vz() = curve[(index - 1) * 20 + sub_index] - curve[(index - 1) * 20 + sub_index + 1];
					view.Vz().Normalize();
					view.Vx() = view.Vz();
					view.Vx().Rotate_PI2_CW();
					view.Vy() = view.Vx() ^ view.Vz();								
					view.Vy().y = fabs(view.Vy().y);

					cur_val.rot.Set(view);
				}

				break;
			}                            
		}        		
	}		

	if (target)
	{
		cur_val.rot.GetMatrix(*target);
		target->Pos() = cur_val.pos;
	}
}

void TransformDataTrack::SetControlledValue(void* value)
{
	target = (Matrix*)value;
}

#ifdef EDITOR

void TransformDataTrack::SetDefaultKey(int index)
{
	if (m_grabCameraOnAddKey)
	{
		GrabCamera(index);
	}
	else
	{
		points[index].pos = m_defaultPos;
		points[index].rot = Quaternion(0.f, 0.f, 0.f, 1.f);

		Matrix mat;
		points[index].rot.GetMatrix(mat);
		points[index].pos += mat.Vz() * radius;
	}
}

int TransformDataTrack::AddKey(float time)
{
	int ret = TemplDataTrack::AddKey(time);
	Prepare();
	return ret;
}

void TransformDataTrack::DelKey(int index)
{
	TemplDataTrack::DelKey(index);
	Prepare();
}

void TransformDataTrack::InitControls(BaseWidget* parent)
{
	TemplDataTrack::InitControls(parent);
	wigets = new TransformDataTrackWidgets(this, 0, 35); 
	wigets->InitControls(parent);
}

void TransformDataTrack::StartEditKey(int key, Gizmo* gz)
{
	TemplDataTrack::StartEditKey(key, gz);
	
	if (edited_key!=-1)
	{
		wigets->StartEditKey();		
	}

	DataToGizmo();
}


void TransformDataTrack::StartEditMultiKeys(std::vector<int>& ks, Gizmo* gz)
{
	TemplDataTrack::StartEditMultiKeys(ks, gz);
	
	if (!m_EditedKeys.empty()) wigets->StartEditMultiKeys();

	DataToGizmoMultiSelect();
}

bool TransformDataTrack::UpdateWidgets()
{
	if (edited_key!=-1)
	{
		wigets->UpdateWidgets();
	}

	DebugDraw();

	return false;
}

void TransformDataTrack::StopEditMultiKeys()
{
	TemplDataTrack::StopEditMultiKeys();
	
	wigets->StopEditKey();

	if (gizmo) gizmo->enabled = false;

	Prepare();
}

void TransformDataTrack::SwicthKeyBlend()
{
	TemplDataTrack::SwicthKeyBlend();
	Prepare();
}

void TransformDataTrack::SetKeyBlend(bool _value)
{
	TemplDataTrack::SetKeyBlend(_value);
	Prepare();
}

void TransformDataTrack::DataToGizmo()
{
	if (edited_key != -1 && gizmo)
	{
		Matrix tr;
		points[edited_key].rot.GetMatrix(tr);
		tr.Pos() = points[edited_key].pos;
	
		gizmo->enabled = true;
		gizmo->transform = tr;
		
		//gizmo->invertSight = invertSightInGizmo;		
	}
}


void TransformDataTrack::DataToGizmoMultiSelect()
{
	if (gizmo)
	{
		/*Matrix tr;
		tr.Pos() = (Vector&)CWorld::instance()->getWorldCamera()->getPos();
		Quaternion quat =  (Quaternion&)CWorld::instance()->getWorldCamera()->getRot();
		quat.GetMatrix(tr);
		tr.Pos() = (Vector&)CWorld::instance()->getWorldCamera()->getPos();

		tr.Pos() += tr.Vz() * 5.f;
		gizmo->enabled = true;
		gizmo->transform = tr;
#ifdef EDITOR
		gizmo->invertSight = invertSightInGizmo;
#endif
		*/

	}
}
void TransformDataTrack::DataFromGizmo()
{
	if (edited_key != -1 && gizmo)
	{		
		if (edited_key!=-1)
		{
			if ((points[edited_key].pos - gizmo->transform.Pos()).Length()>0.0001f)
			{
				wigets->StartEditKey();
			}
		}

		points[edited_key].rot.Set(gizmo->transform);
		points[edited_key].pos = gizmo->transform.Pos();

		Prepare();
	}
	else if (!m_EditedKeys.empty())
	{
		/*dword size =  m_EditedKeys.size();
		for (dword i = 0; i < size; ++i)
		{
			points[m_EditedKeys[i]].pos += gizmo->relativeTransform.Pos();
			IQuaternion quat(gizmo->relativeTransform);
			points[m_EditedKeys[i]].rot *= quat;
		}

		Prepare();*/
	}
}

void TransformDataTrack::StopEditKey()
{
	if (gizmo) gizmo->enabled = false;
	TemplDataTrack::StopEditKey();

	if (wigets)	wigets->StopEditKey();
}

void TransformDataTrack::OnClose()
{
	TemplDataTrack::OnClose();
	delete wigets;
	wigets = 0;
}

void TransformDataTrack::DebugDraw()
{
	if (curve)
    {
		for (int i = 0; i < curve_count - 1; i++)
        {
			int index = (int)((float)i / 20.f);

			if (!points[index].needBlend) continue;

			en_core.Render()->Debug()->DrawLine(curve[ i ], COLOR_GREEN, curve[i+1], COLOR_GREEN);
			
        }
    }
    
	if (!points.size())
		return;



	Matrix view;
	en_core.Render()->Display()->GetView( view );

	Matrix view_proj;
	en_core.Render()->Display()->GetProj( view_proj );
	view_proj = view * view_proj;

	bool blend = true;
	for (int i = 0; i < points.size(); i++)
    {
		//if ((((Vector&)CWorld::instance()->getWorldCamera()->getPos()) - points[i].pos).Length2() < 25.f)
		//		continue;

		dword color = 0xff00ffff;
        
		if (i == edited_key) color = 0xffff00ff;
	          
		// draw sight arrow
		Matrix transform;
		points[i].rot.GetMatrix(transform);
		transform.Pos() = points[i].pos;

		float z=points[i].pos.x*view_proj._13+points[i].pos.y*view_proj._23+points[i].pos.z*view_proj._33+view_proj._43;    

		float scale = 0.01f * (1.0f + z);
		scale = fabs(scale);

		en_core.Render()->Debug()->DrawSolidBox(scale, transform, color);

		Vector pos = points[i].pos;

		Vector dir = transform.Vz();
		
		if (invertSightInGizmo)
			dir *= -1.f;
		
		dir += pos;

		en_core.Render()->Debug()->DrawLine(pos, COLOR_WHITE, dir, COLOR_WHITE);

		Vector dirx = transform.Vx()*0.3f;
		Vector dirxInvert = dirx * (-1.f);
		dirx += pos;
		dirxInvert += pos;

		en_core.Render()->Debug()->DrawLine(pos, COLOR_WHITE, dirx, COLOR_WHITE);
		en_core.Render()->Debug()->DrawLine(pos, COLOR_WHITE, dirxInvert, COLOR_WHITE);

		en_core.Render()->Debug()->DrawLine(dir, COLOR_WHITE, dirxInvert, COLOR_WHITE);
		en_core.Render()->Debug()->DrawLine(dir, COLOR_WHITE, dirx, COLOR_WHITE);		

		if (!blend) 
		{
			blend = points[i].needBlend;
			continue;
		}

		blend = points[i].needBlend;
		
		if (i == 0) continue;

		/*if (wxGetApp().m_FullDebugDraw)
		{
			for (int j = 0; j < 20; j++)
			{
				float k = (float)j / 20;
				TransformTrackPoint temp = cur_val;
				CalcValue(i, k);
				Vector axeZ(0.f, 0.f, 1.f);
				cur_val.rot.Rotate(axeZ);
				axeZ += cur_val.pos;

				en_core.Render()->DrawLine(cur_val.pos, COLOR_WHITE, axeZ, COLOR_WHITE);				
				cur_val = temp;
			}
		}*/
		
	}
}

TransformTrackPoint* TransformDataTrack::GetPoint()
{
	if (edited_key != -1)
	{
		return &points[edited_key];
	}

	return NULL;
}

void TransformDataTrack::PasteFromBuffer()
{
	TemplDataTrack::PasteFromBuffer();
	Prepare();
}

void TransformDataTrack::CheckHotKeys()
{
	if (GetKeyState('G') < 0)
	{
		wigets->GrabKey();
	}
	else
	if (GetKeyState('H') < 0)
	{
		if (edited_key != -1)
		{
			points[edited_key].rot = Quaternion(0, 0, 0, 1);
			points[edited_key].pos = Vector(0, 0, 0);
			Prepare();
			DataToGizmo();
			wigets->StartEditKey();
		}
	}
}

void TransformDataTrack::UpdateValue(int type, float value)
{
	if (edited_key == -1)
	{
		dword size = m_EditedKeys.size();
		for (dword i = 0; i < size; ++i)
		{
			if (type == 0)
			{			
				points[m_EditedKeys[i]].pos.x += value;
			}
			else
			if (type == 1)
			{
				points[m_EditedKeys[i]].pos.y += value;
			}
			else
			if (type == 2)
			{
				points[m_EditedKeys[i]].pos.z += value;
			}
		}
	}
	else
	{
		if (type == 0)
		{			
			points[edited_key].pos.x = value;
		}
		else
		if (type == 1)
		{
			points[edited_key].pos.y = value;
		}
		else
		if (type == 2)
		{
			points[edited_key].pos.z = value;
		}

		DataToGizmo();
	}

	Prepare();
}

void TransformDataTrack::GrabCamera(int index)
{
	Matrix view;
	en_core.Render()->Display()->GetView(view);
	view.Inverse();

	points[index].pos = view.Pos();	
	points[index].rot.Set(view);	

	points[index].pos += view.Vz() * radius;
}

#endif

