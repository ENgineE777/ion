 
#include "PhysicsEntity.h"

//#include "pch.h"
//#include "projectile.h"
//#include "PhysActor.h"
//#include "PhysicsEngine.h"
//#include "game.h"
//#include "md1.h"
#include <algorithm>
//#include "FileSystem.h"
//#include "ThreadReader.h"
//#include "PhysActorDesc.h"

//CLASS_DECLARATION( CEntity, CPhysicsEntity )

ENTITY_DECL(CPhysicsEntity, "CPhysicsEntity", 0)


CPhysicsEntity::CPhysicsEntity():CEntity()
{
	m_actor = NULL;	
}

CPhysicsEntity::~CPhysicsEntity()
{
	if (m_actor)
	{
		m_actor->m_userData = NULL;
		//PhysicsEngine::Instance.ReleaseActor(m_actor);
	}
}

typedef struct {
	float			uv[2];
	unsigned short	color;
	float			normal[3];
	float			xyz[3];
} vertex_t;

int  find( const std::vector<Vector3>& _stdvector, const Vector3& _v)
{
	int size = _stdvector.size();
	for (int i = 0; i < size; ++i)
	{
		if (_stdvector[i] == _v)
			return i;
	}

	return -1;
}

void CreateMd3FromMd1(byte* buf, char *fullPath);

void CPhysicsEntity::Init()
{
	CEntity::Init();

	m_actor = null;

	PROP_REG(phys_type)
	PROP_REG(phys_shape)		
	PROP_REG(phys_radius)
	PROP_REG(phys_weight)
	PROP_REG(phys_restitution)	

	m_type = (enumCPhysicsEnityType)phys_type;
}

void CPhysicsEntity::Start()
{
	CEntity::Start();

	const char *shapeType = phys_shape;

	SPhysActorDesc actorDesc;
	actorDesc.m_pos = (Vector3&)m_state.origin;
	actorDesc.m_quat = (Quaternion&)m_state.quat;

	actorDesc.m_weight = phys_weight;
	actorDesc.m_restitution = phys_restitution;

	switch((int)PhysicsEngine::instance()->StringToShapeType(shapeType))
	{
	case eAnimatedMesh:
	case eSphere:
		{
			PhysSphereDesc sphere;
			PhysMaterialSingle material;
			
			actorDesc.m_shapeDesc = &sphere;
			actorDesc.m_material = &material;

			sphere.GetRadius() = phys_radius;
			material.m_materialID = OtherMaterial;


			m_actor = &PhysicsEngine::instance()->AddActor( actorDesc);
		}

		break;

	//case eMesh:
	//	{
	//		const char *path;
	//		m_createArgs.GetString("path", NULL, &path );

	//		const char *nameMd3;
	//		m_createArgs.GetString("physMesh", NULL, &nameMd3 );

	//		


	//		char fullPath[512];
	//		strcpy(fullPath, path );
	//		strcat(fullPath, nameMd3 );


	//		int size = 0;
	//		// check existing md3
	//		int hf = FS_Open( fullPath, 0 );

	//		//create md3 if it not exists
	//		if ( hf == -1 )
	//		{
	//			char* ext = strchr( fullPath, '.');
	//			strcpy(ext, ".md1");
	//			byte* buf = (byte*)FS_LoadFile( fullPath, &size );

	//			assert(buf && "no md1 and md3 files for phys mesh");

	//			CreateMd3FromMd1(buf, fullPath);

	//			strcpy(ext, ".md3");

	//			Com_Free(buf);
	//			//read writen data
	//		}
	//		else
	//			FS_Close(hf);


	//		PhysMeshDesc physmesh;
	//		PhysMaterialSingle material;
	//		
	//		//ReadMd3FromBuf(buf, physmesh);

	//		actorDesc.m_shapeDesc = &physmesh;
	//		actorDesc.m_material = &material;

	//		strcpy(physmesh.m_md3file, fullPath);

	//		material.m_materialID = OtherMaterial;

	//		m_actor = &PhysicsEngine::instance()->AddActor( actorDesc);


	//	}
	//	break;

	//case eLine:
	//	{
	//		PhysLineDesc line;
	//		PhysMaterialSingle material;
	//		
	//		actorDesc.m_shapeDesc = &line;
	//		actorDesc.m_material = &material;

	//		line.dir.MakeZero();

	//		m_createArgs.GetFloat("length","1", line.dir.z);
	//		
	//		line.dir.z *= -1;

	//		material.m_materialID = OtherMaterial;

	//		m_actor = &PhysicsEngine::instance()->AddActor( actorDesc);

	//	}

	//	break;
	default:
		{
			PhysSphereDesc sphere;
			PhysMaterialSingle material;
			
			actorDesc.m_shapeDesc = &sphere;
			actorDesc.m_material = &material;

			sphere.GetRadius() = 1;
			material.m_materialID = OtherMaterial;


			m_actor = &PhysicsEngine::instance()->AddActor( actorDesc);
		}
			
	}


	//if ( GetType() == &CPhysicsEntity::Type )
	//{
		BecomeActive();
	//}

	
	m_type = eDynamyc;
	m_actor->SetGravity(true);
	m_actor->getCanCollide() = true;
	m_actor->m_userData = this;
	//m_actor->GetIsActive() = false;
}

