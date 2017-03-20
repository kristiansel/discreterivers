#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include "../common/pointer.h"
#include "physicssimulation.h"

class PhysicsManager
{
    Ptr::OwningPtr<PhysicsSimulation> mPhysicsSimPtr;
public:
    PhysicsManager();

    inline void stepPhysicsSimulation(float delta_time_sec);
};

inline void PhysicsManager::stepPhysicsSimulation(float delta_time_sec)
{
    if (mPhysicsSimPtr) { mPhysicsSimPtr->stepSimulation(delta_time_sec); }
}

#endif // PHYSICSMANAGER_H
