#ifndef MECHANICSMANAGER_H
#define MECHANICSMANAGER_H

#include "cameracontroller.h"
#include "thirdpersoncameracontroller.h"

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

public:
    MechanicsManager(Ptr::WritePtr<gfx::Camera> camera_ptr);
    ~MechanicsManager();

    //void wireControlsToCamera(Ptr::WritePtr<gfx::Camera> camera_ptr);

    inline void update();

    inline mech::InputController *getActiveController() { return mActiveInputCtrl; }
};

void MechanicsManager::update()
{
    mActiveInputCtrl->update();
}

#endif // MECHANICSMANAGER_H
