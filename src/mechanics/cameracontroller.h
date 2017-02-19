#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <array>

#include "../graphics/camera.h"
#include "../common/flags.h"
#include "../common/initptr.h"

namespace mech {

class CameraController
{
public:
    // signals
    enum Signal {
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

    using MoveSignalFlags = stdext::Flags<Signal, Signal::Idle>;
    using TurnSignals = std::array<float, 2>;

    // events
    enum Event {
        None          = 0b00000000,
        ToggleOrtho   = 0b00000001,
    };

private:
    // props
    stdext::InitPtr<gfx::Camera> mCameraPtr;

    // state
    float           mSpeed;
    float           mMouseTurnSpeed;
    MoveSignalFlags mSignalFlags;
    TurnSignals     mTurnSignals;

    // non-default-constructible
    CameraController() = delete;

public:
    CameraController(stdext::InitPtr<gfx::Camera> camera_ptr, float speed = 0.1f, float mouse_turn_speed = 2.0f) :  // ieeeee! raw pointer... :(
        mCameraPtr(camera_ptr),
        mSpeed(speed),
        mMouseTurnSpeed(mouse_turn_speed)
    {
        clearSignals();
    }

    // mutators
    inline void clearSignals()
    {
        mSignalFlags.clearAll();
        mTurnSignals = {0.0f, 0.0f};
    }

    inline void sendSignal(Signal signal)
    {
        mSignalFlags.setFlag(signal);
    }

    inline void sendTurnSignals(const TurnSignals &turn_signals)
    {
        mTurnSignals[0] = turn_signals[0];
        mTurnSignals[1] = turn_signals[1];
    }

    inline void sendEvent(Event event)
    {
        // ...
    }

    inline void update();
};

// inline methods
inline void CameraController::update()
{
    float speed = mSpeed;
    if (mSignalFlags.checkFlag(Signal::SpeedUp))
    {
        speed = 4.0f*speed;
    }

    if (mSignalFlags.checkFlag(Signal::Forward))
    {
        mCameraPtr->mTransform.position += speed * mCameraPtr->mTransform.getForwardDir();
    }
    if (mSignalFlags.checkFlag(Signal::Backward))
    {
        mCameraPtr->mTransform.position -= speed * mCameraPtr->mTransform.getForwardDir();
    }
    if (mSignalFlags.checkFlag(Signal::Left))
    {
        mCameraPtr->mTransform.position -= speed * mCameraPtr->mTransform.getRightDir();
    }
    if (mSignalFlags.checkFlag(Signal::Right))
    {
        mCameraPtr->mTransform.position += speed * mCameraPtr->mTransform.getRightDir();
    }
    if (mSignalFlags.checkFlag(Signal::Down))
    {
        mCameraPtr->mTransform.position -= speed * vmath::Vector3(0.0f, 1.0f, 0.0f);
    }
    if (mSignalFlags.checkFlag(Signal::Up))
    {
        mCameraPtr->mTransform.position += speed * vmath::Vector3(0.0f, 1.0f, 0.0f);
    }

    mCameraPtr->mTransform.rotation =
            vmath::Quat::rotation(mMouseTurnSpeed*mTurnSignals[0], vmath::Vector3(0.0f, 1.0f, 0.0f))*
            vmath::Quat::rotation(mMouseTurnSpeed*mTurnSignals[1], mCameraPtr->mTransform.getRightDir())*
            mCameraPtr->mTransform.rotation;

}

}

#endif // CAMERACONTROLLER_H
