#ifndef PHYSICSSUBSYSTEM_H
#define PHYSICSSUBSYSTEM_H

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

class PhysicsSubsystem
{
    btDefaultCollisionConfiguration*        mCollisionConfiguration;
    btCollisionDispatcher*                  mCollisionDispatcher;
    btBroadphaseInterface*                  mOverlappingPairCache;
    btSequentialImpulseConstraintSolver*    mSolver;
    btDiscreteDynamicsWorld*                mDynamicsWorld;
    btGhostPairCallback*                    mGhostPairCallback;
    btAlignedObjectArray<btCollisionShape*> mCollisionShapes;

public:
    PhysicsSubsystem();

    void onCreateMicroState();
};

#endif // PHYSICSSUBSYSTEM_H
