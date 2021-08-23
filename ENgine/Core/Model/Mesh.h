#pragma once

#include "Common/Common.h"

class TextureInstance;

namespace Mesh
{
	static const dword animMagicID = MAKEFOURCC('A', 'N', 'I', 'M');	
	static const dword animSubID   = MAKEFOURCC('V', 'F', '0', '1');	
	
	static const dword geomMagicID = MAKEFOURCC('M', 'E', 'S', 'H');
	static const dword geomSubID   = MAKEFOURCC('V', 'F', '0', '1');	

	struct Bone
	{
		char   name[64];
		int    parent;
		Matrix bindMatrix;

		Matrix cache;
	};

	struct BoneKey
	{
		float      time;
		Vector     scale;
		Quaternion rot;
		Vector     pos;
	};

	struct BoneAnimTrack
	{
		char     name[64];
		int      parent;
		int      num_keys;
		BoneKey* keys;

		BoneAnimTrack()
		{
			parent = -1;
			num_keys = 0;
			keys = null;
		}
	};

	struct Animation
	{
		float time_lenght;

		int num_bones;
		BoneAnimTrack* bones;		

		Animation()
		{
			time_lenght = 0.0f;
			num_bones = 0;
		}
	};	
		
	struct Vertex
	{
		Vector  pos;
		Vector  normal;
		Vector2 uv;
	};

	struct VertexAnim
	{
		Vector  pos;
		Vector  normal;
		Vector2 uv;
		dword   bones;
		Vector4 weights;

		VertexAnim()
		{
			bones = 0;
		};
	};	
	
	struct Mesh
	{
		int indices_offset;		
		int triangleCount;
		int material;		
	};

	struct Material
	{
		int num_tex;
		char textures_names[128];
		TextureInstance* textures[4];
	};

	enum MeshType
	{
		static_mesh,
		animated_mesh,
		forcedword_mesh = 0x7fffffff
	};

	struct MeshHolder
	{
		MeshType  type;
		
		int		  num_meshes;
		Mesh*	  meshes;
		
		int		  num_vertices;		
		union
		{
			Vertex*     vertices;
			VertexAnim* vertices_anim;
		};

		int       num_indices;
		word*	  indices;		

		int		  num_materials;
		Material* materials;	

		int       num_bones;
		Bone*	  bones;

		MeshHolder()
		{
			type = static_mesh;
			
			num_meshes = 0;
			meshes = null;

			num_vertices = 0;
			vertices = null;
				
			num_indices = 0;
			indices = null;		

			num_materials = 0;
			materials = null;	

			num_bones = 0;
			bones = null;
		};
	};
}