#ifndef PLANETGEOMETRY_H
#define PLANETGEOMETRY_H

#include "../common/gfx_primitives.h"
#include <vector>


namespace AltPlanet {


struct PlanetGeometry
{
    std::vector<vmath::Vector3> points;
    std::vector<gfx::Triangle> triangles;
};


}


#endif // PLANETGEOMETRY_H

