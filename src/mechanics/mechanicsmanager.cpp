#include "mechanicsmanager.h"

#include "../common/macro/debuglog.h"
#include "../events/immediateevents.h"

MechanicsManager::MechanicsManager(Ptr::WritePtr<gfx::Camera> camera_ptr,
                                   Ptr::WritePtr<RigidBodyPool> actor_rigid_body_pool_ptr) :
    //mActiveInputCtrl( new mech::CameraController(camera_ptr) ),
    mActiveInputCtrl( nullptr ),
    mActorRigidBodyPoolPtr(actor_rigid_body_pool_ptr)
{
    // ctor
}

void MechanicsManager::initScene(const vmath::Vector3 &point_above,
                                 Ptr::ReadPtr<state::MacroState> scene_data,
                                 const std::vector<state::Actor> &actors)

{
    vmath::Vector3 local_up = scene_data->getLocalUp(point_above);

    /*mech::CameraController *cam_ctrl = dynamic_cast<mech::CameraController*>(mActiveInputCtrl);
    if (cam_ctrl)
    {
        DEBUG_LOG("setting cam ctrler up direction");
        cam_ctrl->setUpDir(local_up);
    }
    else
    {
        DEBUG_LOG("active input controller is not a camera controller");
    }*/

    bool found_atleast_one_player = false;
    for (int i = 0; i<actors.size(); i++)
    {
        const state::Actor &actor = actors[i];
        if (actor.control == state::Actor::Control::Player)
        {
            delete mActiveInputCtrl;
            auto arb = mActorRigidBodyPoolPtr->get_by_offset(i);
            mActiveInputCtrl = new mech::CharacterController(Ptr::WritePtr<RigidBody>(arb->get_ptr()));
            found_atleast_one_player = true;
        }
    }
    DEBUG_ASSERT(found_atleast_one_player);
}

MechanicsManager::~MechanicsManager()
{
    delete mActiveInputCtrl;
}
