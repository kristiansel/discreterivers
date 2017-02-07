#ifndef ALTPLANET_H
#define ALTPLANET_H

#include <vector>
#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"
#include "../common/gfx_primitives.h"
#include "planetshapes.h"
#include "spacehash3d.h"
#include "../common/serialize.h"
#include "../common/macro/macroserialize.h"


namespace vmath = Vectormath::Aos;

namespace AltPlanet
{

struct PlanetGeometry
{
    std::vector<vmath::Vector3> points;
    std::vector<gfx::Triangle> triangles;
};

static const unsigned int NUM_GEN_POINTS_DEFAULT = 10000;

PlanetGeometry generate(unsigned int n_points, const Shape::BaseShape &planet_shape);

enum class PlanetShape {Sphere, Torus};
void createOrLoadPlanetGeom(PlanetGeometry &alt_planet_geometry, Shape::BaseShape *&planet_shape_ptr, PlanetShape shape);

void perturbHeightNoise3D(std::vector<vmath::Vector3> &points, const Shape::BaseShape &planet_shape);

void pointsRepulse(std::vector<vmath::Vector3> &points, SpaceHash3D &spacehash, const Shape::BaseShape &planet_shape, float repulse_factor);

void pointsRepulse(std::vector<vmath::Vector3> &points, const Shape::BaseShape &planet_shape, float repulse_factor);

} // namespace AltPlanet

IMPL_SERIALIZABLE(AltPlanet::PlanetGeometry, points, triangles)

#endif // ALTPLANET_H