void CPhysicsEntity::Finish()
{	
	m_actor->m_userData = NULL;
	PhysicsEngine::instance()->ReleaseActor(m_actor);
}

void CPhysicsEntity::Update(float dt, int level)
{

	CEntity::Update ( dt, level );	

	if (core.Scene()->IsRunning() && m_actor)
	{
		if (m_type == eDynamyc)
		{
			m_state.origin = (IVector&)m_actor->GetPosition();
			m_state.quat = (IQuaternion&)m_actor->GetOrientation();
		}
		else
		{
			m_actor->setPosition((Vector3&)m_state.origin);
			m_actor->setOrientation((Quaternion&)m_state.quat);
		}
	}

	
}

void CPhysicsEntity::OnContact(CPhysicsEntity* _physEnity2, enumPhysContactPairFlag _event)
{
	

}

/*

void CreateMd3FromMd1(byte* buf, char *fullPath)
{

	md1Header_t  *header = (md1Header_t*)buf;

	static std::vector<Vector3> vertexesVector;
	vertexesVector.reserve( 5000 );
	vertexesVector.resize(0);
	
	if (header->version == MD1_VERSION)
	{
		md1Info_t    *info   = (md1Info_t*)&buf[sizeof(md1Header_t)];
		//md1Surface_t *surfaces = (md1Surface_t*)&buf[sizeof(md1Header_t) + sizeof(md1Info_t)];
		md1Vertex_t  *vertex = (md1Vertex_t*)&buf[sizeof(md1Header_t) + sizeof(md1Info_t) + sizeof(md1Surface_t)*info->numSurfaces];

		for (int i = 0; i < info->numVerts; i++)
		{
			vertex_t dst;

			// decompress vertex
			float x = info->posScale[0] * vertex[i].xyz[0]/ 32767.0f;
			float y = info->posScale[1] * vertex[i].xyz[1]/ 32767.0f;
			float z = info->posScale[2] * vertex[i].xyz[2]/ 32767.0f;


			dst.xyz[0] = x; 
			dst.xyz[1] = y;
			dst.xyz[2] = z; 


			vertexesVector.push_back(Vector3(dst.xyz[0], dst.xyz[1], dst.xyz[2]));

			//fprintf(plog," static actor vertex '%f', '%f', '%f' \n", dst.xyz[0], dst.xyz[1], dst.xyz[2]);

		}

		UINT size = vertexesVector.size();
		std::vector<Vector3> uniqueVertexes;
		
		std::map<Vector3, UINT> mapVertexes;
		//UINT vertexId = 0;
		for (UINT i = 0; i < size; ++i)
		{
			Vector3& v = vertexesVector[i];
			std::vector<Vector3>::iterator it = std::find(uniqueVertexes.begin(), uniqueVertexes.end(), v );

			if (it == uniqueVertexes.end())
			{
				uniqueVertexes.push_back(v);
			}
		}
		
		std::vector<int> indexes;
		indexes.reserve(size);
		

		//vertexes, 3*float * numVertexes
		for (UINT i = 0; i < size; ++i)
		{
			//std::map<Vector3, int>::iterator it = uniqueVertexes.find(vertexesVector[i]);
			indexes.push_back( find(uniqueVertexes, vertexesVector[i]) );

			
			//indexes.push_back((*it).second ); 
		}
		UINT sizeUnique = uniqueVertexes.size();

		
		float boundingRadius = 0;

		for (UINT i = 0; i < sizeUnique; ++i)
		{
			float length = uniqueVertexes[i].LengthSquared();
			if (length > boundingRadius)
				boundingRadius = length;
		}

		UINT numTriangles = size/3;

		 //3. numVertexes, unsigned int
		 //4. numTriangles, unsigned int
		 //5. vertexes, (NULL) float* , must refer to the  adress
		 //6. transformedVertexes, (NULL) USHORT* , must refer to the  adress
		 //7. indexes, (NULL) USHORT* , must refer to the  adress
		 //8. vertexes, 3*float * numVertexes
		 //9. transformedVertexes, 3*float * numVertexes
		 //10.indexes, 3*USHORT * numTriangles  
		 // for alignment
		 //if (numTriangles%2 != 0)
		 //		USHORT,0

		//write .md3
		
		char* ext = strchr( fullPath, '.');
		*ext = '\0';
		CFileSystem::FSHANDLE hf = CFileSystem::instance()->openFile( fullPath, "md3", NULL, CFileSystem::g_AccessWrite );

		// shapeType
		int shapeType = eMesh;
		CFileSystem::instance()->writeFile( hf, &shapeType, sizeof(int) );
		// 3. boundingRadius, float
		float radius = sqrt(boundingRadius);
		CFileSystem::instance()->writeFile( hf, &radius, sizeof(float) );
		//3. numVertexes, unsigned int
		CFileSystem::instance()->writeFile( hf, &sizeUnique, sizeof(UINT) );
		// 4. numTriangles, unsigned int
		CFileSystem::instance()->writeFile( hf, &numTriangles, sizeof(UINT) );
		//5. vertexes, (NULL) float* , must refer to the  adress
		UINT fake = 0;
		CFileSystem::instance()->writeFile( hf, &fake, sizeof(UINT) );
		//6. transformedVertexes, (NULL) USHORT* , must refer to the  adress
		CFileSystem::instance()->writeFile( hf, &fake, sizeof(UINT) );
		//7. indexes, (NULL) USHORT* , must refer to the  adress
		CFileSystem::instance()->writeFile( hf, &fake, sizeof(UINT) );





		//8. vertexes, 3*float * numVertexes
		for (UINT i = 0; i < sizeUnique; ++i)
		{
			LogPrintf("model vertex %f, %f, %f' \n",
			uniqueVertexes[i].x, uniqueVertexes[i].y, uniqueVertexes[i].z);

			CFileSystem::instance()->writeFile( hf, &uniqueVertexes[i].x, sizeof(float) );
			CFileSystem::instance()->writeFile( hf, &uniqueVertexes[i].y, sizeof(float) );
			CFileSystem::instance()->writeFile( hf, &uniqueVertexes[i].z, sizeof(float) );
		}

		//9. transformedVertexes, 3*float * numVertexes
		for (UINT i = 0; i < sizeUnique; ++i)
		{
			CFileSystem::instance()->writeFile( hf, &fake, sizeof(float) );
			CFileSystem::instance()->writeFile( hf, &fake, sizeof(float) );
			CFileSystem::instance()->writeFile( hf, &fake, sizeof(float) );
		}



		//10.indexes, 3*USHORT * numTriangles  
		for (UINT i = 0; i < size; ++i)
		{
			CFileSystem::instance()->writeFile( hf, &indexes[i], sizeof(USHORT) );
		}

		// for alignment
		if (numTriangles%2 != 0)
			CFileSystem::instance()->writeFile( hf, &fake, sizeof(USHORT) );

		
		
		//numMaterials, unsigned char ,crashs due to alignment
		//numMaterials, unsigned int
		CFileSystem::instance()->writeFile( hf, &info->numSurfaces, sizeof(UINT) );

		if (info->numSurfaces == 1)
		{
			//materialName , char[32]
			UINT matID = OtherMaterial;


			// materialName , char[32]
			CFileSystem::instance()->writeFile( hf, &matID, sizeof(UINT) );

		}

		CFileSystem::instance()->closeFile(hf);
	}
	else
		assert(!"old md1");
}
*/

