#ifndef THIRDPERSONCAMERACONTROLLER_H
#define THIRDPERSONCAMERACONTROLLER_H

#include "../common/pointer.h"
#include "../graphics/camera.h"
#include "../engine/phystransformcontainer.h"

namespace mech {

class ThirdPersonCameraController
{
    // no signals
    // just updates camera based on physical transform


    Ptr::ReadPtr<PhysTransform> mPhysTransformPtr;
    Ptr::WritePtr<gfx::Camera>  mCameraPtr;

    float mCamDist;

public:
    ThirdPersonCameraController(float cam_dist,
                                Ptr::ReadPtr<PhysTransform> phys_transform_ptr,
                                Ptr::WritePtr<gfx::Camera> cam_ptr = Ptr::WritePtr<gfx::Camera>(nullptr)) :
        mPhysTransformPtr(phys_transform_ptr),
        mCameraPtr(cam_ptr),
        mCamDist(cam_dist) {}

    //void setPhysTransformPtr(Ptr::ReadPtr<PhysTransform> phys_transform_ptr) { mPhysTransformPtr = phys_transform_ptr; }

    void setCameraPtr(Ptr::WritePtr<gfx::Camera> cam_ptr) { mCameraPtr = cam_ptr; }

    void update()
    {
        if (mCameraPtr)
        {
            // find the forward vector
            vmath::Matrix3 rot_mat = vmath::Matrix3::rotation(mPhysTransformPtr->rot);
            vmath::Vector3 forward = rot_mat * vmath::Vector3(0.0f, 0.0f, -1.0f);
            vmath::Vector3 cam_pos = mPhysTransformPtr->pos + mCamDist * -forward;


            mCameraPtr->mTransform.lookAt(cam_pos, mPhysTransformPtr->pos, rot_mat * vmath::Vector3(0.0f, 1.0f, 0.0f));
        }
    }
};

}

#endif // THIRDPERSONCAMERACONTROLLER_H
