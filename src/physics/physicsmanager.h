#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#include "physicssimulation.h"
#include "../common/pointer.h"
#include "../engine/phystransformcontainer.h"
#include "eulerphysicssim.h"

#include "../state/scenecreationinfo.h"
#include "../common/freelistset.h"
#include "../appconstraints.h"

class PhysicsManager
{
    RigidBodyPool mActorRigidBodies; // needs to outlive physics sim...
    RigidBodyPool mStaticsRigidBodies; // needs to outlive physics sim...

    PhysicsSimulation mPhysicsSim;
    Ptr::WritePtr<PhysTransformContainer> mActorTransformsPtr;

    //Ptr::OwningPtr<Euler::PhysicsSim> mEulerPhysicsSimPtr;


    PhysicsManager() = delete;
public:
    PhysicsManager(Ptr::WritePtr<PhysTransformContainer> actor_transforms_ptr);

    inline void stepPhysicsSimulation(float delta_time_sec);

    void initScene(const vmath::Vector3 &land_point,
                   Ptr::ReadPtr<state::MacroState> scene_data,
                   const std::vector<state::Actor> &actors);

    void updateDynamicsGravity(const AltPlanet::Shape::BaseShape *planet_shape);

    Ptr::WritePtr<RigidBodyPool> getActorRigidBodyPoolWPtr() { return Ptr::WritePtr<RigidBodyPool>(&mActorRigidBodies); }

    static float constexpr sGravityMagnitude = 9.81f;
};

inline void PhysicsManager::stepPhysicsSimulation(float delta_time_sec)
{
    mPhysicsSim.stepSimulation(delta_time_sec);

    stdext::freelist_set_zip(mActorRigidBodies, mActorTransformsPtr.getRef(),
                             [](btRigidBody &rb, PhysTransform &pt) {
                                 btTransform rb_transf = rb.getWorldTransform();
                                 btVector3 rb_pos = rb_transf.getOrigin();
                                 btQuaternion rb_rot = rb_transf.getRotation();
                                 pt.pos = vmath::Vector3(rb_pos.getX(), rb_pos.getY(), rb_pos.getZ());
                                 pt.rot = vmath::Quat(rb_rot.getX(), rb_rot.getY(), rb_rot.getZ(), rb_rot.getW());
                             });

    //if (mEulerPhysicsSimPtr) { mEulerPhysicsSimPtr->stepSimulation(delta_time_sec); }
}

#endif // PHYSICSMANAGER_H
