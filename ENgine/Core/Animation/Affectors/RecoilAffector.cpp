
#include "RecoilAffector.h"

void RecoilAffector::Update(float dt)
{
	for (int i=0;i<(int)bones.size();i++)
	{
		if (bones[i].curTime>-1.0f)
		{
			bones[i].curTime -= dt * bones[i].speed;

			if (bones[i].curTime<-1.0f)
			{
				bones[i].curTime = -2.0f;
								
				/*D3DXVECTOR3 offset = D3DXVECTOR3(NxMath::rand(-bones[i].strenght, bones[i].strenght),
												 NxMath::rand(-bones[i].strenght, bones[i].strenght),1.0f);

				D3DXMATRIX mt;

				D3DXMatrixLookAtLH( &mt, &D3DXVECTOR3(0,0,0), &offset, &D3DXVECTOR3(0,1.0f,0));
				D3DXMatrixInverse(&mt, NULL, &mt);

				D3DXQuaternionRotationMatrix( &bones[i].rot, &mt);

				bones[i].curTime = 1.0f;*/
			}
		}		
	}
}

void RecoilAffector::TransformBone(int index, Matrix* parentMat, Matrix* curMat)
{
	for (int i=0;i<(int)bones.size();i++)
	{
		if (bones[i].index == index && bones[i].curTime>-1.0f)
		{								
			Quaternion rot;
			rot.SLerp( bones[i].rot, Quaternion(0.0f,0.0f,0.0f,1.0f), fabs(bones[i].curTime));			

			Matrix mat;
			rot.GetMatrix(mat);			
			*curMat = mat * (*curMat);
			
			return;
		}
	}
}

void RecoilAffector::RecoilBone(int index, float time, float strenght, Vector* offset)
{
	int bn_index = -1;

	for (int i=0;i<(int)bones.size();i++)
	{
		if (bones[i].index == index)
		{
			bn_index = i;
			break;
		}
	}

	if (bn_index<0)
	{
		bones.push_back(boneMat());
		bn_index = (int)bones.size()-1;		
		bones[bn_index].index = index;
	}
	else
	{
		if (bones[bn_index].curTime>-1.0f) return;
	}

	bones[bn_index].speed = 2.0f / time;

	bones[bn_index].curTime = 1.0f;
	bones[bn_index].strenght = strenght;
	Vector delta;
	
	if (!offset)
	{
		///delta = IVector(NxMath::rand(-strenght, strenght),
		//				NxMath::rand(-strenght, strenght),1.0f);
	}
	else
	{
		delta = *offset;
	}

	Matrix mt;
	
	mt.BuildView( Vector(0,0,0), delta, Vector(0,1.0f,0));
	mt.Inverse();

	bones[bn_index].rot.Set( mt );	
}

/*
void RagDollAffector::UnregisterBone(int index)
{
	for (int i=0;i<(int)bones.size();i++)
	{
		if (bones[i].index == index)
		{
			for (int j=0;j<(int)bones.size()-1;j++)			
			{
				bones[j] = bones[j+1];
			}

			bones.pop_back();			

			return;
		}
	}
}*/