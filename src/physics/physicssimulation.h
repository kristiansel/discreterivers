#ifndef PHYSICSSIMULATION_H
#define PHYSICSSIMULATION_H

#include <list>

#include "../common/gfx_primitives.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../common/freelistset.h"
#include "../appconstraints.h"
#include "../common/pointer.h"

// to be removed
#include "../common/procedural/planegeometry.h"

using RigidBodyPool = stdext::freelist_set<btRigidBody, appconstraints::n_actors_max>;
using RigidBodyPoolHandle = typename RigidBodyPool::node*;

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

    Ptr::WritePtr<RigidBodyPool> mActorRigidBodiesPtr;
    Ptr::WritePtr<RigidBodyPool> mStaticRigidBodiesPtr;

public:
    PhysicsSimulation(Ptr::WritePtr<RigidBodyPool> actor_rigid_bodies_ptr,
                      Ptr::WritePtr<RigidBodyPool> static_rigid_bodies_ptr);
    ~PhysicsSimulation();

    enum class Shape { Box, Sphere };

    void setGravity(const vmath::Vector3 &g);
    void addDynamicBody(const vmath::Vector3 &pos, const vmath::Quat &rot, Shape shape);
    void addStaticBodyMesh(const vmath::Vector3 &pos, const vmath::Quat &rot,
                           const std::vector<vmath::Vector3> &points, const std::vector<gfx::Triangle> &triangles);

    void addStaticBodyPlane(const vmath::Vector3 &pos, const vmath::Quat &rot,
                            const vmath::Vector3 &normal);

    inline void stepSimulation(float time_delta_sec);
private:
    btCollisionShape * createMeshShape(const std::vector<vmath::Vector4> &pts, const std::vector<gfx::Triangle> &tris);
};

inline void PhysicsSimulation::stepSimulation(float time_delta_sec)
{
    //mDynamicsWorld->stepSimulation(time_delta_sec);
    mDynamicsWorld->stepSimulation(time_delta_sec, 3, 1.f/120.f); // need to do substeps for stability...
}


#endif // PHYSICSSUBSYSTEM_H
