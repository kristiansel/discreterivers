#ifndef PLANEGEOMETRY_H
#define PLANEGEOMETRY_H

#include "geometry.h"
#include "../stdext.h"
#include "../mathext.h"

namespace Procedural
{

inline Geometry plane(float w, float d)
{
    Geometry out =
    {
        {{-w,   0.0f, -d, 1.0f}, {-w,   0.0f,  d, 1.0f}, {w,   0.0f,  d, 1.0f}, {w,   0.0f,  -d, 1.0f}},
        //  top
        {{0, 1, 2}, {0, 2, 3}},
        {{0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}}
    };

    return out;
}

template<typename T>
std::vector<T> concat(const std::vector<T> &a, const std::vector<T> &b)
{
    std::vector<T> c = a;
    c.insert(c.end(), b.begin(), b.end());
    return c;
}

inline Geometry combine(const Geometry &g0, const Geometry &g1)
{
    int n = g0.points.size();
    return {
        concat(g0.points, g1.points),
        concat(g0.triangles, StdExt::vector_map<gfx::Triangle, gfx::Triangle>(g1.triangles,
                                                [n](const gfx::Triangle &t)->gfx::Triangle{
                                                    return {t[0]+n, t[1]+n, t[2]+n};
                                                })),
        concat(g0.normals, g1.normals)
    };
}

inline Geometry transform(const Geometry &g, const vmath::Matrix4 &m)
{
    return {
        StdExt::vector_map<vmath::Vector4, vmath::Vector4>(g.points,
                           [&m](const vmath::Vector4 &v)->vmath::Vector4{
                               return m * v;
                           }),
        g.triangles,
        StdExt::vector_map<vmath::Vector4, vmath::Vector4>(g.normals,
                           [&m](const vmath::Vector4 &v)->vmath::Vector4{
                               return m * v;
                           })
    };

}

inline Geometry doublePlane(float w, float d)
{
    return combine(           plane(w, d),
                    transform(plane(w, d), vmath::Matrix4::rotation(DR_M_PI, {1.0f, 0.0f, 0.0f}))
                  );
}

inline Geometry boxPlanes(float w, float h, float d)
{
    return combine(
                transform(plane(w, d),  vmath::Matrix4::translation({0.0, h, 0.0}) *
                                        vmath::Matrix4::rotation(   0.0f, {1.0f, 0.0f, 0.0f})),
           combine(
                transform(plane(w, d),  vmath::Matrix4::translation({0.0, -h, 0.0}) *
                                        vmath::Matrix4::rotation(DR_M_PI, {1.0f, 0.0f, 0.0f})),
           combine(
                transform(plane(w, d),  vmath::Matrix4::translation({w, 0.0, 0.0}) *
                                        vmath::Matrix4::rotation(-DR_M_PI/2.0f, {0.0f, 0.0f, 1.0f})),
           combine(
                transform(plane(w, d),  vmath::Matrix4::translation({-w, 0.0, 0.0}) *
                                        vmath::Matrix4::rotation(DR_M_PI/2.0f, {0.0f, 0.0f, 1.0f})),
           combine(
                transform(plane(w, d),  vmath::Matrix4::translation({0.0, 0.0, d}) *
                                        vmath::Matrix4::rotation(DR_M_PI/2.0f, {1.0f, 0.0f, 0.0f})),

                transform(plane(w, d),  vmath::Matrix4::translation({0.0, 0.0, -d}) *
                                        vmath::Matrix4::rotation(-DR_M_PI/2.0f, {1.0f, 0.0f, 0.0f}))
    )))));
}


}

#endif // PLANEGEOMETRY_H
