

#ifdef FBX

#include "ModelFbxLoader.h"
#include "Core/Core.h"

KFbxSdkManager* ModelFbxLoader::sdkManager = null;
KFbxScene* ModelFbxLoader::scene = null;
char ModelFbxLoader::path[256];
ModelRes* ModelFbxLoader::res = null;

void ModelFbxLoader::ConvertMatrix(Matrix& mat, KFbxXMatrix& fbx_mat)
{
	double* ptr = (double*)fbx_mat;
	for (int i=0; i<16;i++) mat.matrix[i] = ptr[i];
}

// Get the matrix of the given pose
KFbxXMatrix ModelFbxLoader::GetPoseMatrix(KFbxPose* pPose, int pNodeIndex)
{
    KFbxXMatrix lPoseMatrix;
    KFbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

    memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

    return lPoseMatrix;
}

// Get the geometry offset to a node. It is never inherited by the children.
KFbxXMatrix ModelFbxLoader::GetGeometry(KFbxNode* pNode)
{
    const KFbxVector4 lT = pNode->GetGeometricTranslation(KFbxNode::eSOURCE_SET);
    const KFbxVector4 lR = pNode->GetGeometricRotation(KFbxNode::eSOURCE_SET);
    const KFbxVector4 lS = pNode->GetGeometricScaling(KFbxNode::eSOURCE_SET);

    return KFbxXMatrix(lT, lR, lS);
}

KFbxXMatrix ModelFbxLoader::GetGlobalPosition(KFbxNode* pNode, const KTime& pTime, 
											  KFbxPose* pPose, KFbxXMatrix* pParentGlobalPosition)
{
    KFbxXMatrix lGlobalPosition;
    bool        lPositionFound = false;

    if (pPose)
    {
        int lNodeIndex = pPose->Find(pNode);

        if (lNodeIndex > -1)
        {
            // The bind pose is always a global matrix.
            // If we have a rest pose, we need to check if it is
            // stored in global or local space.
            if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
            {
                lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
            }
            else
            {
                // We have a local matrix, we need to convert it to
                // a global space matrix.
                KFbxXMatrix lParentGlobalPosition;

                if (pParentGlobalPosition)
                {
                    lParentGlobalPosition = *pParentGlobalPosition;
                }
                else
                {
                    if (pNode->GetParent())
                    {
                        lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
                    }
                }

                KFbxXMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
                lGlobalPosition = lParentGlobalPosition * lLocalPosition;
            }

            lPositionFound = true;
        }
    }

    if (!lPositionFound)
    {
        // There is no pose entry for that node, get the current global position instead.

        // Ideally this would use parent global position and local position to compute the global position.
        // Unfortunately the equation 
        //    lGlobalPosition = pParentGlobalPosition * lLocalPosition
        // does not hold when inheritance type is other than "Parent" (RSrs).
        // To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
        lGlobalPosition = pNode->EvaluateGlobalTransform();
    }

    return lGlobalPosition;
}

//Compute the transform matrix that the cluster will transform the vertex.
void ModelFbxLoader::ComputeClusterDeformation(KFbxXMatrix& pGlobalPosition, KFbxMesh* pMesh,
											   KFbxCluster* pCluster, KFbxXMatrix& pVertexTransformMatrix,
											   KTime pTime, KFbxPose* pPose, int bone_index)
{
    KFbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

	KFbxXMatrix lReferenceGlobalInitPosition;
	KFbxXMatrix lReferenceGlobalCurrentPosition;
	KFbxXMatrix lAssociateGlobalInitPosition;
	KFbxXMatrix lAssociateGlobalCurrentPosition;
	KFbxXMatrix lClusterGlobalInitPosition;
	KFbxXMatrix lClusterGlobalCurrentPosition;

	KFbxXMatrix lReferenceGeometry;
	KFbxXMatrix lAssociateGeometry;
	KFbxXMatrix lClusterGeometry;

	KFbxXMatrix lClusterRelativeInitPosition;
	KFbxXMatrix lClusterRelativeCurrentPositionInverse;
	
	if (lClusterMode == KFbxLink::eADDITIVE && pCluster->GetAssociateModel())
	{
		pCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
		// Geometric transform of the model
		lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
		lAssociateGlobalInitPosition *= lAssociateGeometry;
		lAssociateGlobalCurrentPosition = GetGlobalPosition(pCluster->GetAssociateModel(), pTime, pPose);

		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;
		lReferenceGlobalCurrentPosition = pGlobalPosition;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		// Multiply lClusterGlobalInitPosition by Geometric Transformation
		lClusterGeometry = GetGeometry(pCluster->GetLink());
		lClusterGlobalInitPosition *= lClusterGeometry;
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the shift of the link relative to the reference.
		//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
		pVertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
			lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
	}
	else
	{
		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		lReferenceGlobalCurrentPosition = pGlobalPosition;
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the initial position of the link relative to the reference.
		lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

		// Compute the current position of the link relative to the reference.
		lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

		// Compute the shift of the link relative to the reference.
		pVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;

		/*Bone* bone = &skeleton[bone_index];

		ConvertMatrix(bone->lReferenceGlobalInitPosition, lReferenceGlobalInitPosition);
		ConvertMatrix(bone->lReferenceGlobalCurrentPosition, lReferenceGlobalCurrentPosition);
		ConvertMatrix(bone->lAssociateGlobalInitPosition, lAssociateGlobalInitPosition);
		ConvertMatrix(bone->lAssociateGlobalCurrentPosition, lAssociateGlobalCurrentPosition);
		ConvertMatrix(bone->lClusterGlobalInitPosition, lClusterGlobalInitPosition);
		ConvertMatrix(bone->lClusterGlobalCurrentPosition, lClusterGlobalCurrentPosition);

		ConvertMatrix(bone->lReferenceGeometry, lReferenceGeometry);
		ConvertMatrix(bone->lAssociateGeometry, lAssociateGeometry);
		ConvertMatrix(bone->lClusterGeometry, lClusterGeometry);

		ConvertMatrix(bone->lClusterRelativeInitPosition, lClusterRelativeInitPosition);
		ConvertMatrix(bone->lClusterRelativeCurrentPositionInverse, lClusterRelativeCurrentPositionInverse);

		ConvertMatrix(bone->pVertexTransformMatrix, pVertexTransformMatrix);*/
	}
}

