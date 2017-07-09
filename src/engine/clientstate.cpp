#include "clientstate.h"

ClientState::ClientState(state::SceneCreationInfo &new_game_info) :
   mGFXSceneManager(gfx::Camera(gfx::PerspectiveProjection(new_game_info.aspect_ratio, DR_M_PI_4, 0.0f, 1000000.0f)),
                    gfx::SceneNode(),
                    Ptr::WritePtr<PhysTransformContainer>(&mActorTransforms)),
   mPhysicsManager(Ptr::WritePtr<PhysTransformContainer>(&mActorTransforms)),
   mMechanicsManager(Ptr::WritePtr<gfx::Camera>(&mGFXSceneManager.mCamera), mPhysicsManager.getActorRigidBodyPoolWPtr()),
   mMacroStatePtr(std::move(new_game_info.macro_state_ptr)),
   mSimulationPaused(false),
   mFrame(0)
{
    // ctor
    mGFXSceneManager.initScene(new_game_info.point_above, mMacroStatePtr.getReadPtr(), new_game_info.actors);
    mPhysicsManager.initScene(new_game_info.land_pos, mMacroStatePtr.getReadPtr(), new_game_info.actors);
    mMechanicsManager.initScene(new_game_info.land_pos, mMacroStatePtr.getReadPtr(), new_game_info.actors);

    // notify subscribers
    events::Immediate::broadcast(events::SimStatusUpdateEvent{mSimulationPaused});
    events::Immediate::broadcast(events::ToggleFreeCamEvent{mMechanicsManager.isFreeCam()});

}


void ClientState::update(float delta_time_sec)
{

    // update mechanics
    mMechanicsManager.update(delta_time_sec);

    //vmath::Vector3 pos1 = mMechanicsManager.getPlayerPosition();
    //DEBUG_LOG("PLAYER POSITION BEFORE: "<<pos1.getX()<<","<<pos1.getY()<<","<<pos1.getZ());
    //DEBUG_LOG("DELTA_TIME_SEC: "<<delta_time_sec);

    // update physics
    float sim_delta_time = mSimulationPaused ? 0.0f : delta_time_sec;
    mPhysicsManager.stepPhysicsSimulation(sim_delta_time);

    //vmath::Vector3 pos2 = mMechanicsManager.getPlayerPosition();
    //DEBUG_LOG("PLAYER POSITION AFTER: "<<pos2.getX()<<","<<pos2.getY()<<","<<pos2.getZ());

    // update gravity
    mPhysicsManager.updateDynamicsGravity(mMacroStatePtr->planet_base_shape);

    // update render jobs
    mActorTransforms.for_all([](PhysTransform &pt){
        pt.scene_node_hdl->transform.position = pt.pos;
        pt.scene_node_hdl->transform.rotation = pt.rot;
    });

    // update graphics camera, this smells...
    vmath::Quat player_orientation;
    bool player_is_controlled = mMechanicsManager.getPlayerTargetOrientation(player_orientation);
    if (player_is_controlled)
    {
        mGFXSceneManager.updateCamera(player_orientation, mMacroStatePtr->getLocalUp(getActiveCamera().mTransform.position));
    }


    /*if (mFrame > 0) {
        DEBUG_LOG("TERMINATING FOR DEBUG");
        std::terminate();
    }*/

    mFrame++;
}
