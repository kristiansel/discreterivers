#include "physicsmanager.h"

#include "../events/immediateevents.h"
#include "../common/macro/debuglog.h"

PhysicsManager::PhysicsManager(Ptr::WritePtr<PhysTransformContainer> actor_transforms_ptr) :
    mActorTransformsPtr(actor_transforms_ptr),
    mPhysicsSim(Ptr::WritePtr<RigidBodyPool>(&mActorRigidBodies),
                Ptr::WritePtr<RigidBodyPool>(&mStaticsRigidBodies))//,
    //mEulerPhysicsSimPtr(nullptr)
{
    // ctor...
}

void PhysicsManager::initScene(const vmath::Vector3 &land_point,
               Ptr::ReadPtr<state::MacroState> scene_data,
               const std::vector<state::Actor> &actors)
{
    Ptr::ReadPtr<state::MacroState> macro_state_ptr = scene_data;
    const AltPlanet::Shape::BaseShape *planet_shape = macro_state_ptr->planet_base_shape;

    // set gravity
    vmath::Vector3 grad_dir = planet_shape->getGradDir(land_point);
    vmath::Vector3 gravity = -9.81f * vmath::normalize(grad_dir);
    mPhysicsSim.setGravity(gravity);


    // add static collision object for world
    mPhysicsSim.addStaticBodyMesh(vmath::Vector3(0.0f, 0.0f, 0.0f), vmath::Quat(0.0f, 0.0f, 0.0f, 1.0f),
                                      macro_state_ptr->alt_planet_points, macro_state_ptr->alt_planet_triangles);

   /* mPhysicsSim.addStaticBodyPlane(land_point, vmath::Quat(0.0f, 0.0f, 0.0f, 1.0f),
                                       vmath::normalize(grad_dir));*/

    // add rigidbody for actors
    for (int i = 0; i<actors.size(); i++)
    {
        const state::Actor &actor = actors[i];
        auto type = actor.spec.type;
        PhysicsSimulation::Shape shape = type==state::Actor::Spec::Type::TestBox ?
                                            PhysicsSimulation::Shape::Box :
                                            PhysicsSimulation::Shape::Sphere;
        mPhysicsSim.addDynamicBody(actor.pos, actor.rot, shape);
    }


}

void PhysicsManager::updateDynamicsGravity(const AltPlanet::Shape::BaseShape *planet_shape)
{
    mActorRigidBodies.for_all([&](btRigidBody &rb){
        btVector3 rb_pos = rb.getWorldTransform().getOrigin();
        vmath::Vector3 gravity = -sGravityMagnitude * vmath::normalize(planet_shape->getGradDir((vmath::Vector3&)(rb_pos)));
        rb.setGravity((btVector3&)(gravity));
    });
}
