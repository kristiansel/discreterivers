#include "watersystem.h"
#include "../common/graph_tools.h"
#include <functional>

using namespace graphtools;
typedef vmath::Vector3 Vector3; // Why not "using vmath::Vector3 as Vector 3", or something obvious?
using namespace gfx;
using namespace std; // should really be using just vector

typedef int point_index;
typedef int triangle_index;

namespace AltPlanet
{

WaterSystem::WaterGeometry WaterSystem::generateWaterSystem(const AltPlanet::PlanetGeometry &planet_geometry,
                                                            const Shape::BaseShape &planet_shape,
                                                            float ocean_fraction,
                                                            int num_river_springs)
{
    // Deconstruct input
    const vector<Vector3> &points = planet_geometry.points;
    const vector<Triangle> &triangles = planet_geometry.triangles;

    // Initialize and deconstruct output
    WaterSystem::WaterGeometry water_geometry;
    WaterSystem::WaterGeometry::Ocean &ocean = water_geometry.ocean;
    WaterSystem::WaterGeometry::Freshwater &freshwater = water_geometry.freshwater;


    // generate adjacency
    // TODO: Fix lack of typesafety for these point and triangle indices
    // TODO: Dislodge the dependency on "topology" source files
    vector<vector<int>> point_to_point_adjacency = Adjacancy::createAdjacencyList(points, triangles);
    vector<vector<int>> point_tri_adjacency = Adjacancy::createPointToTriAdjacency(points, triangles);
    vector<vector<int>> tri_to_tri_adjacency = Adjacancy::createTriToTriAdjacency(triangles, point_tri_adjacency);

    // generate ocean
    WaterSystem::GenOceanResult ocean_result = generateOcean(planet_geometry.triangles, planet_geometry.points,
                                                             point_tri_adjacency, point_to_point_adjacency, ocean_fraction,
                                                             planet_shape);
    // Woops copy
    ocean = ocean_result.ocean;

    // generate lakes and rivers
    freshwater = generateFreshwater(&ocean_result.landWaterTypes, triangles, points, point_to_point_adjacency,
                                    point_tri_adjacency, num_river_springs, planet_shape);

    return water_geometry;

}

const float max_ocean_fraction = 0.95f;
const float min_ocean_fraction = 0.00f;


WaterSystem::GenOceanResult WaterSystem::generateOcean(const vector<Triangle> &triangles,
                                                const vector<Vector3> &points,
                                                const vector<vector<int>> &point_tri_adjacency,
                                                const vector<vector<int>> &point_to_point_adjacency,
                                                float ocean_fraction,
                                                const Shape::BaseShape &planet_shape)
{
    WaterSystem::GenOceanResult result_out;
    std::vector<vmath::Vector3> &ocean_points = result_out.ocean.points;
    std::vector<gfx::Triangle> &ocean_triangles = result_out.ocean.triangles;
    std::vector<LandWaterType>  &point_land_water_types = result_out.landWaterTypes;
    float &sealevel_radius = result_out.seaLevel;
    // TODO: Generalize to a potential function not equal to sphere radius, but one reflecting the planet in question (in progress)

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
        float length = planet_shape.getHeight(points[i]);
        if (length < smallest_length) { start_point = i; smallest_length = length; }
    }

    // do a search to certain depth, TODO: change to % triangle cover
    auto flow_graph = make_graph(points, point_to_point_adjacency);

    auto heuristic = [&](const int &i)
    {
        return planet_shape.getHeight(points[i]);
    };

    auto it = flow_graph.search(start_point, heuristic);

    // initialize the point_sea_water_types
    point_land_water_types = vector<LandWaterType>(points.size(), LandWaterType::Land);

    // save the search sequence
    vector<int> search_sequence;
    search_sequence.push_back(it.get_index());

    // save list of triangle indices
    vector<int> search_tris;

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

        latest_ocean_lvl = planet_shape.getHeight(points[this_index]);
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
        int i_triangle = search_tris[i];
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
        planet_shape.setHeight(ocean_points[i], highest_ocean_lvl);
    }

    return result_out;
}

template<class T>
void push_back_if_unique(std::vector<T> &v, const T &e)
{
    if (std::find(v.begin(), v.end(), e) == v.end()) v.push_back(e);
}


