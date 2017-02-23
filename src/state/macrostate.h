#ifndef MACROSTATE_H
#define MACROSTATE_H

#include "../common/gfx_primitives.h"

//namespace state {

//namespace macro {

struct SceneData {
    std::vector<vmath::Vector3> alt_planet_points;
    std::vector<gfx::Triangle> alt_planet_triangles;

    std::vector<vmath::Vector3> alt_ocean_points;
    std::vector<gfx::Triangle> alt_ocean_triangles;

    std::vector<vmath::Vector3> alt_lake_points;
    std::vector<gfx::Triangle> alt_lake_triangles;
    std::vector<gfx::Line> alt_river_lines;

    std::vector<gfx::TexCoords> alt_planet_texcoords;
    std::vector<gfx::TexCoords> clim_mat_texco;
};

//}

//}

#endif // MACROSTATE_H
