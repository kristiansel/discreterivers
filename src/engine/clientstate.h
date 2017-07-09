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

    // Debug flags
    bool mSimulationPaused;

    long mFrame;


public:
    ClientState(state::SceneCreationInfo &new_game_info);

    void update(float delta_time_sec);

    Ptr::ReadPtr<state::MacroState> readMacroState() { return mMacroStatePtr.getReadPtr(); } // is this even necessary?

    const gfx::Camera inline &getActiveCamera() const { return mGFXSceneManager.getActiveCamera(); }
    const gfx::SceneNode inline &getGFXSceneRoot() const  { return mGFXSceneManager.getGFXSceneRoot(); }

    void inline sendControlSignal(mech::InputController::Signal s) { mMechanicsManager.getActiveController()->sendSignal(s); }
    void inline sendTurnSignals(mech::InputController::TurnSignals ts) { mMechanicsManager.getActiveController()->sendTurnSignals(ts); }

    // debug functionality
    void inline toggleDebugFreeCam() {
        mMechanicsManager.toggleDebugFreeCam();
        events::Immediate::broadcast(events::ToggleFreeCamEvent{mMechanicsManager.isFreeCam()});
    }
    void inline toggleSimulationPaused() {
        mSimulationPaused = !mSimulationPaused;
        events::Immediate::broadcast(events::SimStatusUpdateEvent{mSimulationPaused});
    }

};


#endif // CLIENTSTATE_H
