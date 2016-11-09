#ifndef WATERSYSTEM_H
#define WATERSYSTEM_H

#include <vector>
#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"
#include "../common/gfx_primitives.h"
#include "planetshapes.h"
#include "altplanet.h"
#include "adjacency.h"

namespace AltPlanet
{

class WaterSystem
{
public:

    struct WaterGeometry
    {
        struct Ocean {
            std::vector<vmath::Vector3> points;
            std::vector<gfx::Triangle> triangles;
        } ocean;

        struct Freshwater {
            struct Lakes {
                std::vector<vmath::Vector3> points;
                std::vector<gfx::Triangle> triangles;
            } lakes;
            struct Rivers {
                std::vector<vmath::Vector3> points;
                std::vector<gfx::Line> lines;
            } rivers;
        } freshwater;
    };

    static WaterGeometry generateWaterSystem(const AltPlanet::PlanetGeometry &planet_geometry,
                                             const Shape::BaseShape &planet_shape,
                                             float ocean_fraction);


private:
    WaterSystem(); // non instantiable class

    struct AdjacencyInfo {
        std::vector<std::vector<int>> &pointTriAdjacency;
        std::vector<std::vector<int>> &pointPointAdjacency;
    };

    enum class LandWaterType
    {
        Sea,
        River,
        Lake,
        Land
    };

    struct GenOceanResult {
        WaterGeometry::Ocean ocean;
        std::vector<LandWaterType>  landWaterTypes;
        float seaLevel;
    };

    static GenOceanResult generateOcean(const std::vector<gfx::Triangle> &triangles,
                                            const std::vector<vmath::Vector3> &points,
                                            const std::vector<std::vector<int>> &point_tri_adjacency,
                                            const std::vector<std::vector<int>> &point_to_point_adjacency,
                                            float ocean_fraction,
                                            const Shape::BaseShape &planet_shape);

    static WaterGeometry::Freshwater generateFreshwater();

};

} // namespace AltPlanet

#endif // WATERSYSTEM_H
