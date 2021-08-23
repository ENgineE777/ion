
#include "PhysController.h"

//extern KinematicCharacterController m_character;

PhysController::PhysController(PhysicsService* srv)
{
	service = srv;
	bActive = false;
	radius = 0.5f;
	height = 2.0f;	
	move_dir = 0.0f;

	btBody = NULL;
	//m_shape = NULL;
	//m_collisionObject = NULL;
}

void PhysController::Activate(bool act, Matrix mat)
{
	if (bActive == act) return;

	bActive = act;	
	
	if (act)
	{		
		float hgt = height - 2*radius;
		if (hgt<0) hgt = 0.01f;

		/*btVector3 spherePositions[2];
		btScalar sphereRadii[2];

		sphereRadii[0] = radius;
		sphereRadii[1] = radius;
		spherePositions[0] = btVector3 (0.0, (height/btScalar(2.0) ), 0.0);
		spherePositions[1] = btVector3 (0.0, (-height/btScalar(2.0)), 0.0);		

		m_shape = new btMultiSphereShape (btVector3(radius/btScalar(2.0), height/btScalar(2.0), radius/btScalar(2.0)), &spherePositions[0], &sphereRadii[0], 2);
		//m_stepHeight = stepHeight;
		//m_height = height;
		//m_width = width;
		
		btTransform trans;
		trans.setFromOpenGLMatrix(mat.matrix);
		
		//btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		//btRigidBody::btRigidBodyConstructionInfo cInfo(1.0, myMotionState, m_shape);
		m_collisionObject = new btCollisionObject ();

		m_collisionObject->setWorldTransform(trans);
		m_collisionObject->setCollisionShape (m_shape);
		//m_collisionObject->setCollisionFlags (btCollisionObject::CF_NO_CONTACT_RESPONSE);

		m_currentPosition = trans.getOrigin();
		m_targetPosition  = trans.getOrigin();*/
		
		colShape = new btCapsuleShape(btScalar(radius),btScalar(hgt));

		btTransform trans;
		trans.setFromOpenGLMatrix(mat.matrix);

		btVector3 localInertia(0,0,0);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f,myMotionState,colShape,localInertia);
		btBody = new btRigidBody(rbInfo);

		btBody->setCollisionFlags( btBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		btBody->setActivationState(DISABLE_DEACTIVATION);		

		//add the body to the dynamics world
		service->dynamicsWorld->addRigidBody(btBody);
	}
	else
	{
		ReleaseController();	
	}
}

void PhysController::SetTransform(Matrix transform)
{

}

void PhysController::GetTransform(Matrix& transform)
{			
	/*if (m_collisionObject)
	{
		btTransform trans = m_collisionObject->getInterpolationWorldTransform();
		trans.getOpenGLMatrix(transform.matrix);
	}*/

	if (btBody && btBody->getMotionState())	
	{
		btTransform trans;
		btBody->getMotionState()->getWorldTransform(trans);

		trans.getOpenGLMatrix(transform.matrix);
	}
}

class ClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	ClosestNotMeConvexResultCallback (btCollisionObject* me) : btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	{
		m_me = me;
	}

	virtual btScalar AddSingleResult(btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == m_me)
			return 1.0;

		//return ClosestConvexResultCallback::AddSingleResult (convexResult, normalInWorldSpace);
		return 1.0;
	}
protected:
	btCollisionObject* m_me;
};

btVector3 computeReflectionDirection (const btVector3& direction, const btVector3& normal)
{
	return direction - (btScalar(2.0) * direction.dot(normal)) * normal;
}

/*
* Returns the portion of 'direction' that is parallel to 'normal'
*/
btVector3 parallelComponent (const btVector3& direction, const btVector3& normal)
{
	btScalar magnitude = direction.dot(normal);
	return normal * magnitude;
}

/*
* Returns the portion of 'direction' that is perpindicular to 'normal'
*/
btVector3 perpindicularComponent (const btVector3& direction, const btVector3& normal)
{
	return direction - parallelComponent(direction, normal);
}


void PhysController::updateTargetPositionBasedOnCollision (const btVector3& hitNormal, btScalar tangentMag, btScalar normalMag)
{
/*	btVector3 movementDirection = m_targetPosition - m_currentPosition;
	btScalar movementLength = movementDirection.length();
	if (movementLength>SIMD_EPSILON)
	{
		movementDirection.normalize();

		btVector3 reflectDir = computeReflectionDirection (movementDirection, hitNormal);
		reflectDir.normalize();

		btVector3 parallelDir, perpindicularDir;

		parallelDir = parallelComponent (reflectDir, hitNormal);
		perpindicularDir = perpindicularComponent (reflectDir, hitNormal);

		m_targetPosition = m_currentPosition;
		if (0)//tangentMag != 0.0)
		{
			btVector3 parComponent = parallelDir * btScalar (tangentMag*movementLength);
			//			printf("parComponent=%f,%f,%f\n",parComponent[0],parComponent[1],parComponent[2]);
			m_targetPosition +=  parComponent;
		}

		if (normalMag != 0.0)
		{
			btVector3 perpComponent = perpindicularDir * btScalar (normalMag*movementLength);
			//			printf("perpComponent=%f,%f,%f\n",perpComponent[0],perpComponent[1],perpComponent[2]);
			m_targetPosition += perpComponent;
		}
	} else
	{
		//		printf("movementLength don't normalize a zero vector\n");
	}*/
}

