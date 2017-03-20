#ifndef SPATIALINDEX3D_H
#define SPATIALINDEX3D_H

#include "../gfx_primitives.h"
#include "btBulletCollisionCommon.h"

class SpatialIndex3d
{
    btCollisionConfiguration* mCollisionConfiguration;
    btCollisionDispatcher* mCollisionDispatcher;
    btBroadphaseInterface* mBroadPhase;
    btCollisionWorld* mCollisionWorld;
    btAlignedObjectArray<btCollisionShape*> mCollisionShapes;

public:
    SpatialIndex3d();
    ~SpatialIndex3d();

    struct SphereShape
    {
        vmath::Vector3 center;
        float radius;
    };

    template<typename ShapeType>
    inline void addShape(int index, const ShapeType &shape);

    // some btDbvhBroadphase specific code for view frustum and occlusion queries...
    // ghost pair callback on pair cache??
};

template<>
inline void SpatialIndex3d::addShape<SpatialIndex3d::SphereShape>(int index, const SpatialIndex3d::SphereShape &sphere)
{
    btCollisionShape* sphere_collision_shape = new btSphereShape(sphere.radius);
    mCollisionShapes.push_back(sphere_collision_shape);

    btCollisionObject* sphere_collision_object = new btCollisionObject();
    sphere_collision_object->getWorldTransform().setOrigin(btVector3(sphere.center[0], sphere.center[1], sphere.center[2]));
    sphere_collision_object->setCollisionShape(sphere_collision_shape);

    mCollisionWorld->addCollisionObject(sphere_collision_object);
}

#endif // SPATIALINDEX3D_H
