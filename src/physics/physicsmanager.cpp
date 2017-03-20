#include "physicsmanager.h"

#include "../events/immediateevents.h"

PhysicsManager::PhysicsManager() :
    mPhysicsSimPtr(nullptr)
{
    // register a bunch of callbacks
    events::Immediate::add_callback<events::StartGameEvent>(
        [this] (const events::StartGameEvent &evt) {

        mPhysicsSimPtr = Ptr::OwningPtr<PhysicsSimulation>(new PhysicsSimulation());
    });
}
