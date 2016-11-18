#ifndef WATERSYSTEM_H
#define WATERSYSTEM_H

#include <vector>
#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"
#include "../common/gfx_primitives.h"
#include "../common/typetag.h"
#include "planetshapes.h"
#include "altplanet.h"
#include "adjacency.h"

namespace AltPlanet
{

class WaterSystem final // where is C#'s static class or private namespace when you need it???
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
                //std::vector<vmath::Vector3> points;
                std::vector<gfx::Line> lines;
            } rivers;
        } freshwater;
    };

    static WaterGeometry generateWaterSystem(const AltPlanet::PlanetGeometry &planet_geometry,
                                             const Shape::BaseShape &planet_shape,
                                             float ocean_fraction,
                                             int num_river_springs);


private:
    WaterSystem();

    // TODO: Proliferate the use of typesafe indices
    // typesafe tag type tags
    struct triangle_tag{};
    typedef ID<triangle_tag, int, -1> tri_index;

    typedef int point_index;
    /*struct point_tag{};
    typedef ID<point_tag, int, -1> point_index;*/

    struct AdjacencyInfo {
        std::vector<std::vector<tri_index>> &pointTriAdjacency;
        std::vector<std::vector<point_index>> &pointPointAdjacency;
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

    static WaterGeometry::Freshwater generateFreshwater(std::vector<LandWaterType>  * const point_land_water_types,
                                                        const std::vector<gfx::Triangle> &triangles,
                                                        const std::vector<vmath::Vector3> &points,
                                                        const std::vector<std::vector<int>> &point_point_adjacency,
                                                        const std::vector<std::vector<int>> &point_tri_adjacency,
                                                        const unsigned int n_springs,
                                                        const Shape::BaseShape &planet_shape);

};

} // namespace AltPlanet

#endif // WATERSYSTEM_H
