#ifndef SUBIVIDE_H
#define SUBIVIDE_H

#include <vector>
#include "../common/gfx_primitives.h"

namespace AltPlanet
{

void subdivideGeometry(  std::vector<vmath::Vector3> &points,
                         std::vector<gfx::Triangle> &triangles,
                         std::vector<std::vector<gfx::Triangle>> &subd_triangles,
                         const int num_subdivisions);


void subdivideOnce(  std::vector<vmath::Vector3> &points,
                     std::vector<gfx::Triangle> &triangles);

} // namespace AltPlanet


#endif // SUBIVIDE_H
