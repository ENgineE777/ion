
#pragma once

#include "Math.h"

struct FrustumPlane
{
	Vector normal;
	float   dist;
};

inline void UpdateFrustum( FrustumPlane* frustum, const Matrix& view_proj)
{
	frustum[0].normal.x = view_proj._14 - view_proj._11; 
	frustum[0].normal.y = view_proj._24 - view_proj._21; 
	frustum[0].normal.z = view_proj._34 - view_proj._31; 
	frustum[0].dist = view_proj._44 - view_proj._41; 

	frustum[1].normal.x = view_proj._14 + view_proj._11; 
	frustum[1].normal.y = view_proj._24 + view_proj._21; 
	frustum[1].normal.z = view_proj._34 + view_proj._31; 
	frustum[1].dist = view_proj._44 + view_proj._41; 

	frustum[2].normal.x = view_proj._14 + view_proj._12; 
	frustum[2].normal.y = view_proj._24 + view_proj._22; 
	frustum[2].normal.z = view_proj._34 + view_proj._32; 
	frustum[2].dist = view_proj._44 + view_proj._42; 

	frustum[3].normal.x = view_proj._14 - view_proj._12; 
	frustum[3].normal.y = view_proj._24 - view_proj._22; 
	frustum[3].normal.z = view_proj._34 - view_proj._32; 
	frustum[3].dist = view_proj._44 - view_proj._42; 

	frustum[4].normal.x = view_proj._14 - view_proj._13; 
	frustum[4].normal.y = view_proj._24 - view_proj._23; 
	frustum[4].normal.z = view_proj._34 - view_proj._33; 
	frustum[4].dist = view_proj._44 - view_proj._43; 

	frustum[5].normal.x = view_proj._14 + view_proj._13; 
	frustum[5].normal.y = view_proj._24 + view_proj._23; 
	frustum[5].normal.z = view_proj._34 + view_proj._33; 
	frustum[5].dist = view_proj._44 + view_proj._43; 

	for (int i = 0;i<6;i++)
	{		
		frustum[i].dist *= 1.0f / frustum[i].normal.Normalize(); 
	}
}


inline bool SphereInFrustum( FrustumPlane* frustum, const Vector& pos, float radius )
{    
	float dot;

	for (int i=0;i<6;i++)
	{
		dot = frustum[i].normal | pos;

		if (dot + frustum[i].dist + radius<0)
		{
			return false;
		}
	}    

	return true;
}

inline bool BoxInFrustum( FrustumPlane* frustum, Vector* box )
{   	
	for (int i = 0; i < 6; i++)
	{
		int index;

		for (index = 0; index < 8; index++)
		{
			float dot = frustum[i].normal | box[index];

			if (0.0f < dot + frustum[i].dist) break;
		}

		if (8 <= index) return false;
	} 	

	return true;
}