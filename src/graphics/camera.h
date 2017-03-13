#ifndef CAMERA_H
#define CAMERA_H

#include "gfxcommon.h"
#include "transform.h"

#include "../common/either.h"

namespace gfx {

class OrthographicProjection
{
    vmath::Matrix4 mProjectionMatrix;
    float mAspect, mHalfHeight, mZNear, mZFar;

    OrthographicProjection() = delete;
public:
    OrthographicProjection(float aspect, float half_height, float z_near, float z_far) :
        mAspect(aspect), mHalfHeight(half_height), mZNear(z_near), mZFar(z_far),
        mProjectionMatrix(vmath::Matrix4::orthographic( -aspect*half_height, aspect*half_height,
                                                        -half_height, half_height, z_near, z_far)) {}

    vmath::Matrix4 getProjectionMatrix() const { return mProjectionMatrix; }

    void updateAspect(float aspect)
    {
        mAspect = aspect;
        mProjectionMatrix = vmath::Matrix4::orthographic( -mAspect*mHalfHeight, mAspect*mHalfHeight,
                                                          -mHalfHeight, mHalfHeight, mZNear, mZFar );
    }
};

class PerspectiveProjection
{
    vmath::Matrix4 mProjectionMatrix;
    float mAspect, mFieldOfView, mZNear, mZFar;

    PerspectiveProjection() = delete;
public:
    PerspectiveProjection(float aspect, float field_of_view, float z_near, float z_far) :
        mAspect(aspect), mFieldOfView(field_of_view), mZNear(z_near), mZFar(z_far),
        mProjectionMatrix(vmath::Matrix4::perspective(field_of_view, aspect, z_near, z_far )) {}

    vmath::Matrix4 getProjectionMatrix() const { return mProjectionMatrix; }

    void updateAspect(float aspect)
    {
        mAspect = aspect;
        mProjectionMatrix = vmath::Matrix4::perspective(mFieldOfView, mAspect, mZNear, mZFar);
    }
};


using Projection = stdext::either<OrthographicProjection, PerspectiveProjection>;

struct Camera
{
//    /inline Camera(float aspect_ratio);
    inline Camera(const Projection &projection, const Transform &transform = Transform());

    inline vmath::Matrix4 getCamMatrixInverse() const { return vmath::inverse(mTransform.getTransformMatrix()); }

    inline void setProjection(const Projection &projection) { mProjection = projection; }

    inline void updateAspect(float aspect_ratio);

    inline vmath::Matrix4 getProjectionMatrix() const;

    Transform mTransform;
    Projection mProjection;
    //vmath::Matrix4 mProjectionMatrix;
};


inline Camera::Camera(const Projection &projection, const Transform &transform) :
    mTransform(transform), mProjection(projection)
{

}

inline void Camera::updateAspect(float aspect_ratio)
{
    switch (mProjection.get_type())
    {
    case (Projection::is_a<OrthographicProjection>::value):
        {
            mProjection.get<OrthographicProjection>().updateAspect(aspect_ratio);
        }
        break;
    case (Projection::is_a<PerspectiveProjection>::value):
        {
            mProjection.get<PerspectiveProjection>().updateAspect(aspect_ratio);
        }
        break;
    }
}

inline vmath::Matrix4 Camera::getProjectionMatrix() const
{
    switch (mProjection.get_type())
    {
    case (Projection::is_a<OrthographicProjection>::value):
        {
            return mProjection.get_const<OrthographicProjection>().getProjectionMatrix();
        }
        break;
    case (Projection::is_a<PerspectiveProjection>::value):
        {
            return mProjection.get_const<PerspectiveProjection>().getProjectionMatrix();
        }
        break;
    }
}

}

#endif // CAMERA_H
