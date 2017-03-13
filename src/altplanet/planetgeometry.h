#ifndef PLANETGEOMETRY_H
#define PLANETGEOMETRY_H

#include "../common/gfx_primitives.h"
#include "../common/macro/macroserialize.h"
#include "../common/serialize.h"
#include <vector>

namespace AltPlanet {

struct PlanetGeometry
{
    std::vector<vmath::Vector3> points;
    std::vector<gfx::Triangle> triangles;
};

}

IMPL_SERIALIZABLE(AltPlanet::PlanetGeometry, points, triangles)

#endif // PLANETGEOMETRY_H

