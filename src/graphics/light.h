#ifndef LIGHT_H
#define LIGHT_H

#include "transform.h"

namespace gfx {

struct Light
{
    Light(const Transform &transform, const vmath::Vector4 &color)
        : mTransform(transform), mColor(color) {}

    Transform mTransform;
    vmath::Vector4 mColor;
};

} // namespace gfx


#endif // LIGHT_H
