#include "mechanicsmanager.h"

#include "../common/macro/debuglog.h"
#include "../events/immediateevents.h"

MechanicsManager::MechanicsManager(Ptr::WritePtr<gfx::Camera> camera_ptr,
                                   Ptr::WritePtr<RigidBodyPool> actor_rigid_body_pool_ptr) :
    //mActiveInputCtrl( new mech::CameraController(camera_ptr) ),
    mActiveInputCtrl( nullptr ),
    mPreviousInputCtrl( nullptr ),
    mActorRigidBodyPoolPtr(actor_rigid_body_pool_ptr),
    mCameraController(camera_ptr)
{
    // ctor
}

MechanicsManager::~MechanicsManager()
{
    //delete mActiveInputCtrl;
}

void MechanicsManager::initScene(const vmath::Vector3 &point_above,
                                 Ptr::ReadPtr<state::MacroState> scene_data,
                                 const std::vector<state::Actor> &actors)

{
    bool found_atleast_one_player = false;
    for (int i = 0; i<actors.size(); i++)
    {
        auto arb = mActorRigidBodyPoolPtr->get_by_offset(i);
        ControllerPoolHandle actor_ctrl = mControllers.create(Ptr::WritePtr<RigidBody>(arb->get_ptr()));

        const state::Actor &actor = actors[i];
        if (actor.control == state::Actor::Control::Player)
        {
            mActiveInputCtrl = actor_ctrl->get_ptr();
            found_atleast_one_player = true;
        }
    }
    DEBUG_ASSERT(found_atleast_one_player);
}

void MechanicsManager::toggleDebugFreeCam()
{
    DEBUG_LOG("TOGGLING DEBUG FREECAM");

    // if not freecam
    mech::CharacterController *char_contr = dynamic_cast<mech::CharacterController*>(mActiveInputCtrl);
    if (char_contr) // it is a character controller - not freecam
    {
        mPreviousInputCtrl = mActiveInputCtrl;
        mActiveInputCtrl = &mCameraController;
    }
    else  // if freecam - switch to character controller
    {
        mActiveInputCtrl = mPreviousInputCtrl;
        mPreviousInputCtrl = nullptr;
    }
}
