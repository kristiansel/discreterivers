#ifndef CAMERA_H
#define CAMERA_H

#include "gfxcommon.h"
#include "transform.h"

namespace gfx {

struct Camera
{
    Camera(int width, int height);
    vmath::Matrix4 mProjectionMatrix;
    vmath::Matrix4 getCamMatrixInverse() const { return vmath::inverse(mTransform.getTransformMatrix()); }

    Transform mTransform;
};

}

#endif // CAMERA_H
