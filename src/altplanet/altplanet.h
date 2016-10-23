#ifndef ALTPLANET_H
#define ALTPLANET_H

#include <vector>
#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"


#include "../common/gfx_primitives.h"
#include "planetshapes.h"
#include "spacehash3d.h"



namespace vmath = Vectormath::Aos;

namespace AltPlanet
{

struct Geometry {std::vector<vmath::Vector3> points; std::vector<gfx::Triangle> triangles;};

Geometry generate(unsigned int n_points, const Shape::BaseShape &planet_shape);

void pointsRepulse(std::vector<vmath::Vector3> &points, SpaceHash3D &spacehash, const Shape::BaseShape &planet_shape, float repulse_factor);

void pointsRepulse(std::vector<vmath::Vector3> &points, const Shape::BaseShape &planet_shape, float repulse_factor);

namespace util
{

}

}

#endif // ALTPLANET_H
