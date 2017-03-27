#ifndef ICOSPHERE_H
#define ICOSPHERE_H

#include "geometry.h"

namespace Procedural
{

inline Geometry icosahedron(float r)
{
    float t = r*(1.0f + std::sqrt(5.0f)) / 2.0f;

    Geometry out =
    {
        {                               // Points
            {-r,  t,  0, 1.0},
            { r,  t,  0, 1.0},
            {-r, -t,  0, 1.0},
            { r, -t,  0, 1.0},

            { 0, -r,  t, 1.0},
            { 0,  r,  t, 1.0},
            { 0, -r, -t, 1.0},
            { 0,  r, -t, 1.0},

            { t,  0, -r, 1.0},
            { t,  0,  r, 1.0},
            {-t,  0, -r, 1.0},
            {-t,  0,  r, 1.0}
        },
        {                               // Triangles
            // 5 faces around point 0
            {0, 11, 5},
            {0, 5, 1},
            {0, 1, 7},
            {0, 7, 10},
            {0, 10, 11},

            // 5 adjacent faces
            {1, 5, 9},
            {5, 11, 4},
            {11, 10, 2},
            {10, 7, 6},
            {7, 1, 8},

            // 5 faces around point 3
            {3, 9, 4},
            {3, 4, 2},
            {3, 2, 6},
            {3, 6, 8},
            {3, 8, 9},

            // 5 adjacent faces
            {4, 9, 5},
            {2, 4, 11},
            {6, 2, 10},
            {8, 6, 7},
            {9, 8, 1}
        }
    };

    gfx::generateNormals(&out.normals, out.points, out.triangles);

    return out;
}

}


#endif // ICOSPHERE_H
