#ifndef CAMERA_H
#define CAMERA_H

#include "gfxcommon.h"
#include "transform.h"

namespace gfx {

struct Camera
{
    inline Camera(float aspect_ratio);
    vmath::Matrix4 mProjectionMatrix;
    vmath::Matrix4 getCamMatrixInverse() const { return vmath::inverse(mTransform.getTransformMatrix()); }

    // need to pass transform with the constructor...
    inline void updateAspect(float aspect_ratio);

    Transform mTransform;
};

inline Camera::Camera(float aspect_ratio)
{
    // set up camera
    // camera
    // projection matrix
    updateAspect(aspect_ratio);

//    float half_height = 3.5f;
//    mProjectionMatrix = vmath::Matrix4::orthographic(
//        -aspect_ratio*half_height, aspect_ratio*half_height, -half_height, half_height, 0.1f, 100.0f
//    );



    /*vmath::Matrix4 camera_matrix = vmath::Matrix4::translation(vmath::Vector3(0.0f, 0.0f, 10.0f));
    mCamMatrixInverse = vmath::inverse(camera_matrix);*/
}

inline void Camera::updateAspect(float aspect_ratio)
{
    mProjectionMatrix = vmath::Matrix4::perspective(
        M_PI_4,                         // field of view (radians)
        aspect_ratio,                   // aspect ratio
        0.1f,                           // znear
        100.0f                          // zfar
    );
}

}

#endif // CAMERA_H