// Scale all the elements of a matrix.
void ModelFbxLoader::MatrixScale(KFbxXMatrix& pMatrix, double pValue)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pMatrix[i][j] *= pValue;
        }
    }
}

// Add a value to all the elements in the diagonal of the matrix.
void ModelFbxLoader::MatrixAddToDiagonal(KFbxXMatrix& pMatrix, double pValue)
{
    pMatrix[0][0] += pValue;
    pMatrix[1][1] += pValue;
    pMatrix[2][2] += pValue;
    pMatrix[3][3] += pValue;
}

// Sum two matrices element by element.
void ModelFbxLoader::MatrixAdd(KFbxXMatrix& pDstMatrix, KFbxXMatrix& pSrcMatrix)
{
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            pDstMatrix[i][j] += pSrcMatrix[i][j];
        }
    }
}

// Deform the vertex array in classic linear way.
void ModelFbxLoader::ComputeLinearDeformation(KFbxXMatrix& pGlobalPosition, KFbxMesh* pMesh, 
											  KTime& pTime, KFbxVector4* pVertexArray,
											  KFbxPose* pPose, Mesh::Mesh* mesh, bool mAllByControlPoint,
											  int vetices_offset, int vertices_count)
{
	// All the links must have the same link mode.
	KFbxCluster::ELinkMode lClusterMode = ((KFbxSkin*)pMesh->GetDeformer(0, KFbxDeformer::eSKIN))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	KFbxXMatrix* lClusterDeformation = new KFbxXMatrix[lVertexCount];
	memset(lClusterDeformation, 0, lVertexCount * sizeof(KFbxXMatrix));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	if (lClusterMode == KFbxCluster::eADDITIVE)
	{
		for (int i = 0; i < lVertexCount; ++i)
		{
			lClusterDeformation[i].SetIdentity();
		}
	}

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	int lSkinCount = pMesh->GetDeformerCount(KFbxDeformer::eSKIN);
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		KFbxSkin * lSkinDeformer = (KFbxSkin *)pMesh->GetDeformer(lSkinIndex, KFbxDeformer::eSKIN);
		
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			KFbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			const char *boneName = lCluster->GetLink()->GetName();

			int index = -1;

			/*for (int j=0; j<skeleton.size();j++)
			{	
				if (String::IsEqual(skeleton[j].name, boneName))
				{
					index = j;
				}
			}*/

			KFbxXMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose, index);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) 
			{            
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
				{
					continue;
				}

				// Compute the influence of the link on the vertex.
				KFbxXMatrix lInfluence = lVertexTransformMatrix;
				MatrixScale(lInfluence, lWeight);

				if (lClusterMode == KFbxCluster::eADDITIVE)
				{    
					// Multiply with the product of the deformations on the vertex.
					MatrixAddToDiagonal(lInfluence, 1.0 - lWeight);
					lClusterDeformation[lIndex] = lInfluence * lClusterDeformation[lIndex];

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == KFbxLink::eNORMALIZE || lLinkMode == KFbxLink::eTOTAL1
				{
					// Add to the sum of the deformations on the vertex.
					MatrixAdd(lClusterDeformation[lIndex], lInfluence);

					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex			
		}//lClusterCount
	}
	
	if (!mAllByControlPoint)
	{
		const int lPolygonCount = pMesh->GetPolygonCount();
		const KFbxVector4 * lControlPoints = pMesh->GetControlPoints();

		lVertexCount = 0;

		for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
		{
			for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex)
			{
				const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

				KFbxVector4 lCurrentVertex = lControlPoints[lControlPointIndex];

				KFbxVector4 lSrcVertex;

				lSrcVertex.mData[0] = static_cast<float>(lCurrentVertex[0]);
				lSrcVertex.mData[1] = static_cast<float>(lCurrentVertex[1]);
				lSrcVertex.mData[2] = static_cast<float>(lCurrentVertex[2]);
				lSrcVertex.mData[3] = 1;

				KFbxVector4 lDstVertex = lSrcVertex;
			
				KFbxVector4 lCurrentNormal;
				pMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);

				KFbxVector4 lSrcNormal;

				lSrcNormal.mData[0] = static_cast<float>(lCurrentNormal[0]);
                lSrcNormal.mData[1] = static_cast<float>(lCurrentNormal[1]);
                lSrcNormal.mData[2] = static_cast<float>(lCurrentNormal[2]);

				KFbxVector4 lDstNormal = lSrcNormal;

				double lWeight = lClusterWeight[lControlPointIndex];
	
				// Deform the vertex if there was at least a link with an influence on the vertex,
				if (lWeight != 0.0) 
				{
					lDstVertex = lClusterDeformation[lControlPointIndex].MultT(lSrcVertex);
					
					KFbxXMatrix tmp = lClusterDeformation[lControlPointIndex];
					tmp.SetT(KFbxVector4(0,0,0,0));

					lDstNormal = tmp.MultT(lSrcNormal);
					
					if (lClusterMode == KFbxCluster::eNORMALIZE)
					{
						// In the normalized link mode, a vertex is always totally influenced by the links. 
						lDstVertex /= lWeight;
						lDstNormal /= lWeight;
					}
					else if (lClusterMode == KFbxCluster::eTOTAL1)
					{
						// In the total 1 link mode, a vertex can be partially influenced by the links. 
						lSrcVertex *= (1.0 - lWeight);
						lDstVertex += lSrcVertex;

						lSrcNormal *= (1.0 - lWeight);
						lDstNormal += lSrcNormal;
					}
				} 

				res->mesh.vertices_anim[vetices_offset + lVertexCount].pos.x = lDstVertex.mData[0];
				res->mesh.vertices_anim[vetices_offset + lVertexCount].pos.y = lDstVertex.mData[1];
				res->mesh.vertices_anim[vetices_offset + lVertexCount].pos.z = lDstVertex.mData[2];

				res->mesh.vertices_anim[vetices_offset + lVertexCount].normal.x = lDstNormal.mData[0];
				res->mesh.vertices_anim[vetices_offset + lVertexCount].normal.y = lDstNormal.mData[1];
				res->mesh.vertices_anim[vetices_offset + lVertexCount].normal.z = lDstNormal.mData[2];

				res->mesh.vertices_anim[vetices_offset + lVertexCount].normal.Normalize();

				lVertexCount++;
			}
		}
	}
	else
	{
		for (int i = 0; i < vertices_count; i++) 
		{
			KFbxVector4 lSrcVertex;

			lSrcVertex.mData[0] = res->mesh.vertices_anim[i].pos.x;
			lSrcVertex.mData[1] = res->mesh.vertices_anim[i].pos.y;
			lSrcVertex.mData[2] = res->mesh.vertices_anim[i].pos.z;
			lSrcVertex.mData[3] = 1;

			KFbxVector4 lDstVertex = lSrcVertex;

			KFbxVector4 lSrcNormal;

			lSrcNormal.mData[0] = res->mesh.vertices_anim[i].normal.x;
            lSrcNormal.mData[1] = res->mesh.vertices_anim[i].normal.y;
            lSrcNormal.mData[2] = res->mesh.vertices_anim[i].normal.z;
		
			KFbxVector4 lDstNormal = lSrcNormal;

			double lWeight = lClusterWeight[i];

			// Deform the vertex if there was at least a link with an influence on the vertex,
			if (lWeight != 0.0) 
			{
				lDstVertex = lClusterDeformation[i].MultT(lSrcVertex);

				KFbxXMatrix tmp = lClusterDeformation[i];
				tmp.SetT(KFbxVector4(0,0,0,0));

				lDstNormal = tmp.MultT(lSrcNormal);

				if (lClusterMode == KFbxCluster::eNORMALIZE)
				{
					// In the normalized link mode, a vertex is always totally influenced by the links. 
					lDstVertex /= lWeight;

					lDstNormal /= lWeight;
				}
				else if (lClusterMode == KFbxCluster::eTOTAL1)
				{
					// In the total 1 link mode, a vertex can be partially influenced by the links. 
					lSrcVertex *= (1.0 - lWeight);
					lDstVertex += lSrcVertex;

					lSrcNormal *= (1.0 - lWeight);
					lDstNormal += lSrcNormal;
				}
			} 

			res->mesh.vertices_anim[vetices_offset + i].pos.x = lDstVertex.mData[0];
			res->mesh.vertices_anim[vetices_offset + i].pos.y = lDstVertex.mData[1];
			res->mesh.vertices_anim[vetices_offset + i].pos.z = lDstVertex.mData[2];

			res->mesh.vertices_anim[vetices_offset + i].normal.x = lDstNormal.mData[0];
			res->mesh.vertices_anim[vetices_offset + i].normal.y = lDstNormal.mData[1];
			res->mesh.vertices_anim[vetices_offset + i].normal.z = lDstNormal.mData[2];

			res->mesh.vertices_anim[vetices_offset + i].normal.Normalize();
		}
	}

	delete [] lClusterDeformation;
	delete [] lClusterWeight;
}

