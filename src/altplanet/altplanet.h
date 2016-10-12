#ifndef ALTPLANET_H
#define ALTPLANET_H

#include <vector>
#include <tuple>

#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"


#include "../common/gfx_primitives.h"
#include "planetshapes.h"



namespace vmath = Vectormath::Aos;

namespace AltPlanet
{

struct Geometry {std::vector<vmath::Vector3> points; std::vector<gfx::Triangle> triangles;};

Geometry generate(unsigned int n_points, const Shape::BaseShape &planet_shape);

namespace util
{

}

}

#endif // ALTPLANET_H
