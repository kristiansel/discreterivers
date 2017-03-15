#ifndef PROCEDURALGEOMETRY_H
#define PROCEDURALGEOMETRY_H

#include <vector>
#include "../gfx_primitives.h"

namespace Procedural {

struct Geometry
{
    std::vector<vmath::Vector4> points;
    std::vector<gfx::Triangle> triangles;
    std::vector<vmath::Vector4> normals;
};

}
#endif // PROCEDURALGEOMETRY_H
