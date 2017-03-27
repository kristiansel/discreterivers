#ifndef MAPCONTROLLER_H
#define MAPCONTROLLER_H

#include <array>

#include "../graphics/camera.h"
#include "../common/flags.h"
#include "../common/pointer.h"

namespace mech {

class MapController
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
    Ptr::WritePtr<gfx::Camera> mCameraPtr;

    // state
    float           mSpeed;
    float           mMouseScrollSpeed;
    float           mScaleFactor;

    //MoveSignalFlags mSignalFlags;
    TurnSignals     mTurnSignals;
    ScrollSignal    mScrollSignal;

public:
    MapController(float speed = 0.27f*0.0062832f) : // 2π/1000 = 0.0062832f
        mCameraPtr(nullptr),
        mSpeed(speed),
        mScaleFactor(1.0f)
    {
        clearSignals();
    }

    // mutators
    inline void setControlled(Ptr::WritePtr<gfx::Camera> camera_ptr)
    {
        mCameraPtr = camera_ptr;
    }

    inline void clearControlled()
    {
        mCameraPtr = Ptr::WritePtr<gfx::Camera>(nullptr);
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
        //update(); // immediate update
    }

    inline void sendScrollSignal(const ScrollSignal &scroll_signal)
    {
        mScrollSignal = scroll_signal;
        //update(); // immediate update
    }

    /*inline void sendEvent(Event event)
    {
        // ...
    }*/

    inline void update();
};

// inline methods
inline void MapController::update()
{
    if (mCameraPtr)
    {
        mScaleFactor = std::min(2.0f, std::max(0.1f, mScaleFactor - mScrollSignal * 0.1f));

        mCameraPtr->mTransform.position = vmath::Vector3(mSpeed*mScaleFactor*mTurnSignals[0], -mSpeed*mScaleFactor*mTurnSignals[1], 0.0f) +
            mCameraPtr->mTransform.position;

        //std::cout << "scale factor " << mScaleFactor << std::endl;
        mCameraPtr->mTransform.scale = mScaleFactor * vmath::Vector3(1.0f);

        clearSignals();
    }

}

}


#endif // ROTATORCONTROLLER_H
