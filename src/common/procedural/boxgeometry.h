#ifndef BOXGEOMETRY_H
#define BOXGEOMETRY_H

#include <vector>
#include "../gfx_primitives.h"

namespace Procedural
{

struct Geometry
{
    std::vector<vmath::Vector4> points;
    std::vector<gfx::Triangle> triangles;
    std::vector<vmath::Vector4> normals;
};

inline Geometry boxGeometry(float w, float h, float d)
{
    Geometry out =
    {
        {{-w,   h, -d, 1.0f}, {-w,   h,  d, 1.0f}, {w,   h,  d, 1.0f}, {w,   h,  -d, 1.0f},
         {-w,  -h, -d, 1.0f}, {-w,  -h,  d, 1.0f}, {w,  -h,  d, 1.0f}, {w,  -h,  -d, 1.0f}},
        //  top                 // bottom
        {{0, 1, 2}, {0, 2, 3}, {4, 6, 5}, {4, 7, 6},
        //  front                 // back
         {5, 6, 2}, {5, 2, 1}, {0, 3, 7}, {0, 7, 4},
        //  left                 // right
         {5, 1, 4}, {4, 1, 0}, {7, 2, 6}, {7, 3, 2}}
    };

    gfx::generateNormals(&out.normals, out.points, out.triangles);

    return out;

}

} // Procedural

#endif // BOXGEOMETRY_H
