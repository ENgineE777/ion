
#include "PhysObject.h"
#include "Core\core.h"

PhysObject::PhysObject(PhysicsService* srv, Matrix trans)
{
	service = srv;

	num_collisions = 0;	

	transform = trans;
	
	btBody = NULL;
	
	compShape = new btCompoundShape();

	gVertices = NULL;
	gIndices = NULL;
	triMesh = NULL;
	indexVertexArrays = NULL;
}

void PhysObject::AddBoxShape(Vector size,Matrix offset)
{
	colShape[num_collisions] = new btBoxShape(btVector3(btScalar(size.x * 0.5f),btScalar(size.y * 0.5f),btScalar(size.z * 0.5f)));	
	num_collisions++;

	btTransform trans;
	trans.setFromOpenGLMatrix(offset.matrix);

	compShape->addChildShape(trans,colShape[num_collisions-1]);
}

void PhysObject::AddSphereShape(float radius,Matrix offset)
{
	colShape[num_collisions] = new btSphereShape(btScalar(radius));
	num_collisions++;

	btTransform trans;
	trans.setFromOpenGLMatrix(offset.matrix);

	compShape->addChildShape(trans,colShape[num_collisions-1]);
}

void PhysObject::AddCapsuleShape(float height, float radius,Matrix offset)
{	
	float hgt = height - 2*radius;
	if (hgt<0) hgt = 0.01f;

	colShape[num_collisions] = new btCapsuleShape(btScalar(radius),btScalar(hgt));

	num_collisions++;

	btTransform trans;
	trans.setFromOpenGLMatrix(offset.matrix);

	compShape->addChildShape(trans,colShape[num_collisions-1]);
}

/*
void PhysObject::AddCollisionTree(IModel* model)
{		
	if (!model) return;

	EMDModel* EMD = ((Model*)model)->res->model;

	int vertStride = sizeof(btVector3);
	int indexStride = 3*sizeof(int);

	int totalVerts = 0;

	for (int k=0;k<(int)EMD->Meshes.size();k++)
	{	
		totalVerts += EMD->Meshes[k].Vertices.size();
	}

	int totalTriangles = 0;

	for (int k=0;k<(int)EMD->Meshes.size();k++)
	{			
		for (int i=0;i<(int)EMD->Meshes[k].SubMeshes.size();i++)
		{
			totalTriangles += EMD->Meshes[k].SubMeshes[i].NumFaces;		
		}
	}	

	gVertices = new btVector3[totalVerts];
	gIndices = new int[totalTriangles*3];
	
	int index = 0;

	Matrix mat;
	model->GetFullTransform(mat);
	Matrix tr = transform;
	tr.Inverse();
	mat = mat * tr;

	for (int k=0;k<(int)EMD->Meshes.size();k++)
	{	
		for (int i=0;i<(int)EMD->Meshes[k].Vertices.size();i++)
		{
			Vector pos(EMD->Meshes[k].Vertices[i].x,
					   EMD->Meshes[k].Vertices[i].y,
					   EMD->Meshes[k].Vertices[i].z);
		
			

			pos = pos * mat;
			
			gVertices[index].setX(pos.x);
			gVertices[index].setY(pos.y);
			gVertices[index].setZ(pos.z);

			index++;
		}
	}
	
	index = 0;

	for (int k=0;k<(int)EMD->Meshes.size();k++)
	{	
		for (int i=0;i<(int)EMD->Meshes[k].SubMeshes.size();i++)
		{
			for (int j=0;j<EMD->Meshes[k].SubMeshes[i].NumFaces;j++)
			{
				for (int f=0;f<3;f++)
				{
					gIndices[index++] = EMD->Meshes[k].SubMeshes[i].Vertex_Faces[j].index[f];
				}
			}				
		}
	}	
	
	indexVertexArrays = new btTriangleIndexVertexArray(totalTriangles,
													   gIndices,indexStride,totalVerts,
													   (btScalar*) &gVertices[0].x(),vertStride);

	triMesh = new btBvhTriangleMeshShape(indexVertexArrays,true);
}*/

void PhysObject::CompoudShape(bool isStatic)
{	
	float mass = 1.0f;
	if (isStatic) mass = 0.0f;
	bool isDynamic = (mass != 0.f);
	
	btTransform trans;	
	trans.setFromOpenGLMatrix(transform.matrix);
	
	

	btVector3 localInertia(0,0,0);
	if (!isStatic && !triMesh) compShape->calculateLocalInertia(mass,localInertia);
	if (!isStatic && triMesh)  triMesh->calculateLocalInertia(mass,localInertia);	

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);

	btCollisionShape* shape = compShape;
	
	if (triMesh)
	{		
		shape = triMesh;		
	}

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);		

	btBody = new btRigidBody(rbInfo);

	if (isStatic)
	{
		btBody->setCollisionFlags( btBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		btBody->setActivationState(DISABLE_DEACTIVATION);
	}

	//add the body to the dynamics world
	service->dynamicsWorld->addRigidBody(btBody);
}

void PhysObject::SetMass(float mass)
{

}

void PhysObject::SetTransform(Matrix transf)
{
	if (btBody && btBody->getMotionState())
	{
		btTransform trans;
		trans.setFromOpenGLMatrix(transf.matrix);
		btBody->getMotionState()->setWorldTransform(trans);		
	}
}

void PhysObject::GetTransform(Matrix& transf)
{	
	if (btBody && btBody->getMotionState())
	{
		btTransform trans;
		btBody->getMotionState()->getWorldTransform(trans);

		trans.getOpenGLMatrix(transf.matrix);
	}
}

void PhysObject::ApplyImpulse(Vector& imp)
{
	if (btBody)
	{
		btBody->applyCentralImpulse(btVector3(imp.x,imp.y,imp.z));
	}
}

void PhysObject::Release()
{
	if (btBody)
	{
		 if (btBody->getMotionState())
		 {
			 delete btBody->getMotionState();

		 }

		 service->dynamicsWorld->removeCollisionObject( btBody );
		 delete btBody;		
		 btBody = NULL;

		 delete compShape;
		 compShape = NULL;

		 if (num_collisions>0)
		 {
			 for (int i=0;i<num_collisions;i++)
			 {				 
				 delete colShape[i];
			 }			 
		 }

		 if (triMesh)
		 {			 
			 delete triMesh;
			 triMesh = NULL;

			 delete indexVertexArrays;
			 indexVertexArrays = NULL;

			 delete []gVertices;
			 delete []gIndices;
		 }
	}	

	delete this;
}