// Deform the vertex array in Dual Quaternion Skinning way.
void ModelFbxLoader::ComputeDualQuaternionDeformation(KFbxXMatrix& pGlobalPosition, KFbxMesh* pMesh, 
													  KTime& pTime, KFbxVector4* pVertexArray,
													  KFbxPose* pPose, Mesh::Mesh* mesh)
{
	// All the links must have the same link mode.
	KFbxCluster::ELinkMode lClusterMode = ((KFbxSkin*)pMesh->GetDeformer(0, KFbxDeformer::eSKIN))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	int lSkinCount = pMesh->GetDeformerCount(KFbxDeformer::eSKIN);

	KFbxDualQuaternion* lDQClusterDeformation = new KFbxDualQuaternion[lVertexCount];
	memset(lDQClusterDeformation, 0, lVertexCount * sizeof(KFbxDualQuaternion));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	for ( int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		KFbxSkin * lSkinDeformer = (KFbxSkin *)pMesh->GetDeformer(lSkinIndex, KFbxDeformer::eSKIN);
		int lClusterCount = lSkinDeformer->GetClusterCount();
		for ( int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			KFbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			const char *boneName = lCluster->GetLink()->GetName();

			int index = -1;

			/*for (int j=0; j<skeleton.size();j++)
			{	
				if (String::IsEqual(skeleton[j].name, boneName))
				{
					index = j;
				}
			}*/

			KFbxXMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime, pPose, index);

			KFbxQuaternion lQ = lVertexTransformMatrix.GetQ();
			KFbxVector4 lT = lVertexTransformMatrix.GetT();
			KFbxDualQuaternion lDualQuaternion(lQ, lT);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k) 
			{ 
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
					continue;

				// Compute the influence of the link on the vertex.
				KFbxDualQuaternion lInfluence = lDualQuaternion * lWeight;
				if (lClusterMode == KFbxCluster::eADDITIVE)
				{    
					// Simply influenced by the dual quaternion.
					lDQClusterDeformation[lIndex] = lInfluence;

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == KFbxLink::eNORMALIZE || lLinkMode == KFbxLink::eTOTAL1
				{
					if(lClusterIndex == 0)
					{
						lDQClusterDeformation[lIndex] = lInfluence;
					}
					else
					{
						// Add to the sum of the deformations on the vertex.
						// Make sure the deformation is accumulated in the same rotation direction. 
						// Use dot product to judge the sign.
						double lSign = lDQClusterDeformation[lIndex].GetFirstQuaternion().DotProduct(lDualQuaternion.GetFirstQuaternion());
						if( lSign >= 0.0 )
						{
							lDQClusterDeformation[lIndex] += lInfluence;
						}
						else
						{
							lDQClusterDeformation[lIndex] -= lInfluence;
						}
					}
					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex
		}//lClusterCount
	}
	
	/*for (int i = 0; i < lVertexCount; i++) 
	{
		KFbxVector4 lSrcVertex = pVertexArray[i];
		KFbxVector4& lDstVertex = pVertexArray[i];
		double lWeightSum = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeightSum != 0.0) 
		{
			lDQClusterDeformation[i].Normalize();
			lDstVertex = lDQClusterDeformation[i].Deform(lDstVertex);

			if (lClusterMode == KFbxCluster::eNORMALIZE)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeightSum;
			}
			else if (lClusterMode == KFbxCluster::eTOTAL1)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeightSum);
				lDstVertex += lSrcVertex;
			}
		} 
	}*/

	delete [] lDQClusterDeformation;
	delete [] lClusterWeight;
}

void ModelFbxLoader::ComputeSkinDeformation(KFbxXMatrix& pGlobalPosition, KFbxMesh* pMesh, 
											KTime& pTime, KFbxVector4* pVertexArray,
											KFbxPose* pPose, Mesh::Mesh* mesh, bool mAllByControlPoint,
											int vetices_offset, int vertices_count)
{
	KFbxSkin * lSkinDeformer = (KFbxSkin *)pMesh->GetDeformer(0, KFbxDeformer::eSKIN);

	if (!lSkinDeformer) return;

	KFbxSkin::ESkinningType lSkinningType = lSkinDeformer->GetSkinningType();

	if(lSkinningType == KFbxSkin::eLINEAR || lSkinningType == KFbxSkin::eRIGID)
	{
		ComputeLinearDeformation(pGlobalPosition, pMesh, pTime, pVertexArray, pPose, mesh, mAllByControlPoint, vetices_offset, vertices_count);
	}
	else if(lSkinningType == KFbxSkin::eDUALQUATERNION)
	{
		ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pTime, pVertexArray, pPose, mesh);
	}
	else if(lSkinningType == KFbxSkin::eBLEND)
	{
		int lVertexCount = pMesh->GetControlPointsCount();

		KFbxVector4* lVertexArrayLinear = new KFbxVector4[lVertexCount];
		memcpy(lVertexArrayLinear, pMesh->GetControlPoints(), lVertexCount * sizeof(KFbxVector4));

		KFbxVector4* lVertexArrayDQ = new KFbxVector4[lVertexCount];
		memcpy(lVertexArrayDQ, pMesh->GetControlPoints(), lVertexCount * sizeof(KFbxVector4));

		ComputeLinearDeformation(pGlobalPosition, pMesh, pTime, lVertexArrayLinear,
								 pPose, mesh, mAllByControlPoint, vetices_offset, vertices_count);

		ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pTime, lVertexArrayDQ, pPose, mesh);

		// To blend the skinning according to the blend weights
		// Final vertex = DQSVertex * blend weight + LinearVertex * (1- blend weight)
		// DQSVertex: vertex that is deformed by dual quaternion skinning method;
		// LinearVertex: vertex that is deformed by classic linear skinning method;
		int lBlendWeightsCount = lSkinDeformer->GetControlPointIndicesCount();
		for(int lBWIndex = 0; lBWIndex<lBlendWeightsCount; ++lBWIndex)
		{
			double lBlendWeight = lSkinDeformer->GetControlPointBlendWeights()[lBWIndex];
			pVertexArray[lBWIndex] = lVertexArrayDQ[lBWIndex] * lBlendWeight + lVertexArrayLinear[lBWIndex] * (1 - lBlendWeight);
		}
	}
}

