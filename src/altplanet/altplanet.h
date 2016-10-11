#ifndef ALTPLANET_H
#define ALTPLANET_H

#include <vector>

#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"

#include "planetshapes.h"

namespace vmath = Vectormath::Aos;

namespace AltPlanet
{

std::vector<vmath::Vector3> generate(unsigned int n_points, const Shape::BaseShape &planet_shape);

namespace util
{

}

}

#endif // ALTPLANET_H
