#ifndef ORIENTEDANGLE_H
#define ORIENTEDANGLE_H

#include <cmath>

#include "gfx_primitives.h"
#include "macro/macrodebugassert.h"

namespace mathext {

inline float angle(const vmath::Vector3 &a, const vmath::Vector3 &b)
{
    float l_a = vmath::length(a);
    float l_b = vmath::length(b);
    DEBUG_ASSERT(((l_a>0)&&(l_b>0)));
    return std::acos(vmath::dot(a, b)/(l_a*l_b));
}

inline float orientedAngle(const vmath::Vector3 &a, const vmath::Vector3 &b, const vmath::Vector3 &n)
{
    float x_len = vmath::dot(cross(a, b), n);
    return x_len >= 0.0f ? angle(a, b) : -angle(a, b);
}

}

#endif // ORIENTEDANGLE_H
