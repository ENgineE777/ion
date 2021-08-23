#pragma once

#ifdef FBX

#include "core/Model/ModelRes.h"
#include <fbxsdk.h>

class ModelFbxLoader
{
public:	
		
	static KFbxSdkManager* sdkManager;
	static KFbxScene* scene;
	static char path[256];

	static ModelRes* res;

	static void ConvertMatrix(Matrix& mat, KFbxXMatrix& fbx_mat);

	static KFbxXMatrix GetPoseMatrix(KFbxPose* pPose, int pNodeIndex);

	static KFbxXMatrix GetGeometry(KFbxNode* pNode);

	static KFbxXMatrix GetGlobalPosition(KFbxNode* pNode, const KTime& pTime, KFbxPose* pPose = NULL,
										 KFbxXMatrix* pParentGlobalPosition = NULL);

	static void ComputeClusterDeformation(KFbxXMatrix& pGlobalPosition, KFbxMesh* pMesh,
										  KFbxCluster* pCluster, KFbxXMatrix& pVertexTransformMatrix,
										  KTime pTime, KFbxPose* pPose, int bone_index);

	static void MatrixScale(KFbxXMatrix& pMatrix, double pValue);

	static void MatrixAddToDiagonal(KFbxXMatrix& pMatrix, double pValue);

	static void MatrixAdd(KFbxXMatrix& pDstMatrix, KFbxXMatrix& pSrcMatrix);

	static void ComputeLinearDeformation(KFbxXMatrix& pGlobalPosition, KFbxMesh* pMesh, 
										 KTime& pTime, KFbxVector4* pVertexArray,
										 KFbxPose* pPose, Mesh::Mesh* mesh, bool mAllByControlPoint,
										 int vetices_offset, int vertices_count);

	static void ComputeDualQuaternionDeformation(KFbxXMatrix& pGlobalPosition, KFbxMesh* pMesh, 
												 KTime& pTime, KFbxVector4* pVertexArray,
												 KFbxPose* pPose, Mesh::Mesh* mesh);

	static void ComputeSkinDeformation(KFbxXMatrix& pGlobalPosition, KFbxMesh* pMesh, KTime& pTime, 
									   KFbxVector4* pVertexArray, KFbxPose* pPose, Mesh::Mesh* mesh,
									   bool mAllByControlPoint, int vetices_offset, int vertices_count);

	static void GetMaterialProperty(const KFbxSurfaceMaterial * pMaterial, const char * pPropertyName, char* pTextureName);

	static void CountBones(KFbxNode* pNode, int& num_bones);
	static void ExtractBones(KFbxNode* pNode, int& cur_bone);

	static void CountTextures(KFbxNode* pNode, int& num_textures);
	static void ExtractTextures(KFbxNode* pNode, int& cur_texture);

	static void CountMeshes(KFbxNode* pNode, int& num_meshes, int& num_vertices, int& num_indices);
	static void ModelFbxLoader::AddSkinData(int vertex, int bone, float  weight);
	static void ExtractSkin(KFbxMesh *mesh, Mesh::Mesh* model_mesh, int vertices_offset, int* tmp_indices);
	static void ExtractMesh(KFbxNode* pNode, int& cur_meshes, int& offset_vertices, int& offset_indices);
	static void ExtractMeshes(KFbxNode* pNode, int& num_meshes, int& offset_vertices, int& offset_indices);

	static bool Load(ModelRes* res, const char* name);
};

#endif