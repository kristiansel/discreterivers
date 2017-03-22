#include "physicsmanager.h"

#include "../events/immediateevents.h"
#include "../common/macro/debuglog.h"

PhysicsManager::PhysicsManager(Ptr::WritePtr<PhysTransformContainer> actor_transforms_ptr) :
    mActorTransformsPtr(actor_transforms_ptr),
    mPhysicsSimPtr(nullptr)
{
    // register a bunch of callbacks
    events::Immediate::add_callback<events::StartGameEvent>(
        [this] (const events::StartGameEvent &evt) {

            mPhysicsSimPtr = Ptr::OwningPtr<PhysicsSimulation>(new PhysicsSimulation());

    });

    events::Immediate::add_callback<events::CreateSceneEvent>(
        [this] (const events::CreateSceneEvent &evt) {
            Ptr::ReadPtr<state::MacroState> macro_state_ptr = evt.scene_creation_info->macro_state_ptr;
            const AltPlanet::Shape::BaseShape *planet_shape = macro_state_ptr->planet_base_shape;

            // set gravity
            vmath::Vector3 grad_dir = planet_shape->getGradDir(evt.scene_creation_info->anchor_pos);
            //vmath::Vector3 gravity = -9.81f * vmath::normalize(grad_dir);
            vmath::Vector3 gravity = -1.0f * vmath::normalize(grad_dir);
            mPhysicsSimPtr->setGravity(gravity);

            // add rigidbody for actors
            for (int i = 0; i<evt.scene_creation_info->actors.size(); i++)
            {
                const state::ActorCreationInfo &actor = evt.scene_creation_info->actors[i];
                mPhysicsSimPtr->addDynamicBody(actor.pos, actor.rot, mActorTransformsPtr->get_by_offset(i));
            }


            // add static collision object for world
            /*mPhysicsSimPtr->addStaticBodyMesh(vmath::Vector3(0.0f, 0.0f, 0.0f), vmath::Quat(0.0f, 0.0f, 0.0f, 1.0f),
                                              macro_state_ptr->alt_lake_points, macro_state_ptr->alt_lake_triangles);*/

            mPhysicsSimPtr->addStaticBodyPlane(evt.scene_creation_info->land_pos, vmath::Quat(0.0f, 0.0f, 0.0f, 1.0f),
                                               vmath::normalize(grad_dir));
    });
}
