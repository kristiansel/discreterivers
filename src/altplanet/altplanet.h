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

struct Geometry
{
    ~Geometry(){std::cout << "Geometry Destructor called! &points[0]: " << &points[0] << std::endl;}
    std::vector<vmath::Vector3> points;
    std::vector<gfx::Triangle> triangles;
};

Geometry generate(unsigned int n_points, const Shape::BaseShape &planet_shape);

void pointsRepulse(std::vector<vmath::Vector3> &points, SpaceHash3D &spacehash, const Shape::BaseShape &planet_shape, float repulse_factor);

void pointsRepulse(std::vector<vmath::Vector3> &points, const Shape::BaseShape &planet_shape, float repulse_factor);

}

namespace Serial
{

// should really give these their own namespace
template<>
inline void serialize<AltPlanet::Geometry>(const AltPlanet::Geometry &obj, StreamType &res)
{
    serialize(obj.points, res);
    serialize(obj.triangles, res);
}

template<>
inline AltPlanet::Geometry deserialize<AltPlanet::Geometry>(const StreamType &res)
{
    StreamType::const_iterator it = res.cbegin();
    AltPlanet::Geometry obj;
    obj.points = deserialize<std::vector<vmath::Vector3>>(it, res.cend());
    obj.triangles = deserialize<std::vector<gfx::Triangle>>(it, res.cend());
    return obj;
}

}

#endif // ALTPLANET_H
