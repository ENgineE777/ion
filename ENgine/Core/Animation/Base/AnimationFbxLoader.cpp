
#ifdef FBX

#include "AnimationFbxLoader.h"
#include "Core/Core.h"

KFbxSdkManager* AnimationFbxLoader::sdkManager = null;
KFbxScene* AnimationFbxLoader::scene = null;
float AnimationFbxLoader::grab_time = 0.0f;

void ConvertMatrixAnim(Matrix& mat, KFbxXMatrix& fbx_mat)
{
	double* ptr = (double*)fbx_mat;
	for (int i=0; i<16;i++) mat.matrix[i] = ptr[i];
}

void AnimationFbxLoader::CountBones(KFbxNode* pNode, int& num_bones)
{
	KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute && lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eSKELETON)
	{
		KFbxSkeleton* lSkeleton = (KFbxSkeleton*) pNode->GetNodeAttribute();

		if (lSkeleton->GetSkeletonType() == KFbxSkeleton::eLIMB_NODE)
		{
			num_bones++;
		}
	}

	const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
		CountBones(pNode->GetChild(lChildIndex), num_bones);
    }
}

void AnimationFbxLoader::ExtractBones(Mesh::Animation* res, KFbxNode* pNode, int& cur_bone)
{
	KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute && lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eSKELETON)
	{
		KFbxSkeleton* lSkeleton = (KFbxSkeleton*) pNode->GetNodeAttribute();

		if (lSkeleton->GetSkeletonType() == KFbxSkeleton::eLIMB_NODE)
		{
			Mesh::BoneAnimTrack* bone = &res->bones[cur_bone];

			String::Copy(bone->name, 64, pNode->GetName());
			bone->parent = -1;

			if (pNode->GetParent() &&
				pNode->GetParent()->GetNodeAttribute() &&
				pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eSKELETON)
			{
				const char* parent = pNode->GetParent()->GetName();

				for (int i=0; i<cur_bone; i++)
				{
					if (String::IsEqual(res->bones[i].name, parent))
					{
						bone->parent = i;
					}
				}
			}

			cur_bone++;
		}
	}

	const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
		ExtractBones(res, pNode->GetChild(lChildIndex), cur_bone);
    }
}

void AnimationFbxLoader::ExtractBonesTrack(Mesh::Animation* res, KFbxNode* pNode, KTime& time, int& cur_bone, int cur_key)
{
	KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute && lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eSKELETON)
	{
		KFbxSkeleton* lSkeleton = (KFbxSkeleton*) pNode->GetNodeAttribute();

		if (lSkeleton->GetSkeletonType() == KFbxSkeleton::eLIMB_NODE)
		{
			int index = -1;
			const char* bone = pNode->GetName();

			for (int i=0; i<res->num_bones;i++)
			{
				if (String::IsEqual(res->bones[i].name, bone))
				{
					index = i;
				}
			}

			if (index != -1)
			{
				Matrix global;	
				ConvertMatrixAnim(global, pNode->EvaluateGlobalTransform(time));
				
				Matrix local = global;

				/*int parent_index = res->bones[cur_bone].parent;

				if (parent_index != -1)
				{
					global.Normalize();

					Matrix parent;
					res->bones[parent_index].keys[cur_key].rot.GetMatrix(parent);
					parent.Scale(res->bones[parent_index].keys[cur_key].scale);
					parent.Pos() = res->bones[parent_index].keys[cur_key].pos;

					Matrix parent_inv = parent;
					parent_inv.InverseComplette();

					local = global * parent_inv;
				}*/

				Mesh::BoneKey* key = &res->bones[cur_bone].keys[cur_key];

				key->time = grab_time;
				key->scale = local.GetScale();
				key->pos = local.Pos();

				local.Normalize();
				key->rot.Set(local);
			}

			cur_bone++;
		}
	}

	const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
		ExtractBonesTrack(res, pNode->GetChild(lChildIndex), time, cur_bone, cur_key);
    }
}

