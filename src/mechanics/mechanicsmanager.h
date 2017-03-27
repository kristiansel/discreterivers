#ifndef MECHANICSMANAGER_H
#define MECHANICSMANAGER_H

#include "cameracontroller.h"
#include "charactercontroller.h"
#include "thirdpersoncameracontroller.h"
#include "../state/scenecreationinfo.h"
#include "../physics/physicssimulation.h"

class MechanicsManager
{
    //mech::CameraController              mCameraController;   // must be init after camera
    mech::InputController *mActiveInputCtrl;
    //mech::ThirdPersonCameraController   mThirdPersonCameraController;

    /*struct MechanicsState
    {
        mech::CameraController mCameraController;
    };

    Ptr::OwningPtr<MechanicsState> mMechanicsStatePtr;*/
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


    inline mech::InputController *getActiveController() { return mActiveInputCtrl; }
};

void MechanicsManager::update(float delta_time_sec)
{
    mActiveInputCtrl->update(delta_time_sec);
}

#endif // MECHANICSMANAGER_H