void PhysController::MoveTo(Vector dir)
{
	//m_character.mvDir = btVector3(dir.x, dir.y, dir.z);

	/*
	move_dir = dir;	

	//if (btBody && btBody->getMotionState())
	if (m_collisionObject)
	{	
		{
			// phase 1: up
			btTransform start, end;
			m_targetPosition = m_currentPosition + btVector3 (btScalar(0.0), 0.2f, btScalar(0.0));

			start.setIdentity ();
			end.setIdentity ();
			
			start.setOrigin (m_currentPosition + btVector3(btScalar(0.0), btScalar(0.1), btScalar(0.0)));
			end.setOrigin (m_targetPosition);

			ClosestNotMeConvexResultCallback callback (m_collisionObject);

			service->dynamicsWorld->convexSweepTest (m_shape, start, end, callback);

			if (callback.HasHit())
			{
				// we moved up only a fraction of the step height
				//m_currentStepOffset = m_stepHeight * callback.m_closestHitFraction;
				m_currentPosition.setInterpolate3 (m_currentPosition, m_targetPosition, callback.m_closestHitFraction);
			}
			else
			{
				//m_currentStepOffset = m_stepHeight;
				m_currentPosition = m_targetPosition;
			}
		}

		btVector3 originalDir = btVector3(move_dir.x,move_dir.y,move_dir.z);
		originalDir.normalize();

		btScalar fraction = 1.0;
		btScalar distance2 = 1.0f;	

		btTransform trans =	m_collisionObject->getInterpolationWorldTransform();

		btTransform start, end;
		//btVector3 m_currentPosition = trans.getOrigin();
		m_targetPosition = m_targetPosition + btVector3(move_dir.x,move_dir.y,move_dir.z);
		start.setIdentity ();
		end.setIdentity ();

		int maxIter = 10;

		while (fraction > btScalar(0.01) && maxIter-- > 0)
		{
			start.setOrigin (m_currentPosition);
			end.setOrigin (m_targetPosition);

			ClosestNotMeConvexResultCallback callback (m_collisionObject);

			//btScalar margin = m_shape->getMargin();
			//m_shape->setMargin(margin - 0.06f);
			service->dynamicsWorld->convexSweepTest (m_shape, start, end, callback);
			//m_shape->setMargin(margin);

			fraction -= callback.m_closestHitFraction;

			if (callback.HasHit())
			{	
				// we moved only a fraction
				btScalar hitDistance = (callback.m_hitPointWorld - m_currentPosition).length();
				
				if (hitDistance<0.f)
				{
					//				printf("neg dist?\n");
				}
				
				if (hitDistance > 0.05)
				{					
					m_currentPosition.setInterpolate3 (m_currentPosition, m_targetPosition, callback.m_closestHitFraction);
				}
				
				btVector3 currentDir = m_targetPosition - m_currentPosition;
				distance2 = currentDir.length2();
				if (distance2 > SIMD_EPSILON)
				{
					currentDir.normalize();					
					if (currentDir.dot(originalDir) <= btScalar(0.0))
					{
						break;
					}
				}
				else
				{
					//				printf("currentDir: don't normalize a zero vector\n");
					break;
				}
			}
			else
			{
				// we moved whole way
				m_currentPosition = m_targetPosition;
			}

			//	if (callback.m_closestHitFraction == 0.f)
			//		break;

		}
	}
	*/

	if (btBody)
	{		
		btTransform trans;
		btBody->getMotionState()->getWorldTransform(trans);
		
		trans.getOrigin().setX(trans.getOrigin().getX() + dir.x);
		trans.getOrigin().setY(trans.getOrigin().getY() + dir.y);
		trans.getOrigin().setZ(trans.getOrigin().getZ() + dir.z);

		btBody->getMotionState()->setWorldTransform(trans);
	}
}

void PhysController::GetPos(Vector& pos)
{
	//pos = Vector(m_currentPosition.getX(),
	//			 m_currentPosition.getY(),
	//			 m_currentPosition.getZ());
}

void PhysController::SetRadius(float r)
{
	radius = r;

}

void PhysController::SetHeight(float h)
{
	height = h;
}

void* PhysController::GetData()
{
	return NULL;
}

void PhysController::Release()
{
	ReleaseController();

	delete this;
}

void PhysController::ReleaseController()
{	
	/*if (m_collisionObject)
	{
		delete m_shape;
		m_shape = NULL;
		
		delete m_collisionObject;		
		m_collisionObject = NULL;
	}*/

	if (btBody)
	{
		if (btBody->getMotionState())
		{
			delete btBody->getMotionState();
		}

		service->dynamicsWorld->removeCollisionObject( btBody );
		delete btBody;
		btBody = NULL;

		delete colShape;
	}
}