void ModelFbxLoader::GetMaterialProperty(const KFbxSurfaceMaterial * pMaterial,
										 const char * pPropertyName,
										 char* pTextureName)
{
	pTextureName[0] = 0;

	const KFbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
	
	if (lProperty.IsValid())
	{
		const int lTextureCount = lProperty.GetSrcObjectCount(KFbxFileTexture::ClassId);
		if (lTextureCount)
		{
			const KFbxFileTexture* lTexture = lProperty.GetSrcObject(FBX_TYPE(KFbxFileTexture), 0);
			if (lTexture)
			{
				String::Copy(pTextureName, 128, lTexture->GetFileName());
			}
		}
	}
}

void ModelFbxLoader::CountBones(KFbxNode* pNode, int& num_bones)
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

void ModelFbxLoader::ExtractBones(KFbxNode* pNode, int& cur_bone)
{
	KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute && lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eSKELETON)
	{
		KFbxSkeleton* lSkeleton = (KFbxSkeleton*) pNode->GetNodeAttribute();

		if (lSkeleton->GetSkeletonType() == KFbxSkeleton::eLIMB_NODE)
		{
			Mesh::Bone* bone = &res->mesh.bones[cur_bone];

			String::Copy(bone->name, 64, pNode->GetName());
			bone->parent = -1;

			Matrix mat;	
			ConvertMatrix(bone->bindMatrix, pNode->EvaluateGlobalTransform());
			bone->bindMatrix.Normalize();
			bone->bindMatrix.InverseComplette();
			
			if (pNode->GetParent() &&
				pNode->GetParent()->GetNodeAttribute() &&
				pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eSKELETON)
			{
				const char* parent = pNode->GetParent()->GetName();

				for (int i=0; i<cur_bone; i++)
				{
					if (String::IsEqual(res->mesh.bones[i].name, parent))
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
		ExtractBones(pNode->GetChild(lChildIndex), cur_bone);
    }
}

void ModelFbxLoader::CountTextures(KFbxNode* pNode, int& num_textures)
{
	KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute)
	{
		if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH)
		{
			KFbxMesh* pMesh = pNode->GetMesh();

			if (pMesh->GetNode())
			{
				num_textures += pNode->GetMaterialCount();
			}
		}
	}

	const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
	{
		CountTextures(pNode->GetChild(lChildIndex), num_textures);
	}
}