WaterSystem::WaterGeometry::Freshwater WaterSystem::generateFreshwater(std::vector<WaterSystem::LandWaterType>  * const point_land_water_types,
                                                                       const std::vector<gfx::Triangle> &triangles,
                                                                       const std::vector<vmath::Vector3> &points,
                                                                       const std::vector<std::vector<int>> &point_to_point_adjacency,
                                                                       const std::vector<std::vector<int>> &point_tri_adjacency,
                                                                       const unsigned int n_springs,
                                                                       const Shape::BaseShape &planet_shape)
{
    WaterGeometry::Freshwater freshwater;
    std::vector<vmath::Vector3> &lake_points = freshwater.lakes.points;
    std::vector<gfx::Triangle> &lake_triangles = freshwater.lakes.triangles;
    std::vector<gfx::Line> &river_lines = freshwater.rivers.lines;

    for (int i_springs = 0; i_springs<n_springs; i_springs++)
    {
        point_index start_point;
        do
        {
            start_point = rand() % points.size();
        } while ((*point_land_water_types)[int(start_point)] != LandWaterType::Land);
        // The start_point should be guaranteed to be on land

        auto flow_graph = make_graph(points, point_to_point_adjacency);

        auto heuristic = [&](const point_index &i)
        {
            return planet_shape.getHeight(points[i]);
        };

        auto it = flow_graph.search(start_point, heuristic);

        // save the search sequence
        std::vector<point_index> search_sequence;
        search_sequence.push_back(it.get_index());

        // save the search result bi-directional graph
        std::vector<optional<point_index>> search_parents(points.size());
        std::vector<std::vector<point_index>> search_children(points.size());

        while ((*point_land_water_types)[it.get_index()] == LandWaterType::Land && !it.search_end())
        {
            ++it;
            point_index this_index = it.get_index();

            search_sequence.push_back(this_index);

            optional<point_index> parent_index = it.get_parent_index();
            search_parents[this_index] = parent_index;
            if (parent_index.exists()) search_children[parent_index.get()].push_back(this_index);
        }

        // reverse the search and set the height to water height
        std::vector<optional<float>> water_height(points.size());

        { // separate scope for safety

            float highest_water_level = 0.0f;

            for (auto rit = search_sequence.rbegin(); rit!=search_sequence.rend(); rit++)
            {
                point_index rev_search_index = *rit;

                if (planet_shape.getHeight(points[rev_search_index]) > highest_water_level)
                {
                    highest_water_level = planet_shape.getHeight(points[rev_search_index]);
                }

                // set the highest water level
                water_height[rev_search_index] = make_optional(highest_water_level);
            }
        }

        // all drainage system points should now have a water level

        // store resulting triangles
        std::vector<gfx::Triangle> this_lake_triangles;

        { // separate scope for safety

            point_index for_search_index = start_point;

            std::function<void (point_index)> do_for_children = [&] (point_index i_p)
            {
                // assume water_height[i_p] exists!
                if (water_height[i_p].get() > planet_shape.getHeight(points[i_p])) // if water height > terrain hight
                {
                    // the point is a lake point
                    (*point_land_water_types)[i_p] == LandWaterType::Lake;

                    // iterate over all triangles adjacent to point
                    for (const auto &adj_tri : point_tri_adjacency[i_p])
                    {
                        push_back_if_unique(this_lake_triangles, triangles[int(adj_tri)]);
                    }

                    // if the parent exists...
                    if (search_parents[i_p].exists())
                    {
                        point_index prev = search_parents[i_p].get();
                        // ...and is a river point...
                        if (!(water_height[prev].get() > planet_shape.getHeight(points[prev])))
                        {
                            // ...add a river line as well!
                            river_lines.push_back(gfx::Line{prev, i_p});
                        }
                    }
                }
                else
                {
                    // the point is a river point
                    (*point_land_water_types)[i_p] == LandWaterType::River;

                    // if the point has a parent, make a river
                    if (search_parents[i_p].exists())
                    {
                        river_lines.push_back(gfx::Line{search_parents[i_p].get(), i_p});
                    }

                }

                for (const auto &i_c : search_children[i_p]) do_for_children(i_c);
            };

            do_for_children(for_search_index);
        }

        // remap the triangles/points to create a sparse point/triangle representation for just the lakes
        std::vector<point_index> point_lake_map(points.size(), -1);

        for (int i = 0 ; i<this_lake_triangles.size(); i++)
        {
            float lake_height = -1.0f;
            for (int j = 0; j<3; j++)
            {
                point_index i_p_unmapped = (this_lake_triangles)[i][j];
                if (water_height[i_p_unmapped].exists()) lake_height = water_height[i_p_unmapped].get();
            }
            assert(lake_height > 0.0f);

            for (int j = 0; j<3; j++)
            {
                point_index i_p_unmapped = (this_lake_triangles)[i][j];
                //float lake_height = water_height[i_p_unmapped].get();

                // Not all points in a sea triangle are below sealevel, therefore extra check
                if (planet_shape.getHeight(points[i_p_unmapped]) < lake_height)
                {
                    (*point_land_water_types)[i_p_unmapped] = LandWaterType::Lake;
                }

                // map the point index
                if (point_lake_map[i_p_unmapped] == -1)
                {
                    point_lake_map[i_p_unmapped] = lake_points.size();

                    //vmath::Vector3 lake_point = lake_height * vmath::normalize(points[i_p_unmapped]);
                    vmath::Vector3 lake_point = points[i_p_unmapped];
                    planet_shape.setHeight(lake_point, lake_height);
                    lake_points.push_back(lake_point);
                }

                // change the triangle->point index from unmapped to mapped
                (this_lake_triangles)[i][j] = point_lake_map[i_p_unmapped];
            }
        }

        lake_triangles.insert(lake_triangles.end(), this_lake_triangles.begin(), this_lake_triangles.end());
    }

    return freshwater;
}

} // namespace AltPlanet
