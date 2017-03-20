#ifndef PHYSICSSIMULATION_H
#define PHYSICSSIMULATION_H

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

class PhysicsSimulation
{
    btDefaultCollisionConfiguration*        mCollisionConfiguration;
    btCollisionDispatcher*                  mCollisionDispatcher;
    btBroadphaseInterface*                  mOverlappingPairCache;
    btSequentialImpulseConstraintSolver*    mSolver;
    btDiscreteDynamicsWorld*                mDynamicsWorld;
    btGhostPairCallback*                    mGhostPairCallback;
    btAlignedObjectArray<btCollisionShape*> mCollisionShapes;

public:
    PhysicsSimulation();
    ~PhysicsSimulation();

    inline void stepSimulation(float time_delta_sec);
};

inline void PhysicsSimulation::stepSimulation(float time_delta_sec)
{
    mDynamicsWorld->stepSimulation(time_delta_sec);
    //mDynamicsWorld->stepSimulation(time_delta_sec, 3, 1.f/120.f);
}


#endif // PHYSICSSUBSYSTEM_H