bool AnimationFbxLoader::Load(Mesh::Animation* res, const char* name)
{		
    sdkManager = KFbxSdkManager::Create();

    if (!sdkManager)
    {
        core->TraceTo("Animation", "Unable to create the FBX SDK manager");
        return false;
    }

	KFbxIOSettings * ios = KFbxIOSettings::Create(sdkManager, IOSROOT );
	sdkManager->SetIOSettings(ios);

	KString lPath = KFbxGetApplicationDirectory();
	KString lExtension = "dll";
	sdkManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

    scene = KFbxScene::Create(sdkManager,"");

	int lFileFormat = -1;

	KFbxImporter* lImporter = KFbxImporter::Create(sdkManager,"");

	if (!sdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(name, lFileFormat) )
    {
        lFileFormat = sdkManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );
    }

	bool ret = lImporter->Initialize(name, lFileFormat);

	if (ret)
	{
		if (lImporter->Import(scene) == true)
        {       
            KFbxAxisSystem SceneAxisSystem = scene->GetGlobalSettings().GetAxisSystem();
			KFbxAxisSystem OurAxisSystem(KFbxAxisSystem::XAxis, KFbxAxisSystem::ParityEven, KFbxAxisSystem::LeftHanded);
            
			if( SceneAxisSystem != OurAxisSystem )
            {
                //OurAxisSystem.ConvertScene(scene);
            }

            KFbxSystemUnit SceneSystemUnit = scene->GetGlobalSettings().GetSystemUnit();
            if (SceneSystemUnit.GetScaleFactor() != 1.0)
            {               
               // KFbxSystemUnit::cm.ConvertScene(scene);
            }

			KArrayTemplate<KString*> animStackNameArray;
			scene->FillAnimStackNameArray(animStackNameArray);

			CountBones(scene->GetRootNode(), res->num_bones);

			if (res->num_bones)
			{
				res->bones = new Mesh::BoneAnimTrack[res->num_bones];
				int index = 0;
				ExtractBones(res, scene->GetRootNode(), index);
			}

			if (animStackNameArray.GetCount())
			{
				KFbxAnimStack * lCurrentAnimationStack = scene->FindMember(FBX_TYPE(KFbxAnimStack), animStackNameArray[0]->Buffer());
   
				if (lCurrentAnimationStack && res->num_bones)
				{
					KFbxAnimLayer* currentAnimLayer = lCurrentAnimationStack->GetMember(FBX_TYPE(KFbxAnimLayer), 0);
					scene->GetEvaluator()->SetContext(lCurrentAnimationStack);

					KFbxTakeInfo* lCurrentTakeInfo = scene->GetTakeInfo(*(animStackNameArray[0]));
				
					KTime mStart = 0.0f, mStop = 0.0f;

					if (lCurrentTakeInfo)
					{
						mStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
						mStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
					}
					else
					{
						KTimeSpan lTimeLineTimeSpan;
						scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

						mStart = lTimeLineTimeSpan.GetStart();
						mStop  = lTimeLineTimeSpan.GetStop();
					}

					KTime currentTime = mStart;

					KTime frameTime;
					frameTime.SetMilliSeconds(50);

					res->time_lenght = (mStop.GetMilliSeconds() - mStart.GetMilliSeconds()) * 0.001f;

					int num_keys = (int)(res->time_lenght / 0.05f) + 2;

					for (int i=0;i<res->num_bones;i++)
					{
						res->bones[i].num_keys = num_keys;
						res->bones[i].keys = new Mesh::BoneKey[num_keys];
					}

					int cur_key = 0;

					grab_time = 0.0f;

					while (currentTime<mStop + frameTime)
					{
						int index = 0;

						KTime tm = currentTime;
						tm = currentTime;
						if (tm>mStop)
						{
							tm = mStop;
						}

						ExtractBonesTrack(res, scene->GetRootNode(), currentTime, index, cur_key);
						currentTime += frameTime;
						cur_key++;
						grab_time += 0.05f;
					}
				}
			}
        }
	}

	lImporter->Destroy();
    lImporter = NULL;

	if (sdkManager) sdkManager->Destroy();
    sdkManager = NULL;

	return ret;
}

#endif