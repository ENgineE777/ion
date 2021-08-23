
#include "PhysicsService.h"
#include "PhysObject.h"
#include "PhysController.h"
#include "..\core.h"


class DebugDrawer : public btIDebugDraw
{
	int m_debugMode;

public:

	DebugDrawer() {};

	virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color);

	virtual void	drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha);

	virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) {};

	virtual void	reportErrorWarning(const char* warningString) {};

	virtual void	draw3dText(const btVector3& location,const char* textString) {};

	virtual void	setDebugMode(int debugMode) { m_debugMode = debugMode; };

	virtual int		getDebugMode() const { return m_debugMode;}
};

void DebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{	
	Vector p1(from.getX(),from.getY(),from.getZ());
	Vector p2(to.getX(),to.getY(),to.getZ());
	//Renderer.DrawLine(p1,p2,0xff0000ff);
					  //D3DCOLOR_ARGB(255,(byte)(255*color.x),(byte)(255*color.y),(byte)(255*color.z)));
}

void DebugDrawer::drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha)
{
	drawLine(a,b, color);
	drawLine(b,c, color);
	drawLine(a,c, color);
}

DebugDrawer dbgDrawer;

PhysicsService::PhysicsService() : IPhysicsService()
{
}

bool PhysicsService::Init()
{
	core->AddTask(this,this, (TaskHolder::Task)&PhysicsService::Work,-20, -1.0f);
	core->AddTask(this,this, (TaskHolder::Task)&PhysicsService::DebugDraw,10, -1.0f);
		
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///the maximum size of the collision world. Make sure objects stay within these boundaries
	///Don't make the world AABB size too large, it will harm simulation quality and performance
	btVector3 worldAabbMin(-10000,-10000,-10000);
	btVector3 worldAabbMax(10000,10000,10000);
	int	maxProxies = 1024;
	overlappingPairCache = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0,-10,0));

	dynamicsWorld->setDebugDrawer(&dbgDrawer);		

	//dbgDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);	

	return true;
}

IPhysObject* PhysicsService::CreatePhysObject(Matrix transform)
{
	return new PhysObject(this,transform);
}

IPhysController* PhysicsService::CreatePhysConytoller()
{
	return new PhysController(this);
}

void PhysicsService::Work(float fDelta,int level)
{
	dynamicsWorld->stepSimulation(fDelta,10);
}

void PhysicsService::DebugDraw(float fDelta,int level)
{
	/*if (DirectInput.IsKeyPressed(DIK_N,KeyImmediate))
	{
		dbgDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);	
	}
	else
	{
		dbgDrawer.setDebugMode(0);	
	}*/
	
	dynamicsWorld->debugDrawWorld();
}

float PhysicsService::RayCast(Vector from, Vector to, Vector& cp, Vector& cn, Vector& vl, void* data)
{	
	struct  ClosestRayResultCallback : public btCollisionWorld::RayResultCallback
	{
		ClosestRayResultCallback(const btVector3& rayFromWorld,const btVector3& rayToWorld,btCollisionObject* m_collisionObjectF)
		{
			m_rayFromWorld = rayFromWorld;
			m_rayToWorld = rayToWorld;
			m_collisionObject = NULL;
			m_collisionObjectFlt = m_collisionObjectF;
		}
		
		btVector3       m_rayFromWorld;//used to calculate hitPointWorld from hitFraction
		btVector3       m_rayToWorld;
				
		btVector3       m_hitNormalWorld;
		btVector3       m_hitPointWorld;
			
		btCollisionObject* m_collisionObject;
		btCollisionObject* m_collisionObjectFlt;		
			
		virtual btScalar AddSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
		{
			//caller already does the filter on the m_closestHitFraction
			//assert(rayResult.m_hitFraction != m_collisionObjectFlt);
			assert(rayResult.m_hitFraction <= m_closestHitFraction);
					
			m_closestHitFraction = rayResult.m_hitFraction;
			m_collisionObject = rayResult.m_collisionObject;			

			if (normalInWorldSpace)
			{
				m_hitNormalWorld = rayResult.m_hitNormalLocal;
			}
			else
			{
				m_hitNormalWorld = m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;
			}
			m_hitPointWorld.setInterpolate3(m_rayFromWorld,m_rayToWorld,rayResult.m_hitFraction);
			return rayResult.m_hitFraction;
		}
	};


	btCollisionWorld::ClosestRayResultCallback rayCallback(btVector3(from.x,from.y,from.z),btVector3(to.x,to.y,to.z));
	//ClosestRayResultCallback rayCallback(btVector3(from.x,from.y,from.z),btVector3(to.x,to.y,to.z),(btCollisionObject*)data);
	dynamicsWorld->rayTest(btVector3(from.x,from.y,from.z),btVector3(to.x,to.y,to.z),rayCallback);
	
	

	/*if (rayCallback.HasHit())
	{
		cn = Vector(rayCallback.m_hitNormalWorld.getX(),
					 rayCallback.m_hitNormalWorld.getY(),
				 	 rayCallback.m_hitNormalWorld.getZ());

		cp = Vector(rayCallback.m_hitPointWorld.getX(),
					 rayCallback.m_hitPointWorld.getY(),
					 rayCallback.m_hitPointWorld.getZ());

		btVector3 v = rayCallback.m_collisionObject->getInterpolationLinearVelocity();
		vl = Vector(v.getX(),v.getY(),v.getZ());
		
		return (cp - from).GetLength();
	}*/

	return -1.0f;
}

void PhysicsService::Release()
{	
	core->DelTask(this,(PhysicsService::Task)&PhysicsService::Work);
	core->DelTask(this,(PhysicsService::Task)&PhysicsService::DebugDraw);

	delete dynamicsWorld;	
	delete solver;	
	delete overlappingPairCache;	
	delete dispatcher;
	delete collisionConfiguration;
}