#ifndef ROTATORCONTROLLER_H
#define ROTATORCONTROLLER_H

#include <array>

#include "../graphics/scenenode.h"
#include "../common/flags.h"
#include "../common/pointer.h"

namespace mech {

class RotatorController
{
public:
    // signals
    /*enum Signal {
        Idle          = 0b00000000,

        Forward       = 0b00000001,
        Backward      = 0b00000010,
        Left          = 0b00000100,
        Right         = 0b00001000,

        SpeedUp       = 0b00010000,
        SlowDown      = 0b00100000,
        Up            = 0b01000000,
        Down          = 0b10000000
    };

    using MoveSignalFlags = stdext::Flags<Signal, Signal::Idle>;*/
    using TurnSignals = std::array<float, 2>;
    using ScrollSignal = int32_t;

    // events
    /*enum Event {
        None          = 0b00000000,
        ToggleOrtho   = 0b00000001,
    };*/

private:
    // props
    Ptr::WritePtr<gfx::SceneNode> mSceneNodePtr;

    // state
    float           mMouseTurnSpeed;
    //MoveSignalFlags mSignalFlags;
    TurnSignals     mTurnSignals;
    ScrollSignal    mScrollSignal;

public:
    RotatorController(float mouse_turn_speed = 1.0f*0.0062832f) :
        mSceneNodePtr(nullptr),
        mMouseTurnSpeed(mouse_turn_speed)
    {
        clearSignals();
    }

    // mutators
    inline void setControlledNode(gfx::SceneNode * const scenenode_ptr)
    {
        mSceneNodePtr = scenenode_ptr;
    }

    inline void clearControlledNode()
    {
        mSceneNodePtr = nullptr;
    }

    // mutators
    inline void clearSignals()
    {
        //mSignalFlags.clearAll();
        mTurnSignals = {0.0f, 0.0f};
        mScrollSignal = 0.0f;
    }

    /*inline void sendSignal(Signal signal)
    {
        mSignalFlags.setFlag(signal);
    }*/

    inline void sendTurnSignals(const TurnSignals &turn_signals)
    {
        mTurnSignals[0] = turn_signals[0];
        mTurnSignals[1] = turn_signals[1];
        update(); // immediate update
    }

    inline void sendScrollSignal(const ScrollSignal &scroll_signal)
    {
        mScrollSignal = scroll_signal;
        update(); // immediate update
    }

    /*inline void sendEvent(Event event)
    {
        // ...
    }*/

    inline void update();
};

// inline methods
inline void RotatorController::update()
{
    if (mSceneNodePtr)
    {   
        //std::cout << "sending turn signals " << mTurnSignals[0] << ", " << mTurnSignals[1] << std::endl;

        mSceneNodePtr->transform.rotation =
            vmath::Quat::rotation(-mMouseTurnSpeed*mTurnSignals[0], vmath::Vector3(0.0f, 1.0f, 0.0f))*
            vmath::Quat::rotation(-mMouseTurnSpeed*mTurnSignals[1], vmath::Vector3(1.0f, 0.0f, 0.0f))*
            mSceneNodePtr->transform.rotation;

        mSceneNodePtr->transform.scale = (1.0f + mScrollSignal*0.1f) * mSceneNodePtr->transform.scale;

        clearSignals();
    }

}

}


#endif // ROTATORCONTROLLER_H
