#ifndef CLIENTSTATE_H
#define CLIENTSTATE_H

#include "../state/scenecreationinfo.h"
#include "gfxscenemanager.h"
#include "phystransformcontainer.h"
#include "../mechanics/mechanicsmanager.h"
#include "../physics/physicsmanager.h"

class ClientState
{
    // macrostate
    Ptr::OwningPtr<state::MacroState> mMacroStatePtr;

    // Must be init before physics... in order to add sceneobject... UGH!!!
    GFXSceneManager mGFXSceneManager;

    // intermediary between physics, control and graphics
    PhysTransformContainer mActorTransforms;
    PhysTransformContainer mKinematicTransforms; // camera etc, not physics controlled

    // physics
    PhysicsManager mPhysicsManager;

    // needs to be after physics!!
    MechanicsManager mMechanicsManager;


public:
    ClientState(state::SceneCreationInfo &new_game_info);

    Ptr::ReadPtr<state::MacroState> readMacroState() { return mMacroStatePtr.getReadPtr(); } // is this even necessary?

    const gfx::Camera inline &getActiveCamera() const { return mGFXSceneManager.getActiveCamera(); }
    const gfx::SceneNode inline &getGFXSceneRoot() const  { return mGFXSceneManager.getGFXSceneRoot(); }

    void inline sendControlSignal(mech::InputController::Signal s) { mMechanicsManager.getActiveController()->sendSignal(s); }
    void inline sendTurnSignals(mech::InputController::TurnSignals ts) { mMechanicsManager.getActiveController()->sendTurnSignals(ts); }

    void inline update(float delta_time_sec);
};

void inline ClientState::update(float delta_time_sec)
{
    // update mechanics
    mMechanicsManager.update(delta_time_sec);

    // update physics
    mPhysicsManager.stepPhysicsSimulation(delta_time_sec);

    // update render jobs
    mActorTransforms.for_all([](PhysTransform &pt){
        pt.scene_node_hdl->transform.position = pt.pos;
        pt.scene_node_hdl->transform.rotation = pt.rot;
    });
}

#endif // CLIENTSTATE_H