void ModelFbxLoader::ExtractTextures(KFbxNode* pNode, int& cur_texture)
{
	KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

    if (lNodeAttribute)
    {
        if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH)
        {
			KFbxMesh* pMesh = pNode->GetMesh();

			if (pMesh->GetNode())
			{
				const int lMaterialCount = pNode->GetMaterialCount();

				for (int lMaterialIndex = 0; lMaterialIndex < lMaterialCount; lMaterialIndex++)
				{
					KFbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lMaterialIndex);
									
					res->mesh.materials[cur_texture].num_tex = 0;

					if (lMaterial && !lMaterial->GetUserDataPtr())
					{				
						for (int i=0;i<4;i++)
						{
							char txt[256];
							txt[0] = 0;

							if (i==0)
							{
								GetMaterialProperty(lMaterial, KFbxSurfaceMaterial::sDiffuse, txt);
							}
							else
							if (i==1)
							{
								GetMaterialProperty(lMaterial, KFbxSurfaceMaterial::sSpecular, txt);
							}
							else
							if (i==2)
							{
								GetMaterialProperty(lMaterial, KFbxSurfaceMaterial::sBump, txt);
							}
							else
							if (i==3)
							{
								GetMaterialProperty(lMaterial, KFbxSurfaceMaterial::sReflection, txt);
							}

							if (String::IsEmpty(txt)) break;

							String::ExtractFileName(txt, &res->mesh.materials[cur_texture].textures_names[32*i]);
							
							String::Copy(txt, 256, path);
							String::Cat(txt, 256, &res->mesh.materials[cur_texture].textures_names[32*i]);

							res->mesh.materials[cur_texture].textures[i] = en_core.Render()->LoadTexture(txt, _FL_);
							res->mesh.materials[cur_texture].num_tex++;
						}
					}
				
					cur_texture++;					
				}				
			}
        }
    }

	const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
		ExtractTextures(pNode->GetChild(lChildIndex), cur_texture);
    }
}

void ModelFbxLoader::CountMeshes(KFbxNode* pNode, int& num_meshes, int& num_vertices, int& num_indices)
{
	KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

    if (lNodeAttribute)
    {
        if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH)
        {
			KFbxMesh* pMesh = pNode->GetMesh();

			if (pMesh->GetNode())
			{
				bool mAllByControlPoint = true;

				bool mHasNormal = pMesh->GetElementNormalCount() > 0;
				bool mHasUV = pMesh->GetElementUVCount() > 0;
				KFbxGeometryElement::EMappingMode lNormalMappingMode = KFbxGeometryElement::eNONE;
				KFbxGeometryElement::EMappingMode lUVMappingMode = KFbxGeometryElement::eNONE;

				if (mHasNormal)
				{
					lNormalMappingMode = pMesh->GetElementNormal(0)->GetMappingMode();
					if (lNormalMappingMode == KFbxGeometryElement::eNONE)
					{
						mHasNormal = false;
					}

					if (mHasNormal && lNormalMappingMode != KFbxGeometryElement::eBY_CONTROL_POINT)
					{
						mAllByControlPoint = false;
					}
				}

				if (mHasUV)
				{
					lUVMappingMode = pMesh->GetElementUV(0)->GetMappingMode();
					if (lUVMappingMode == KFbxGeometryElement::eNONE)
					{
						mHasUV = false;
					}

					if (mHasUV && lUVMappingMode != KFbxGeometryElement::eBY_CONTROL_POINT)
					{
						mAllByControlPoint = false;
					}
				}

				const int lPolygonCount = pMesh->GetPolygonCount();

				// Allocate the array memory, by control point or by polygon vertex.
				int lPolygonVertexCount = pMesh->GetControlPointsCount();
				if (!mAllByControlPoint)
				{
					lPolygonVertexCount = lPolygonCount * 3;
				}

				num_vertices += lPolygonVertexCount;
				num_indices += lPolygonCount * 3;

				num_meshes++;
			}
        }
    }

	const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
		CountMeshes(pNode->GetChild(lChildIndex), num_meshes, num_vertices, num_indices);
    }
}

void ModelFbxLoader::AddSkinData(int vertex, int bone, float  weight)
{
	Mesh::VertexAnim* vtx = &res->mesh.vertices_anim[vertex];						

	int bone_index = bone;

	int cur_bone_index = vtx->bones;

	for (int l=0;l<4;l++)						
	{
		if (vtx->weights.v4[l] < 0.0000001f)
		{								
			vtx->bones |= bone_index;
			vtx->weights.v4[l] = weight;							

			break;
		}

		if ((cur_bone_index & 0xff) == bone) break;

		bone_index = bone_index<<8;							
		cur_bone_index = cur_bone_index>>8;
	}
}	

