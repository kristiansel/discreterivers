#ifndef ALTPLANET_H
#define ALTPLANET_H

#include <vector>
#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"
#include "../common/gfx_primitives.h"
#include "planetshapes.h"
#include "spacehash3d.h"
#include "../common/serialize.h"


namespace vmath = Vectormath::Aos;

namespace AltPlanet
{

struct PlanetGeometry
{
    std::vector<vmath::Vector3> points;
    std::vector<gfx::Triangle> triangles;
};

PlanetGeometry generate(unsigned int n_points, const Shape::BaseShape &planet_shape);

void pointsRepulse(std::vector<vmath::Vector3> &points, SpaceHash3D &spacehash, const Shape::BaseShape &planet_shape, float repulse_factor);

void pointsRepulse(std::vector<vmath::Vector3> &points, const Shape::BaseShape &planet_shape, float repulse_factor);

} // namespace AltPlanet

namespace Serial
{

template<>
inline void serialize<AltPlanet::PlanetGeometry>(const AltPlanet::PlanetGeometry &obj, StreamType &res)
{
    serialize(obj.points, res);
    serialize(obj.triangles, res);
}

template<>
inline AltPlanet::PlanetGeometry deserialize<AltPlanet::PlanetGeometry>(const StreamType &res)
{
    StreamType::const_iterator it = res.cbegin();
    AltPlanet::PlanetGeometry obj;
    obj.points = deserialize<std::vector<vmath::Vector3>>(it, res.cend());
    obj.triangles = deserialize<std::vector<gfx::Triangle>>(it, res.cend());
    return obj;
}

} // namespace Serial

#endif // ALTPLANET_H
