#ifndef MECHANICSMANAGER_H
#define MECHANICSMANAGER_H

#include "cameracontroller.h"
#include "charactercontroller.h"
#include "thirdpersoncameracontroller.h"
#include "../state/scenecreationinfo.h"
#include "../physics/physicssimulation.h"
#include "../common/freelistset.h"
#include "../appconstraints.h"
#include "../events/immediateevents.h"

class MechanicsManager
{
    mech::InputController  *mActiveInputCtrl;

    using ControllerPool = stdext::freelist_set<mech::CharacterController, appconstraints::n_actors_max>;
    using ControllerPoolHandle = typename ControllerPool::node*;

    ControllerPool mControllers;

    Ptr::WritePtr<RigidBodyPool> mActorRigidBodyPoolPtr;

    // for debug purposes
    mech::InputController  *mPreviousInputCtrl;  // used to store non-camera-controller controller...
    mech::CameraController  mCameraController;   // must be init after camera

public:
    MechanicsManager(Ptr::WritePtr<gfx::Camera> camera_ptr,
                     Ptr::WritePtr<RigidBodyPool> actor_rigid_body_pool_ptr);

    ~MechanicsManager();

    //void wireControlsToCamera(Ptr::WritePtr<gfx::Camera> camera_ptr);

    inline void update(float delta_time_sec);

    void initScene(const vmath::Vector3 &point_above,
                   Ptr::ReadPtr<state::MacroState> scene_data,
                   const std::vector<state::Actor> &actors);

    inline bool getPlayerTargetOrientation(vmath::Quat &player_orientation);

    inline mech::InputController *getActiveController() { return mActiveInputCtrl; }

    // for debug purposes
    void toggleDebugFreeCam();

    bool inline isFreeCam() { return mPreviousInputCtrl!=nullptr; }

    vmath::Vector3 getPlayerPosition() const {
        mech::CharacterController *player_char_contr = mPreviousInputCtrl!=nullptr ?
                    (mech::CharacterController*)(mPreviousInputCtrl) :
                    (mech::CharacterController*)(mActiveInputCtrl);
        btRigidBody &player_rigid_body = player_char_contr->getRigidBody();
        btTransform &transf = player_rigid_body.getWorldTransform();
        btVector3 &pos = transf.getOrigin();
        return vmath::Vector3(pos.x(), pos.y(), pos.z());
    } // a very dodgy methods...
};

void MechanicsManager::update(float delta_time_sec)
{
    //mActiveInputCtrl->update(delta_time_sec);

    mControllers.for_all([delta_time_sec](mech::CharacterController &ctrl) {
        ctrl.update(delta_time_sec);
    });

    mCameraController.update(delta_time_sec);

    // notify subscribers... smell...
    events::Immediate::broadcast(events::PlayerUpdateEvent{getPlayerPosition()});

}

inline bool MechanicsManager::getPlayerTargetOrientation(vmath::Quat &player_orientation)
{
    mech::CharacterController* player_controller = dynamic_cast<mech::CharacterController*>(mActiveInputCtrl);
    if (player_controller)
    {
        player_orientation = player_controller->getTargetOrientation();
        return true;
    }
    else
    {
        //DEBUG_LOG("ERROR: active controller is not a character controller");
        //DEBUG_ASSERT(false);
        //return vmath::Quat(0.0f, 0.0f, 0.0f, 1.0f);
        return false;
    }
}

#endif // MECHANICSMANAGER_H
