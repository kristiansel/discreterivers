#ifndef MACROSTATE_H
#define MACROSTATE_H

#include "../common/gfx_primitives.h"
#include "../altplanet/planetshapes.h"

#include "../altplanet/watersystem.h"
#include "../altplanet/civ/civ.h"

namespace state {

//namespace macro {

class MacroState {
public:
    // data
    std::vector<vmath::Vector3> alt_planet_points;
    std::vector<gfx::Triangle> alt_planet_triangles;

    std::vector<vmath::Vector3> alt_ocean_points;
    std::vector<gfx::Triangle> alt_ocean_triangles;

    std::vector<vmath::Vector3> alt_lake_points;
    std::vector<gfx::Triangle> alt_lake_triangles;
    std::vector<gfx::Line> alt_river_lines;

    std::vector<gfx::TexCoords> alt_planet_texcoords;
    std::vector<gfx::TexCoords> clim_mat_texco;

    std::vector<AltPlanet::LandWaterType> land_water_types;

    const AltPlanet::Shape::BaseShape * planet_base_shape;

    std::vector<AltPlanet::Civ::Resource> resource_locations;

    // methods
    ~MacroState() { delete planet_base_shape; }


    vmath::Vector3 getLocalUp(const vmath::Vector3 &point) const
    { return vmath::normalize(planet_base_shape->getGradDir(point)); }
};

//}

}

#endif // MACROSTATE_H
