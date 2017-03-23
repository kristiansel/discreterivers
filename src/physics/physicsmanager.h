#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include "physicssimulation.h"
#include "../common/pointer.h"
#include "../engine/phystransformcontainer.h"
#include "eulerphysicssim.h"

class PhysicsManager
{
    Ptr::OwningPtr<PhysicsSimulation> mPhysicsSimPtr;
    Ptr::WritePtr<PhysTransformContainer> mActorTransformsPtr;

    //Ptr::OwningPtr<Euler::PhysicsSim> mEulerPhysicsSimPtr;

    PhysicsManager() = delete;
public:
    PhysicsManager(Ptr::WritePtr<PhysTransformContainer> actor_transforms_ptr);

    inline void stepPhysicsSimulation(float delta_time_sec);
};

inline void PhysicsManager::stepPhysicsSimulation(float delta_time_sec)
{
    if (mPhysicsSimPtr) { mPhysicsSimPtr->stepSimulation(delta_time_sec); }
}

#endif // PHYSICSMANAGER_H