void ModelFbxLoader::ExtractSkin(KFbxMesh *mesh, Mesh::Mesh* model_mesh, int vertices_offset, int* tmp_indices)
{
	if (!mesh->GetDeformerCount()) return;
	
	int lSkinCount = mesh->GetDeformerCount(KFbxDeformer::eSKIN);

	if (!lSkinCount) return;

	KFbxSkin* skin = (KFbxSkin *)mesh->GetDeformer(0, KFbxDeformer::eSKIN);

	int clusterCount = skin->GetClusterCount();
	
	for (int i = 0 ; i < clusterCount ; i++)
	{	
		KFbxCluster *cluster = skin->GetCluster(i);
		KFbxCluster::ELinkMode lClusterMode = cluster->GetLinkMode();
		const char *boneName = cluster->GetLink()->GetName();
		KFbxXMatrix kLinkMatrix;
		cluster->GetTransformLinkMatrix(kLinkMatrix);
		KFbxXMatrix kTM;
		cluster->GetTransformMatrix(kTM);
		KFbxXMatrix kInvLinkMatrix(kLinkMatrix.Inverse());
		KFbxXMatrix kM(kInvLinkMatrix * kTM);
		
		int index = -1;

		for (int j=0; j<res->mesh.num_bones;j++)
		{
			if (String::IsEqual(res->mesh.bones[j].name, boneName))
			{
				index = j;
			}
		}

		if (index != -1)
		{
			int indexCount = cluster->GetControlPointIndicesCount();
			int *indices = cluster->GetControlPointIndices();
			double *weights = cluster->GetControlPointWeights();
		
			for (int j = 0 ; j < indexCount ; j++)
			{
				int vertex = indices[j];
				float weight = (float)weights[j];


				if (!tmp_indices)
				{
					AddSkinData(vertices_offset + vertex, index, weight);
				}
				else
				{
					for (int k = 0 ; k < model_mesh->triangleCount * 3 ; k++)
					{
						if (tmp_indices[k] == vertex)
						{
							AddSkinData(vertices_offset + k, index, weight);							
						}
					}
				}						
			}
		}
	}
}

