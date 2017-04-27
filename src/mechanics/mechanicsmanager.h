#ifndef MECHANICSMANAGER_H
#define MECHANICSMANAGER_H

#include "cameracontroller.h"
#include "charactercontroller.h"
#include "thirdpersoncameracontroller.h"
#include "../state/scenecreationinfo.h"
#include "../physics/physicssimulation.h"
#include "../common/freelistset.h"
#include "../appconstraints.h"

class MechanicsManager
{
    //mech::CameraController              mCameraController;   // must be init after camera
    mech::InputController *mActiveInputCtrl;
    //mech::ThirdPersonCameraController   mThirdPersonCameraController;

    using ControllerPool = stdext::freelist_set<mech::CharacterController, appconstraints::n_actors_max>;
    using ControllerPoolHandle = typename ControllerPool::node*;

    ControllerPool mControllers;

    Ptr::WritePtr<RigidBodyPool> mActorRigidBodyPoolPtr;

public:
    MechanicsManager(Ptr::WritePtr<gfx::Camera> camera_ptr,
                     Ptr::WritePtr<RigidBodyPool> actor_rigid_body_pool_ptr);
    ~MechanicsManager();

    //void wireControlsToCamera(Ptr::WritePtr<gfx::Camera> camera_ptr);

    inline void update(float delta_time_sec);

    void initScene(const vmath::Vector3 &point_above,
                   Ptr::ReadPtr<state::MacroState> scene_data,
                   const std::vector<state::Actor> &actors);

    inline vmath::Quat getPlayerTargetOrientation();


    inline mech::InputController *getActiveController() { return mActiveInputCtrl; }
};

void MechanicsManager::update(float delta_time_sec)
{
    mActiveInputCtrl->update(delta_time_sec);
}

inline vmath::Quat MechanicsManager::getPlayerTargetOrientation()
{
    mech::CharacterController* player_controller = dynamic_cast<mech::CharacterController*>(mActiveInputCtrl);
    if (player_controller)
    {
        return player_controller->getTargetOrientation();
    }
    else
    {
        DEBUG_LOG("ERROR: active controller is not a character controller");
        DEBUG_ASSERT(false);
        return vmath::Quat(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

#endif // MECHANICSMANAGER_H
