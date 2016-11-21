#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../common/gfx_primitives.h"

namespace gfx {

struct Transform
{
    Transform() : position(0.0f, 0.0f, 0.0f), rotation(vmath::Quat::identity()), scale(1.0f, 1.0f, 1.0f) {}
    Transform(const Transform &t) : position(t.position), rotation(t.rotation), scale(t.scale) {}

    vmath::Vector3 position;
    vmath::Quat rotation;
    vmath::Vector3 scale;

    inline vmath::Matrix4 getTransformMatrix() const
    {
        return vmath::Matrix4::translation(position) * vmath::Matrix4::rotation(rotation) * vmath::Matrix4::scale(scale);
    }
};

}

#endif // TRANSFORM_H