void ModelFbxLoader::ExtractMesh(KFbxNode* pNode, int& cur_meshes, int& vertices_offset, int& indices_offset)
{
	Matrix mt;
	ConvertMatrix(mt, pNode->EvaluateLocalTransform());

	Matrix pre;
	pre.BuildRotateX(-90.0f * RADIAN);
	mt = mt * pre;
	
	KFbxMesh* pMesh = pNode->GetMesh();

	bool mHasNormal = false;
	bool mHasUV = false;
	bool mAllByControlPoint = true;

	if (!pMesh->GetNode()) return;

	Mesh::Mesh* mesh = &res->mesh.meshes[cur_meshes];
	cur_meshes++;

	mesh->triangleCount = 0;
	mesh->material = 0;
	mesh->indices_offset = indices_offset;

    const int lPolygonCount = pMesh->GetPolygonCount();

    // Count the polygon count of each material
    KFbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
    KFbxGeometryElement::EMappingMode lMaterialMappingMode = KFbxGeometryElement::eNONE;
    
	/*if (pMesh->GetElementMaterial())
    {
        lMaterialIndice = &pMesh->GetElementMaterial()->GetIndexArray();
        lMaterialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
        
		if (lMaterialIndice && lMaterialMappingMode == KFbxGeometryElement::eBY_POLYGON)
        {
			//Why?????
            K_ASSERT(lMaterialIndice->GetCount() == lPolygonCount);
            
			if (lMaterialIndice->GetCount() == lPolygonCount)
            {
                for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
                {
                    const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
					if (mesh->num_submeshes < lMaterialIndex + 1) mesh->num_submeshes = lMaterialIndex + 1;
				}

				mesh->subMeshes = new Mesh::SubMesh[mesh->num_submeshes];
			
                for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
                {
                    const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
                    mesh->subMeshes[lMaterialIndex].triangleCount += 1;
                }

                // Record the offset (how many vertex)
                int lOffset = 0;

                for (int lIndex = 0; lIndex < mesh->num_submeshes; ++lIndex)
                {
                    mesh->subMeshes[lIndex].indexOffset = lOffset;
					mesh->subMeshes[lIndex].num_tex = 0;

                    lOffset += mesh->subMeshes[lIndex].triangleCount * 3;
                    // This will be used as counter in the following procedures, reset to zero
                    mesh->subMeshes[lIndex].triangleCount = 0;
                }

				/// Why??
                K_ASSERT(lOffset == lPolygonCount * 3);
            }
        }
    }*/

    // All faces will use the same material.
    /*if (mesh->num_submeshes == 0)
    {
        mesh->subMeshes = new Mesh::SubMesh[1];
		mesh->subMeshes[0].num_tex = 0;		
    }*/

    // Congregate all the data of a mesh to be cached in VBOs.
    // If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
    mHasNormal = pMesh->GetElementNormalCount() > 0;
    mHasUV = pMesh->GetElementUVCount() > 0;
    KFbxGeometryElement::EMappingMode lNormalMappingMode = KFbxGeometryElement::eNONE;
    KFbxGeometryElement::EMappingMode lUVMappingMode = KFbxGeometryElement::eNONE;
    
	if (mHasNormal)
    {
        lNormalMappingMode = pMesh->GetElementNormal(0)->GetMappingMode();
        if (lNormalMappingMode == KFbxGeometryElement::eNONE)
        {
            mHasNormal = false;
        }
        if (mHasNormal && lNormalMappingMode != KFbxGeometryElement::eBY_CONTROL_POINT)
        {
            mAllByControlPoint = false;
        }
    }
    
	if (mHasUV)
    {
        lUVMappingMode = pMesh->GetElementUV(0)->GetMappingMode();
        if (lUVMappingMode == KFbxGeometryElement::eNONE)
        {
            mHasUV = false;
        }
        if (mHasUV && lUVMappingMode != KFbxGeometryElement::eBY_CONTROL_POINT)
        {
            mAllByControlPoint = false;
        }
    }

    // Allocate the array memory, by control point or by polygon vertex.
    int lPolygonVertexCount = pMesh->GetControlPointsCount();
    if (!mAllByControlPoint)
    {
        lPolygonVertexCount = lPolygonCount * 3;
    }
	
    KStringList lUVNames;
    pMesh->GetUVSetNames(lUVNames);
    const char * lUVName = NULL;
    if (mHasUV && lUVNames.GetCount())
    {
        lUVName = lUVNames[0];
    }

    // Populate the array with vertex attribute, if by control point.
    const KFbxVector4 * lControlPoints = pMesh->GetControlPoints();
    
	KFbxVector4 lCurrentVertex;
    KFbxVector4 lCurrentNormal;
    KFbxVector2 lCurrentUV;
    
	if (mAllByControlPoint)
    {
        const KFbxGeometryElementNormal * lNormalElement = NULL;
        const KFbxGeometryElementUV * lUVElement = NULL;
        
		if (mHasNormal)
        {
            lNormalElement = pMesh->GetElementNormal(0);
        }

        if (mHasUV)
        {
            lUVElement = pMesh->GetElementUV(0);
        }

        for (int lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
        {
            // Save the vertex position.
            lCurrentVertex = lControlPoints[lIndex];

			if (res->mesh.num_bones)
			{
				res->mesh.vertices_anim[vertices_offset + lIndex].pos.x = static_cast<float>(lCurrentVertex[0]);
				res->mesh.vertices_anim[vertices_offset + lIndex].pos.y = static_cast<float>(lCurrentVertex[1]);
				res->mesh.vertices_anim[lIndex].pos.z = static_cast<float>(lCurrentVertex[2]);				
			}
			else
			{
				res->mesh.vertices[vertices_offset + lIndex].pos.x = static_cast<float>(lCurrentVertex[0]);
				res->mesh.vertices[vertices_offset + lIndex].pos.y = static_cast<float>(lCurrentVertex[1]);
				res->mesh.vertices[vertices_offset + lIndex].pos.z = static_cast<float>(lCurrentVertex[2]);				
			}

            // Save the normal.
            if (mHasNormal)
            {
                int lNormalIndex = lIndex;
                if (lNormalElement->GetReferenceMode() == KFbxLayerElement::eINDEX_TO_DIRECT)
                {
                    lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);
                }
                lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
                
				if (res->mesh.num_bones)
				{
					res->mesh.vertices_anim[vertices_offset + lIndex].normal.x = static_cast<float>(lCurrentNormal[0]);
					res->mesh.vertices_anim[vertices_offset + lIndex].normal.y = static_cast<float>(lCurrentNormal[1]);
					res->mesh.vertices_anim[vertices_offset + lIndex].normal.z = static_cast<float>(lCurrentNormal[2]);
				}
				else
				{
					res->mesh.vertices[vertices_offset + lIndex].normal.x = static_cast<float>(lCurrentNormal[0]);
					res->mesh.vertices[vertices_offset + lIndex].normal.y = static_cast<float>(lCurrentNormal[1]);
					res->mesh.vertices[vertices_offset + lIndex].normal.z = static_cast<float>(lCurrentNormal[2]);
				}
            }

            // Save the UV.
            if (mHasUV)
            {
                int lUVIndex = lIndex;
                if (lUVElement->GetReferenceMode() == KFbxLayerElement::eINDEX_TO_DIRECT)
                {
                    lUVIndex = lUVElement->GetIndexArray().GetAt(lIndex);
                }
                lCurrentUV = lUVElement->GetDirectArray().GetAt(lUVIndex);

				if (res->mesh.num_bones)
				{
					res->mesh.vertices_anim[vertices_offset + lIndex].uv.x = static_cast<float>(lCurrentUV[0]);
					res->mesh.vertices_anim[vertices_offset + lIndex].uv.y = static_cast<float>(lCurrentUV[1]);
				}
				else
				{
					res->mesh.vertices[vertices_offset + lIndex].uv.x = static_cast<float>(lCurrentUV[0]);
					res->mesh.vertices[vertices_offset + lIndex].uv.y = static_cast<float>(lCurrentUV[1]);
				}
            }

			if (res->mesh.num_bones)
			{
				res->mesh.vertices_anim[vertices_offset + lIndex].bones = 0;
				res->mesh.vertices_anim[vertices_offset + lIndex].weights = Vector4(0.0f);
			}
        }

    }

    int lVertexCount = 0;	

	int* tmp_indices = null;

	if (!mAllByControlPoint)
	{
		tmp_indices = new int[lPolygonVertexCount];
	}

    for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
    {
        // The material for current face.
        int lMaterialIndex = 0;
        if (lMaterialIndice && lMaterialMappingMode == KFbxGeometryElement::eBY_POLYGON)
        {
            lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
        }
		
        // Where should I save the vertex attribute index, according to the material        
		//const int lIndexOffset = mesh->subMeshes[lMaterialIndex].indexOffset +
			//					 mesh->subMeshes[lMaterialIndex].triangleCount * 3;
        
		const int lIndexOffset = indices_offset + mesh->triangleCount * 3;

		for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex)
        {
            const int lControlPointIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

            if (mAllByControlPoint)
            {
                res->mesh.indices[lIndexOffset + lVerticeIndex] = vertices_offset + static_cast<unsigned int>(lControlPointIndex);
            }            
            else
            {
                res->mesh.indices[lIndexOffset + lVerticeIndex] = vertices_offset + static_cast<unsigned int>(lVertexCount);				

				tmp_indices[lVertexCount] = lControlPointIndex;

                lCurrentVertex = lControlPoints[lControlPointIndex];
                
				if (res->mesh.num_bones)
				{
					res->mesh.vertices_anim[vertices_offset+lVertexCount].pos.x = static_cast<float>(lCurrentVertex[0]);
					res->mesh.vertices_anim[vertices_offset+lVertexCount].pos.y = static_cast<float>(lCurrentVertex[1]);
					res->mesh.vertices_anim[vertices_offset+lVertexCount].pos.z = static_cast<float>(lCurrentVertex[2]);                

					if (mHasNormal)
					{
						pMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
						res->mesh.vertices_anim[vertices_offset+lVertexCount].normal.x = static_cast<float>(lCurrentNormal[0]);
						res->mesh.vertices_anim[vertices_offset+lVertexCount].normal.y = static_cast<float>(lCurrentNormal[1]);
						res->mesh.vertices_anim[vertices_offset+lVertexCount].normal.z = static_cast<float>(lCurrentNormal[2]);
					}

					if (mHasUV)
					{
						pMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName, lCurrentUV);
						res->mesh.vertices_anim[vertices_offset+lVertexCount].uv.x = static_cast<float>(lCurrentUV[0]);
						res->mesh.vertices_anim[vertices_offset+lVertexCount].uv.y = 1 - static_cast<float>(lCurrentUV[1]);
					}

					res->mesh.vertices_anim[vertices_offset + lVertexCount].bones = 0;
					res->mesh.vertices_anim[vertices_offset + lVertexCount].weights = Vector4(0.0f);					
				}
				else
				{
					res->mesh.vertices[vertices_offset+lVertexCount].pos.x = static_cast<float>(lCurrentVertex[0]);
					res->mesh.vertices[vertices_offset+lVertexCount].pos.y = static_cast<float>(lCurrentVertex[1]);
					res->mesh.vertices[vertices_offset+lVertexCount].pos.z = static_cast<float>(lCurrentVertex[2]);                

					if (mHasNormal)
					{
						pMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
						res->mesh.vertices[vertices_offset+lVertexCount].normal.x = static_cast<float>(lCurrentNormal[0]);
						res->mesh.vertices[vertices_offset+lVertexCount].normal.y = static_cast<float>(lCurrentNormal[1]);
						res->mesh.vertices[vertices_offset+lVertexCount].normal.z = static_cast<float>(lCurrentNormal[2]);
					}

					if (mHasUV)
					{
						pMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName, lCurrentUV);
						res->mesh.vertices[vertices_offset+lVertexCount].uv.x = static_cast<float>(lCurrentUV[0]);
						res->mesh.vertices[vertices_offset+lVertexCount].uv.y = 1 - static_cast<float>(lCurrentUV[1]);
					}
				}

            }
            
			lVertexCount++;
        }
        
		mesh->triangleCount++;
    }
	
	if (res->mesh.num_bones)
	{
		ExtractSkin(pMesh, mesh, vertices_offset, tmp_indices);

		KFbxXMatrix pGlobalPosition;
		KTime pTime;

		ComputeSkinDeformation(pGlobalPosition, 
							   pMesh, pTime, NULL, NULL, mesh,
							   mAllByControlPoint, vertices_offset, lPolygonVertexCount);		
	}
	else
	{
		for (int i=0;i<lPolygonVertexCount;i++)
		{
			res->mesh.vertices[vertices_offset + i].pos = res->mesh.vertices[vertices_offset + i].pos * mt;
			res->mesh.vertices[vertices_offset + i].normal = mt.MulNormal(res->mesh.vertices[vertices_offset + i].normal);
			res->mesh.vertices[vertices_offset + i].normal.Normalize();
		}
	}	

	vertices_offset += lPolygonVertexCount;
	indices_offset += lPolygonCount * 3;

	if (tmp_indices) delete[] tmp_indices;
}

