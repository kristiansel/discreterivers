#include "watersystem.h"
#include "../common/graph_tools.h"
using namespace graphtools;
typedef vmath::Vector3 Vector3; // Why not "using vmath::Vector3 as Vector 3", or something obvious?
using namespace gfx;
using namespace std;

namespace AltPlanet
{

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

WaterSystem::WaterGeometry WaterSystem::generateWaterSystem(const AltPlanet::PlanetGeometry &planet_geometry,
                                                            const Shape::BaseShape &planet_shape,
                                                            float ocean_fraction)
{
    // Initialize and deconstruct output
    WaterSystem::WaterGeometry water_geometry;
    WaterSystem::WaterGeometry::Ocean &ocean = water_geometry.ocean;
    WaterSystem::WaterGeometry::Freshwater &freshwater = water_geometry.freshwater;

    // generate adjacency
    // TODO: Dislodge the dependency on "topology" source files
    vector<vector<int>> point_to_point_adjacency;  // Lack of typesafety for these
    vector<vector<tri_index>> point_tri_adjacency;       // indices really shine
    vector<vector<tri_index>> tri_to_tri_adjacency;      // through here

    Topology::createAdjacencyList(&point_to_point_adjacency, planet_geometry.points, planet_geometry.triangles);
    Topology::createPointToTriAdjacency(&point_tri_adjacency, planet_geometry.triangles, planet_geometry.points);
    Topology::createTriToTriAdjacency(&tri_to_tri_adjacency, planet_geometry.triangles, planet_geometry.points, point_tri_adjacency);

    // generate ocean
    WaterSystem::GenOceanResult ocean_result = generateOcean(planet_geometry.triangles, planet_geometry.points,
                                                             point_tri_adjacency, point_to_point_adjacency, ocean_fraction);

    // generate lakes and rivers

    return water_geometry;

}

const float max_ocean_fraction = 0.95f;
const float min_ocean_fraction = 0.00f;


WaterSystem::GenOceanResult WaterSystem::generateOcean(const vector<Triangle> &triangles,
                                                const vector<Vector3> &points,
                                                const vector<vector<tri_index>> &point_tri_adjacency,
                                                const vector<vector<int>> &point_to_point_adjacency,
                                                float ocean_fraction)
{
    WaterSystem::GenOceanResult result_out;
    std::vector<vmath::Vector3> &ocean_points = result_out.ocean.points;
    std::vector<gfx::Triangle> &ocean_triangles = result_out.ocean.triangles;
    std::vector<LandWaterType>  &point_land_water_types = result_out.landWaterTypes;
    float &sealevel_radius = result_out.seaLevel;
    // TODO: Remove these ugly pointers
    // TODO: Generalize to a potential function not equal to sphere radius, but one reflecting the planet in question

    // sanitize input
    if (ocean_fraction < min_ocean_fraction|| ocean_fraction >= max_ocean_fraction)
    {
        cerr << "ocean fraction " << ocean_fraction << " out of bounds (" << min_ocean_fraction << ", " << max_ocean_fraction << ")" << endl;
        ocean_fraction = 0.5f;
        cerr << "setting ocean fraction to " << ocean_fraction << endl;
    }

    // find the deepest point on the planet as starting point
    int start_point = -1;
    float smallest_length = numeric_limits<float>::max();
    for (int i = 0 ; i<points.size(); i++)
    {
        float length = vmath::length(points[i]);
        if (length<smallest_length) {start_point=i; smallest_length=length;}
    }

    // do a search to certain depth, TODO: change to % triangle cover
    auto flow_graph = make_graph(points, point_to_point_adjacency);

    auto heuristic = [&](const int &i)
    {
        return vmath::length(points[i]);
    };

    auto it = flow_graph.search(start_point, heuristic);

    // initialize the point_sea_water_types
    point_land_water_types = vector<LandWaterType>(points.size(), LandWaterType::Land);

    // save the search sequence
    vector<int> search_sequence;
    search_sequence.push_back(it.get_index());

    // save list of triangle indices
    vector<tri_index> search_tris;

    //float current_sea_level = length(points[it.get_index()]);
    //while (current_sea_level < sealevel_radius && !it.search_end())

    unsigned int num_ocean_points = 0;
    float current_sea_coverage = 0.f;
    float highest_ocean_lvl = 0.f;
    float latest_ocean_lvl = 0.f;
    int rising_global_sealevel = true;
    while ((  current_sea_coverage < ocean_fraction || (!rising_global_sealevel) ) &&
            !it.search_end())
    {
        ++it;

        int this_index = it.get_index();
        search_sequence.push_back(this_index);

        // set the land/water type
        point_land_water_types[this_index] = LandWaterType::Sea;

        //current_sea_level = length(points[it.get_index()]);
        num_ocean_points++;
        current_sea_coverage = (float)(num_ocean_points)/(float)(points.size());

        latest_ocean_lvl = vmath::length(points[this_index]);
        if (latest_ocean_lvl > highest_ocean_lvl)
        {
            rising_global_sealevel = true;
            highest_ocean_lvl = latest_ocean_lvl;
        }
        else
        {
            rising_global_sealevel = false;
        }

        // save triangles (contains duplicates)
        for (const auto &i_t : point_tri_adjacency[int(this_index)])
            search_tris.push_back( i_t );
    }
    sealevel_radius = latest_ocean_lvl;

    cout << "final ocean level: " << latest_ocean_lvl << endl;
    cout << "final ocean coverage: " << current_sea_coverage << endl;

    // remove duplicate triangles
    sort(search_tris.begin(), search_tris.end());
    auto last = unique(search_tris.begin(), search_tris.end());
    search_tris.erase(last, search_tris.end());

    // remap the triangles/points to create a sparse point/triangle representation for just the ocean
    vector<int> point_ocean_map(points.size(), -1);

    ocean_triangles.resize(search_tris.size());

    for (int i = 0 ; i<search_tris.size(); i++)
    {
        tri_index i_triangle = search_tris[i];
        for (int j = 0; j<3; j++)
        {
            int i_p_unmapped = triangles[int(i_triangle)][j];

            // map the point index
            if (point_ocean_map[i_p_unmapped] == -1)
            {
                point_ocean_map[i_p_unmapped] = ocean_points.size();
                ocean_points.push_back(points[i_p_unmapped]);
            }

            // change the triangle->point index from unmapped to mapped
            ocean_triangles[i][j] = point_ocean_map[i_p_unmapped];
        }
    }

    // normalise the ocean points length to sea level
    for (int i = 0 ; i<ocean_points.size(); i++)
    {
        ocean_points[i] = highest_ocean_lvl * normalize(ocean_points[i]);
    }

    return result_out;
}

WaterSystem::WaterGeometry::Freshwater WaterSystem::generateFreshwater()
{

}

} // namespace AltPlanet
