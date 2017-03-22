#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <array>

#include "../graphics/camera.h"
/*#include "../common/flags.h"*/
#include "../common/pointer.h"
#include "inputcontroller.h"

namespace mech {

class CameraController : public InputController
{
public:

private:
    // props
    Ptr::WritePtr<gfx::Camera> mCameraPtr;

    // state
    float           mSpeed;
    float           mMouseTurnSpeed;
    vmath::Vector3  mUpDir;

public:
    CameraController(Ptr::WritePtr<gfx::Camera> camera_ptr = nullptr, const vmath::Vector3 &up_dir = vmath::Vector3(0.0f, 1.0f, 0.0f),
                     float speed = 1.0f, float mouse_turn_speed = 2.0f) :  // ieeeee! raw pointer... :(
        mCameraPtr(camera_ptr),
        mUpDir(up_dir),
        mSpeed(speed),
        mMouseTurnSpeed(mouse_turn_speed)
    {
        // clearSignals(); // done by base...
    }

    // mutators
    inline void setControlled(Ptr::WritePtr<gfx::Camera> camera_ptr)
    {
        mCameraPtr = camera_ptr;
    }

    void setUpDir(const vmath::Vector3 &up_dir) { mUpDir = up_dir; }

    inline void update();
};

// inline methods
inline void CameraController::update()
{
    float speed = mSpeed;
    if (mSignalFlags.checkFlag(Signal::SpeedUp))
    {
        speed = 4.0f*speed; // SeedUp signal speeds up by four
    }

    if (mCameraPtr)
    {
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
                vmath::Quat::rotation(mMouseTurnSpeed*mTurnSignals[0], mUpDir ) *
                vmath::Quat::rotation(mMouseTurnSpeed*mTurnSignals[1], mCameraPtr->mTransform.getRightDir()) *
                mCameraPtr->mTransform.rotation;
    }

    clearSignals();
}

}

#endif // CAMERACONTROLLER_H