void ModelFbxLoader::ExtractMeshes(KFbxNode* pNode, int& cur_meshes, int& offset_vertices, int& offset_indices)
{
	KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

    if (lNodeAttribute)
    {
        if (lNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eMESH)
        {
			ExtractMesh(pNode, cur_meshes, offset_vertices, offset_indices);			
        }
    }

	const int lChildCount = pNode->GetChildCount();
    for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
    {
		ExtractMeshes(pNode->GetChild(lChildIndex), cur_meshes, offset_vertices, offset_indices);
    }
}

bool ModelFbxLoader::Load(ModelRes* rs, const char* name)
{		
	res = rs;

	String::ExtractPath(name, path, false);

    sdkManager = KFbxSdkManager::Create();

    if (!sdkManager)
    {
        core->TraceTo("Models", "Unable to create the FBX SDK manager");
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

			CountBones(scene->GetRootNode(), res->mesh.num_bones);

			if (res->mesh.num_bones)
			{
				res->mesh.bones = new Mesh::Bone[res->mesh.num_bones];
				int index = 0;

				ExtractBones(scene->GetRootNode(), index);

				res->mesh.type = Mesh::animated_mesh;
			}
			
			//TriangulateRecursive(pScene->GetRootNode());
			
			CountTextures(scene->GetRootNode(), res->mesh.num_materials);

			if (res->mesh.num_materials)
			{
				res->mesh.materials = new Mesh::Material[res->mesh.num_materials];
				
				int index = 0;
				ExtractTextures(scene->GetRootNode(), index);
			}

			CountMeshes(scene->GetRootNode(), res->mesh.num_meshes, res->mesh.num_vertices, res->mesh.num_indices);

			if (res->mesh.num_meshes)
			{
				res->mesh.meshes = new Mesh::Mesh[res->mesh.num_meshes];
				
				if (!res->mesh.num_bones)
				{	
					res->mesh.vertices = new Mesh::Vertex[res->mesh.num_vertices];
				}
				else
				{
					res->mesh.vertices_anim = new Mesh::VertexAnim[res->mesh.num_vertices];
				}

				res->mesh.indices = new word[res->mesh.num_indices];

				int index = 0;
				int vertex_offset = 0;
				int indices_offset = 0;
				ExtractMeshes(scene->GetRootNode(), index, vertex_offset, indices_offset);
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