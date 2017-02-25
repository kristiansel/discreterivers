#ifndef LIGHT_H
#define LIGHT_H

#include "transform.h"

namespace gfx {

struct Light
{
    Light(const vmath::Vector4 &p, const vmath::Vector4 &c)
        : position(p), color(c) {}

    vmath::Vector4 position;
    vmath::Vector4 color;
};

} // namespace gfx


#endif // LIGHT_H
