
#include "WaveAffector.h"
#include "Core/Animation/IAnimationInstance.h"

#ifdef ANIM_AFFECT_PROP

AFFECTOR_DECL(WaveAffector, "Waver", 0)

void WaveAffector::Init()
{
	PROP_REG( sStartBone )
	PROP_REG( sEndBone )
	PROP_REG( fTideHeight )
	PROP_REG( fTideAmp )
	PROP_REG( fTideSpeed )		

	inited = false;
}


void WaveAffector::ReInit()
{	
	Init(sStartBone.c_str(), sEndBone.c_str());
}

bool WaveAffector::Init(const char* start_bone, const char* end_bone)
{		
	offset = 0;
	map.clear();

	if (!Owner()) return false;

	int start = Owner()->GetBoneIndex(start_bone);
	int end = Owner()->GetBoneIndex(end_bone);

	if (start<0 || end<0) return false;

	map.push_back(Data(end));

	int next = end;

	do
	{
		next = Owner()->GetBoneParentIndex(next);

		if (next == -1)
		{
			inited = true;
			return false;
		}

		map.push_back(Data(next));
	}		
	while (next != start);	

	inited = true;

	return true;
}

float WaveAffector::Tider(float x)
{
	float amp = x / map[0].parent_lenght;
	amp *= amp;
	if (amp>1.0f) amp = 1.0f;

	return sinf(x * fTideAmp + offset) * fTideHeight * amp;
}

void WaveAffector::TransformBone(int index, IMatrix* parentMat, IMatrix* curMat)
{	
	if (!inited)
	{
		ReInit();
	}

	for (int i=0;i<(int)map.size();i++)
	{
		if (map[i].index == index)
		{		
			IMatrix curGl;

			if (parentMat)
			{
				curGl = (*curMat) * (*parentMat);	
			}
			else
			{
				curGl = (*curMat);	
			}

			IVector pos = curGl.Pos();

			if (map[i].parent_lenght<0)
			{
				if (parentMat && i<(int)map.size()-1)
				{						
					IVector parent_pos = parentMat->Pos();
					map[i].parent_lenght = (parent_pos - pos).Length();

					for (int j=i+1;j<(int)map.size();j++)
					{
						map[i].parent_lenght += map[j].parent_lenght;
					}
				}
				else
				{
					map[i].parent_lenght = 0;
					root = pos;
				}
			}				

			if (i<(int)map.size()-1)
			{	
				*curMat = curGl;

				{			
					float sgn = 1.0f;
					if (map[i].parent_lenght<0.0f) sgn = -1.0f;

					IVector parent_pos = IVector(root.x + Tider(map[i].parent_lenght),
												 pos.y,
												 root.z + map[i].parent_lenght);

					if (i>0)
					{
						pos = IVector(root.x + Tider(map[i-1].parent_lenght),
									  pos.y,
									  root.z + map[i-1].parent_lenght);
					}
					else
					{
						pos = IVector(root.x + Tider(map[i].parent_lenght + 0.5f*sgn),
									  pos.y,
									  root.z + map[i].parent_lenght + 0.05f*sgn);
					}

					IVector dir = pos - parent_pos;
					dir.Normalize();

					curMat->m[0][0] = dir.x;
					curMat->m[0][1] = dir.y;
					curMat->m[0][2] = dir.z;

					curMat->m[1][0] = 0.0f;
					curMat->m[1][1] = 1.0f;
					curMat->m[1][2] = 0.0f;						

					curMat->m[2][0] = -dir.z * sgn;
					curMat->m[2][1] =  dir.y;
					curMat->m[2][2] =  dir.x * sgn;					
				}

				curMat->Pos() = IVector(root.x + Tider(map[i].parent_lenght),
									    pos.y,
									    root.z + map[i].parent_lenght);

				if (parentMat)
				{
					IMatrix invParent = *parentMat;
					invParent.Inverse();

					(*curMat) = (*curMat) * invParent;
				}
			}				

			return;
		}
	}
}

void WaveAffector::Update(float dt)
{
	offset += dt * fTideSpeed;
}

#endif