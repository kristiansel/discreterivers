#ifndef PHYSICSSIMULATION_H
#define PHYSICSSIMULATION_H

#include "../common/gfx_primitives.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../engine/phystransformcontainer.h"

class PhysicsSimulation
{
    btDefaultCollisionConfiguration*        mCollisionConfiguration;
    btCollisionDispatcher*                  mCollisionDispatcher;
    btBroadphaseInterface*                  mOverlappingPairCache;
    btSequentialImpulseConstraintSolver*    mSolver;
    btDiscreteDynamicsWorld*                mDynamicsWorld;
    btGhostPairCallback*                    mGhostPairCallback;
    btAlignedObjectArray<btCollisionShape*> mCollisionShapes;

    // lives as long as the simulation, therefore should not keep adding
    // static data without quitting
    std::list<btTriangleMesh> mStaticMeshData;

public:
    PhysicsSimulation();
    ~PhysicsSimulation();

    void setGravity(const vmath::Vector3 &g);
    void addDynamicBody(const vmath::Vector3 &pos, const vmath::Quat &rot, PhysTransformNode * phys_tranf_node);
    void addStaticBodyMesh(const vmath::Vector3 &pos, const vmath::Quat &rot,
                           const std::vector<vmath::Vector3> &points, const std::vector<gfx::Triangle> &triangles);

    void addStaticBodyPlane(const vmath::Vector3 &pos, const vmath::Quat &rot,
                            const vmath::Vector3 &normal);

    inline void stepSimulation(float time_delta_sec);
};

inline void PhysicsSimulation::stepSimulation(float time_delta_sec)
{
    mDynamicsWorld->stepSimulation(time_delta_sec);
    //mDynamicsWorld->stepSimulation(time_delta_sec, 3, 1.f/120.f);
}


#endif // PHYSICSSUBSYSTEM_H
