#include "planet.h"

#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>

#include "../common/gfx_primitives.h"
#include "vectorgeometry.h"

namespace vgeom = VectorGeometry;
namespace vmath = Vectormath::Aos;

void findCriticalPoints(std::vector<TopoTypeIndexPair> * const critical_point_type,
                        std::vector<point_index> * const maxima,
                        std::vector<point_index> * const minima,
                        std::vector<point_index> * const saddles,
                        const std::vector<vmath::Vector3> &points,
                        const std::vector<gfx::Triangle> &triangles,
                        const std::vector<ConnectionList> &edges);

void findWatersheds(// std::vector<watershed_index> * const watershed_indices,
                    std::vector<SaddleList> * const minimum_to_saddle_adjacency,
                    std::vector<MinimaList> * const saddle_to_minimum_adjecency,
                    const std::vector<vmath::Vector3> &points,
                    const std::vector<ConnectionList> &point_to_point_adjacency_list,
                    const std::vector<TopoTypeIndexPair> &critical_point_type,
                    const std::vector<point_index> &minima,
                    const std::vector<point_index> &saddles);

template<class PotentialFunc>
void createFlowAdjacencyLists(  std::vector<ConnectionList> * const flow_down_adjacency,
                                std::vector<ConnectionList> * const flow_up_adjacency,
                                const std::vector<vmath::Vector3> &points,
                                const std::vector<ConnectionList> &connectivity_adjacency,
                                PotentialFunc pot_func);

// set these as planet methods

void createOceanGeometry(std::vector<vmath::Vector3> * const ocean_points,
                         std::vector<gfx::Triangle> * const ocean_triangles,
                         std::vector<LandWaterType>  * const point_land_water_types,
                         float * const sealevel_radius,
                         const std::vector<gfx::Triangle> &triangles,
                         const std::vector<vmath::Vector3> &points,
                         const std::vector<std::vector<tri_index>> &point_tri_adjacency,
                         const std::vector<std::vector<point_index>> &point_to_point_adjacency,
                         const float ocean_fraction);

void createLakesAndRivers(std::vector<vmath::Vector3> * const lake_points,
                          std::vector<gfx::Triangle> * const lake_triangles,
                          std::vector<gfx::Line> * const river_lines,
                          std::vector<LandWaterType>  * const point_land_water_types,
                          const std::vector<gfx::Triangle> &triangles,
                          const std::vector<vmath::Vector3> &points,
                          const std::vector<ConnectionList> &point_to_point_adjacency,
                          const std::vector<ConnectionList> &flow_down_adjacency,
                          const std::vector<ConnectionList> &flow_up_adjacency,
                          const std::vector<std::vector<tri_index>> &point_tri_adjacency,
                          const std::vector<std::vector<tri_index>> &tri_tri_adjacency,
                          const unsigned int n_springs);


inline float get_height(const vmath::Vector3 &p)
{
    return vmath::length(p);
}

Planet::Planet(const float radius,
               const int subdivision_level,
               const float ocean_fraction,
               const unsigned int n_springs,
               const int seed)
    //: mPointsDirty(true)
{
    srand(seed);

    //createIcoSphereGeometry(&mPoints, &mTriangles, radius, 8);
    Topology::createIcoSphereGeometry(&mPoints, &mNormals, &mTriangles, &mSubdTriangles, radius, subdivision_level);

    Topology::createAdjacencyList(&mPointToPointAdjacencyList, mPoints, mTriangles);

    // these functions are of dubious necessity-------------------------------------------------
    createFlowAdjacencyLists(&mFlowDownAdjacency, &mFlowUpAdjacency, mPoints,
                             mPointToPointAdjacencyList, get_height);



    findCriticalPoints(&mPointsTopologicalType, &mMaxima, &mMinima, &mSaddles,
                       mPoints, mTriangles, mPointToPointAdjacencyList);
    // ..........................................................................................

    Topology::createPointToTriAdjacency(&mPointToTriAdjacencyList, mTriangles, mPoints);
    Topology::createTriToTriAdjacency(&mTriToTriAdjacencyList, mTriangles, mPoints, mPointToTriAdjacencyList);

    float sealevel_radius;
    createOceanGeometry(&mOceanPoints, &mOceanTriangles, &mPointsLandWaterType, &sealevel_radius,
                        mTriangles, mPoints, mPointToTriAdjacencyList, mPointToPointAdjacencyList,
                        ocean_fraction);

    createLakesAndRivers(&mLakePoints, &mLakeTriangles, &mRiverLines, &mPointsLandWaterType, mTriangles, mPoints, mPointToPointAdjacencyList,
                         mFlowDownAdjacency, mFlowUpAdjacency,
                         mPointToTriAdjacencyList, mTriToTriAdjacencyList, n_springs);

}

// dirty hack... is this the sort of code one gets fired for?
inline const std::vector<vmath::Vector4> * const coerceVec3toVec4(std::vector<vmath::Vector3> &vec3s_in)
{
    // dirty dirty hack, depends on underlying implementation of Vector3 as a 128 bit aligend vector
    assert( sizeof(vmath::Vector3)==sizeof(vmath::Vector4) && sizeof(vmath::Vector3)==4*sizeof(float) );
    for (auto &v3 : vec3s_in)
    {
        vmath::Vector4 &v4_ref = (vmath::Vector4&)(v3);
        v4_ref.setW(1.0f);
    }

    return (std::vector<vmath::Vector4>*)(&vec3s_in);
}


const std::vector<vmath::Vector4> * const Planet::getPointsPtr()
{
    return coerceVec3toVec4(mPoints);
}

const std::vector<vmath::Vector4> * const Planet::getOceanVerticesPtr()
{
    return coerceVec3toVec4(mOceanPoints);
}

const std::vector<vmath::Vector4> * const Planet::getLakeVerticesPtr()
{
    return coerceVec3toVec4(mLakePoints);
}

float randFloatInRange(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

template<class T>
void push_back_if_unique(std::vector<T> &v, const T &e)
{
    if (std::find(v.begin(), v.end(), e) == v.end()) v.push_back(e);
}


#include <map>
template<class KeyType, class VectorType, class KeyFunc>
void createHistogramMap(std::map<KeyType, int> * const key_count_map, const VectorType &val_vector, KeyFunc key_func)
{
    for (auto const &val : val_vector)
    {
        KeyType key = key_func(val);
        auto map_it = key_count_map->find(key);
        if (map_it != key_count_map->end())
        {
            map_it->second+=1;
        }
        else
        {
            key_count_map->insert({key, 1});
        }
    }
}

enum class TopologicalType {
    None,
    Maximum,
    Minimum,
    Saddle,
    TriSaddle,
    // Valley,
    // Ridge
};


// !!! Why is this even necessary ???
int sane_modulus (int a, int b)
{
    if(b < 0) return sane_modulus(-a, -b);

    int ret = a % b;

    if(ret < 0) ret+=b;

    return ret;
}

template<class PotentialFunc>
void createFlowAdjacencyLists(  std::vector<ConnectionList> * const flow_down_adjacency,
                                std::vector<ConnectionList> * const flow_up_adjacency,
                                const std::vector<vmath::Vector3> &points,
                                const std::vector<ConnectionList> &connectivity_adjacency,
                                PotentialFunc pot_func)
{
    (*flow_down_adjacency) = std::vector<ConnectionList>(points.size());
    (*flow_up_adjacency) = std::vector<ConnectionList>(points.size());

    for (int i_point = 0; i_point<points.size(); i_point++)
    {
//        bool debug = false;
//        if (i_point==1) debug = true;

//        if(debug)
//        {
//            std::cout << "DEBUG:" << std::endl;
//            std::cout << "point: " << i_point << ":" << pot_func(points[i_point]())<< std::endl;
//            std::cout << "  adj: ";
//            for (const point_index &p_i : connectivity_adjacency[i_point]) std::cout << p_i << ":" << pot_func(points[p_i]()) << " ";
//            std::cout << std::endl;
//        }

        const std::vector<point_index> &adjacent = connectivity_adjacency[i_point];

        for (int i_adj = 0; i_adj<adjacent.size(); i_adj++)
        {
            // if it adjacent point lies lower than neighbours and lower than current
            // then it is a preferred flow down route
            const point_index i_point_adj       = adjacent[i_adj];
            const point_index i_point_adj_left  = adjacent[sane_modulus(i_adj+1, adjacent.size())]; // ccw numbering, left is +1
            const point_index i_point_adj_right = adjacent[sane_modulus(i_adj-1, adjacent.size())]; // ccw numbering, right is -1

            const vmath::Vector3 p          = points[i_point_adj];
            const vmath::Vector3 p_left     = points[i_point_adj_left];
            const vmath::Vector3 p_right    = points[i_point_adj_right];

            bool lower_than_parent = pot_func(p) < pot_func(points[i_point]);
            bool lower_than_neighbours = (pot_func(p) < pot_func(p_left)) && (pot_func(p) < pot_func(p_right));

//            if(debug)
//            {
//                std::cout << "   lower_than_parent:" << lower_than_parent << std::endl;
//                std::cout << "   lower_than_neighbours:" << lower_than_neighbours << std::endl;
//                std::cout << "      pot_func(p_right):" << pot_func(p_right) << std::endl;
//                std::cout << "      pot_func(p):" << pot_func(p) << std::endl;
//                std::cout << "      pot_func(p_left):" << pot_func(p_left) << std::endl;
//                std::cout << "         sane_modulus(i_adj-1, adjacent.size()):" << sane_modulus(i_adj-1, adjacent.size()) << std::endl;
//                std::cout << "         i_adj:" << i_adj << std::endl;
//                std::cout << "         sane_modulus(i_adj+1, adjacent.size()):" << sane_modulus(i_adj+1, adjacent.size()) << std::endl;
//            }

            if (lower_than_parent && lower_than_neighbours)
            {
                (*flow_down_adjacency)[i_point].push_back(i_point_adj);
                (*flow_up_adjacency)[i_point_adj].push_back(i_point);
            }
        }

//        if(debug)
//        {
//            std::cout << "END:DEBUG" << std::endl;
//        }
    }

#define DEBUG_FLOWADJACENCY
#ifdef DEBUG_FLOWADJACENCY
    // Verify that all points are connected to the flow graph
    for (int i_point = 0; i_point<points.size(); i_point++)
    {
        const int flow_down_size = (*flow_down_adjacency)[i_point].size();
        const int flow_up_size = (*flow_up_adjacency)[i_point].size();

//        std::cout << "point: " << i_point << ":" << pot_func(points[i_point]())<< std::endl;
//        std::cout << "  adj: ";
//        for (const point_index &p_i : connectivity_adjacency[i_point]) std::cout << p_i << ":" << pot_func(points[p_i]()) << " ";
//        std::cout << std::endl;
//        std::cout << "   down: ";
//        for (const point_index &p_i : (*flow_down_adjacency)[i_point]) std::cout << p_i << " ";
//        std::cout << std::endl;
//        std::cout << "     up: ";
//        for (const point_index &p_i : (*flow_up_adjacency)[i_point]) std::cout << p_i << " ";
//        std::cout << std::endl;

        bool flows_either_up_or_down = (flow_down_size + flow_up_size) > 0;
        //assert(flows_either_up_or_down);
    }

    // inspect some properties:
    // find the distribution of number of down paths
    std::map<int, int> downpath_count_map;
    createHistogramMap(&downpath_count_map, (*flow_down_adjacency),
                       [](const std::vector<point_index> &e) {return e.size();});
    int total_visit_count = 0;
    int total_point_count = 0;
    for (auto const &down_count_pair : downpath_count_map)
    {
        std::cout << down_count_pair.second << " points have " << down_count_pair.first << " preferred down paths" << std::endl;
        total_visit_count += down_count_pair.first * down_count_pair.second;
        total_point_count += down_count_pair.second;
    }
    std::cout << "number of points " << total_point_count << std::endl;
    std::cout << "number of visits " << total_visit_count << std::endl;

    // of the points with two or more down directions, how many are being flowed to?
    int two_or_more_down = 0;
    int two_or_more_down_and_receiving = 0;
    int two_or_more_down_and_receiving_more_than_one = 0;

    for (int i_point = 0; i_point<points.size(); i_point++)
    {
        if ((*flow_down_adjacency)[i_point].size()>1)
        {
            two_or_more_down++;
            if ((*flow_up_adjacency)[i_point].size()>0)two_or_more_down_and_receiving++;
            if ((*flow_up_adjacency)[i_point].size()>1)two_or_more_down_and_receiving_more_than_one++;
        }
    }

    std::cout << "              two_or_more_down: " << two_or_more_down << std::endl;
    std::cout << "two_or_more_down_and_receiving: " << two_or_more_down_and_receiving << std::endl;
    std::cout << "two_or_more_down_and_receiving_more_than_one: " << two_or_more_down_and_receiving_more_than_one << std::endl;

#endif //#ifdef DEBUG_FLOWADJACENCY
}

#include "../common/typetag.h"
struct ridge_point_tag{};
typedef ID<ridge_point_tag, int, -1> ridge_point_id;

struct ridge_line_tag{};
typedef ID<ridge_line_tag, int, -1> ridge_line_id;



const std::vector<gfx::Line> * const Planet::getFlowLinesPtr()
{
    if (mFlowLines.size()==0)
    {
        for (int i = 0; i<mFlowDownAdjacency.size(); i++)
        {
            const auto &adj_list = mFlowDownAdjacency[i];
            for (const auto &i_adj : adj_list)
            {
                mFlowLines.push_back({i, i_adj});
            }
        }
    }

    return &mFlowLines;
}


#include "../common/graph_tools.h"
using namespace graphtools;

const float max_ocean_fraction = 0.95f;
const float min_ocean_fraction = 0.00f;

void createOceanGeometry(std::vector<vmath::Vector3> * const ocean_points,
                         std::vector<gfx::Triangle> * const ocean_triangles,
                         std::vector<LandWaterType>  * const point_land_water_types,
                         float * const sealevel_radius,
                         const std::vector<gfx::Triangle> &triangles,
                         const std::vector<vmath::Vector3> &points,
                         const std::vector<std::vector<tri_index>> &point_tri_adjacency,
                         const std::vector<std::vector<point_index>> &point_to_point_adjacency,
                         float ocean_fraction)
{
    // sanitize input
    if (ocean_fraction < min_ocean_fraction|| ocean_fraction >= max_ocean_fraction)
    {
        std::cerr << "ocean fraction " << ocean_fraction << " out of bounds (" << min_ocean_fraction << ", " << max_ocean_fraction << ")" << std::endl;
        ocean_fraction = 0.5f;
        std::cerr << "setting ocean fraction to " << ocean_fraction << std::endl;
    }

    // find the deepest point on the planet as starting point
    int start_point = -1;
    float smallest_length = std::numeric_limits<float>::max();
    for (int i = 0 ; i<points.size(); i++)
    {
        float length = vmath::length(points[i]);
        if (length<smallest_length) {start_point=i; smallest_length=length;}
    }

    // do a search to certain depth, TODO: change to % triangle cover
    auto flow_graph = make_graph(points, point_to_point_adjacency);

    auto heuristic = [&](const point_index &i)
    {
        return vmath::length(points[i]);
    };

    auto it = flow_graph.search(start_point, heuristic);

    // initialize the point_sea_water_types
    *point_land_water_types = std::vector<LandWaterType>(points.size(), LandWaterType::Land);

    // save the search sequence
    std::vector<point_index> search_sequence;
    search_sequence.push_back(it.get_index());

    // save list of triangle indices
    std::vector<tri_index> search_tris;

    //float current_sea_level = vmath::length(points[it.get_index()]);
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

        point_index this_index = it.get_index();
        search_sequence.push_back(this_index);

        // set the land/water type
        (*point_land_water_types)[this_index] = LandWaterType::Sea;

        //current_sea_level = vmath::length(points[it.get_index()]);
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
    *sealevel_radius = latest_ocean_lvl;

    std::cout << "final ocean level: " << latest_ocean_lvl << std::endl;
    std::cout << "final ocean coverage: " << current_sea_coverage << std::endl;

    // remove duplicate triangles
    std::sort(search_tris.begin(), search_tris.end());
    auto last = std::unique(search_tris.begin(), search_tris.end());
    search_tris.erase(last, search_tris.end());

    // remap the triangles/points to create a sparse point/triangle representation for just the ocean
    std::vector<point_index> point_ocean_map(points.size(), -1);

    (*ocean_triangles).resize(search_tris.size());

    for (int i = 0 ; i<search_tris.size(); i++)
    {
        tri_index i_triangle = search_tris[i];
        for (int j = 0; j<3; j++)
        {
            point_index i_p_unmapped = triangles[int(i_triangle)][j];

            // map the point index
            if (point_ocean_map[i_p_unmapped] == -1)
            {
                point_ocean_map[i_p_unmapped] = ocean_points->size();
                ocean_points->push_back(points[i_p_unmapped]);
            }

            // change the triangle->point index from unmapped to mapped
            (*ocean_triangles)[i][j] = point_ocean_map[i_p_unmapped];
        }
    }

    // normalise the ocean points length to sea level
    for (int i = 0 ; i<ocean_points->size(); i++)
    {
        (*ocean_points)[i] = highest_ocean_lvl * vmath::normalize((*ocean_points)[i]);
    }
}

template<class element_t>
inline void dump_vector(std::vector<element_t> vec_in, std::string label = "vector")
{
    std::cout << "printing " << label << ", length: " << vec_in.size() << std::endl;
    for (const auto &el : vec_in) {std::cout << el << std::endl;}
}

void createLakesAndRivers(std::vector<vmath::Vector3> * const lake_points,
                          std::vector<gfx::Triangle> * const lake_triangles,
                          std::vector<gfx::Line> * const river_lines,
                          std::vector<LandWaterType>  * const point_land_water_types,
                          const std::vector<gfx::Triangle> &triangles,
                          const std::vector<vmath::Vector3> &points,
                          const std::vector<ConnectionList> &point_to_point_adjacency,
                          const std::vector<ConnectionList> &flow_down_adjacency,
                          const std::vector<ConnectionList> &flow_up_adjacency,
                          const std::vector<std::vector<tri_index>> &point_tri_adjacency,
                          const std::vector<std::vector<tri_index>> &tri_tri_adjacency,
                          const unsigned int n_springs)
{
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
            return vmath::length(points[i]);
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

                if (vmath::length(points[rev_search_index]) > highest_water_level)
                {
                    highest_water_level = vmath::length(points[rev_search_index]);
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
                if (water_height[i_p].get() > vmath::length(points[i_p])) // if water height > terrain hight
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
                        if (!(water_height[prev].get() > vmath::length(points[prev])))
                        {
                            // ...add a river line as well!
                            river_lines->push_back(gfx::Line{prev, i_p});
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
                        river_lines->push_back(gfx::Line{search_parents[i_p].get(), i_p});
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
                if (vmath::length(points[i_p_unmapped]) < lake_height)
                {
                    (*point_land_water_types)[i_p_unmapped] = LandWaterType::Lake;
                }

                // map the point index
                if (point_lake_map[i_p_unmapped] == -1)
                {
                    point_lake_map[i_p_unmapped] = lake_points->size();

                    vmath::Vector3 lake_point = lake_height * vmath::normalize(points[i_p_unmapped]);
                    lake_points->push_back(lake_point);
                }

                // change the triangle->point index from unmapped to mapped
                (this_lake_triangles)[i][j] = point_lake_map[i_p_unmapped];
            }
        }

        lake_triangles->insert(lake_triangles->end(), this_lake_triangles.begin(), this_lake_triangles.end());
    }
}

// function with only intention to save lines of code
inline bool Planet::checkPointInTriIndex(vmath::Vector3 *b, const vmath::Vector3 point, const gfx::Triangle &triangle)
{
    vmath::Vector3 tp0 = mPoints[triangle[0]];
    vmath::Vector3 tp1 = mPoints[triangle[1]];
    vmath::Vector3 tp2 = mPoints[triangle[2]];

    vmath::Vector3 tri_cross = vgeom::triangleCross(tp0, tp1, tp2);
    //vmath::Vector3 n_unit_tri = vgeom::triangleUnitNormal(tp0, tp1, tp2);

    // project point onto triangle
    vmath::Vector3 p_proj = vgeom::linePlaneIntersection(point, point, tri_cross, tp0);

    // find the barycentric coordinates
    *b = vgeom::baryPointInTriangle(p_proj, tp0, tp1, tp2, tri_cross);

    return vgeom::pointInTriangle(*b);
}

vmath::Vector3 Planet::getSmoothPoint(vmath::Vector3 point, tri_index guess_triangle)
{
    // debug
//    std::cout << "Planet::getSmoothPoint" << std::endl << "point: ";
//    vmath::print(point);
//    std::cout << "tri: ";
//    for(int i=0;i<3;i++)vmath::print(mPoints[mTriangles[int(guess_triangle)][i]]);



    // get the guess triangle
    const gfx::Triangle &triangle = mTriangles[int(guess_triangle)];

    // find the barycentric coordinates
    vmath::Vector3 b;



    bool is_in_tri = checkPointInTriIndex(&b, point, triangle);

    // debug
//    std::cout << "b: ";
//    vmath::print(b);

    if (!is_in_tri)
    {
        // start a search for the triangle
        auto tri_graph = make_graph(mTriangles, mTriToTriAdjacencyList);

        // prioritize triangles close to the point in question
        auto heuristic = [&](const tri_index &i)
        {
            point_index p = mTriangles[int(i)][0];
            return vmath::lengthSqr(point-mPoints[p]);
        };

        auto it = tri_graph.search(guess_triangle, heuristic);

        do
        {
            ++it;
            tri_index i_tri = it.get_index();
            const gfx::Triangle &search_triangle = mTriangles[int(i_tri)];
            is_in_tri = checkPointInTriIndex(&b, point, search_triangle);
        }
        while (!is_in_tri && !it.search_end());

        assert( (!it.search_end()) );
    }

    return vgeom::findPointInTriangle(point, triangle, b, mPoints, mNormals);
}


void Planet::smoothSubdivideTriangle( std::vector<Vectormath::Aos::Vector3> * subd_points,
                            std::vector<gfx::Triangle> * subd_triangles,
                            tri_index tri_to_subd)
{
    //
}



int Planet::getSubdPointIndex(const point_index i1,
                      const point_index i2,
                      const tri_index triangle_index,
                      std::vector<vmath::Vector3> * const points,
                      IntpairIntMapType * const midpoints_cache)
{
    // ordering of subdivision edge points is irrelevant, so
    // order i1 and i2 according to relative magnitude to avoid
    // duplicates in cache, i.e. don't want both (i1,i2) and (i2,i1)
    point_index i_lo = i1<i2 ? i1 : i2;
    point_index i_hi = i1>i2 ? i1 : i2;
    std::pair<point_index, point_index> midpoint_key = {i_lo, i_hi};

    // check if point exists in cache
    auto midpoint_it = midpoints_cache->find(midpoint_key);

    if (midpoint_it != midpoints_cache->end())
    {
        // just return the point from the cache
        return midpoint_it->second;
    }
    else
    {
        // create point by averaging parents
        vmath::Vector3 mid_pt = 0.5f*((*points)[i_lo]+(*points)[i_hi]);

        // project the mid_pt onto the spline surface
        mid_pt = getSmoothPoint(mid_pt, triangle_index);

        points->push_back(
            mid_pt
        );
        int last_element_index = points->size() - 1;

        // add it to the cache
        midpoints_cache->insert( {midpoint_key, last_element_index} );
        return last_element_index;
    }
}

void Planet::getSmoothSubdGeometry(std::vector<vmath::Vector4> * vertices,
                                   std::vector<gfx::Triangle> * triangles, int subd_lvl)
{
    assert( ((subd_lvl>=0) && vertices && triangles) );

    std::cout << "subdividing smooth geometry..." << std::endl;

    // get the real geometry
    std::vector<vmath::Vector3> points = mPoints; // copy the existing points

    // temporary vector to store triangle subdivision levels
    std::vector<std::vector<gfx::Triangle>> subd_triangles;
    subd_triangles.push_back(mTriangles); // subd lvl zero

    // Subdivide triangles
    for (int k = 1; k < subd_lvl+1; k++)
    {
        // add another triangle subdivision level
        subd_triangles.push_back(std::vector<gfx::Triangle>());

        // Use a cache to keep points resulting from subidiving a shared edge
        IntpairIntMapType midpoints_cache;

        for (int i = 0; i < subd_triangles[k-1].size(); i++)
        {
            // original parent triangle index
            tri_index i_tri_orig = tri_index(i/(std::pow(4,k-1)));

            // get indices of the three new points (organized by sides)
            gfx::Triangle const &prev_triangle = subd_triangles[k-1][i];
            point_index new_points_indices[] = {
                getSubdPointIndex(prev_triangle[0], prev_triangle[1], i_tri_orig, &points, &midpoints_cache),
                getSubdPointIndex(prev_triangle[1], prev_triangle[2], i_tri_orig, &points, &midpoints_cache),
                getSubdPointIndex(prev_triangle[2], prev_triangle[0], i_tri_orig, &points, &midpoints_cache)
            };

            // create the subdivided triangles, using previously existing and new corners
            subd_triangles[k].push_back(
                {
                    prev_triangle[0],
                    new_points_indices[0],
                    new_points_indices[2]
                }
            );

            subd_triangles[k].push_back(
                {
                    prev_triangle[1],
                    new_points_indices[1],
                    new_points_indices[0]
                }
            );

            subd_triangles[k].push_back(
                {
                    prev_triangle[2],
                    new_points_indices[2],
                    new_points_indices[1]
                }
            );

            subd_triangles[k].push_back(
                {
                    new_points_indices[0],
                    new_points_indices[1],
                    new_points_indices[2]
                }
            );
        } // for (int i = 0; i < prev_subdlvl_size; i++)

        std::cout << "lvl: " << k << "/" << subd_lvl << std::endl;
    }

    // convert the points to vertices
    const std::vector<vmath::Vector4> * const points4_ptr = coerceVec3toVec4(points);
    *vertices = *points4_ptr;

    // copy the highest subdivision level to the resulting triangles
    *triangles = subd_triangles.back();

    std::cout << "...done!" << std::endl;
}

// shit really hit the fan from here down...


float lengthSqrd(const vmath::Vector3 &p)
{
    return vmath::lengthSqr(p);
}

bool lengthSqrdComp(const vmath::Vector3 &p1, const vmath::Vector3 &p2)
{
    return lengthSqrd(p1) < lengthSqrd(p2);
}

template<class MagnitudeFunction>
TopologicalType classifyPoint(const point_index i_p,
                                const std::vector<vmath::Vector3> &points,
                                const std::vector<ConnectionList> &adjacency_list,
                                MagnitudeFunction magn_func)
{

    const std::vector<point_index> &adjacent = adjacency_list[i_p];

    // map
    std::vector<bool> adjacent_point_smaller(adjacent.size());
    for (int i = 0; i<adjacent.size(); i++)
    {
        adjacent_point_smaller[i] =   magn_func(points[adjacent[i]])
                                    < magn_func(points[i_p]);
    }

    // reduce
    int number_of_flips = 0;
    for (int i = 0; i<adjacent_point_smaller.size(); i++)
    {
        int j = (i+1)%adjacent_point_smaller.size(); // 1,2,3,4,5,0
        if (adjacent_point_smaller[j]!=adjacent_point_smaller[i])
        {
            number_of_flips++;
        }
    }

    TopologicalType critical_point_type;
    switch (number_of_flips)
    {
    case 0: // all adjacent points are either greater or less than current point
    {
        if (adjacent_point_smaller[0]==true)
        {
            critical_point_type = TopologicalType::Maximum;
        }
        else
        {
            critical_point_type = TopologicalType::Minimum;
        }
        break;
    }
    case 2:
    {
        critical_point_type = TopologicalType::None;
        break;
    }
    case 4:
    {
        critical_point_type = TopologicalType::Saddle;
        break;
    }
    case 6:
    {
        critical_point_type = TopologicalType::TriSaddle;
        break;
    }
    default:
    {
        critical_point_type = TopologicalType::None;
        std::cerr << "strange critical point, index: " << i_p << ". Number of flips: " << number_of_flips << std::endl;
        break;
    }
    }

    return critical_point_type;
}

void findCriticalPoints(std::vector<TopoTypeIndexPair> * const critical_point_type,
                        std::vector<point_index> * const maxima,
                        std::vector<point_index> * const minima,
                        std::vector<point_index> * const saddles,
                        const std::vector<vmath::Vector3> &points,
                        const std::vector<gfx::Triangle> &triangles,
                        const std::vector<ConnectionList> &edges)
{
    // TODO: check integrity of inputs
    critical_point_type->resize(points.size());


    // count tri-saddles for debug
    int tri_saddles_count=0;

    for (int i = 0; i<points.size(); i++)
    {
        auto point_classified = classifyPoint(i,
                                              points,
                                              edges,
                                              lengthSqrd);

        (*critical_point_type)[i].topo_type = point_classified;

        switch (point_classified)
        {
        case TopologicalType::Maximum:
        {
            (*critical_point_type)[i].topo_index = maxima->size();
            maxima->push_back(i);
            break;
        }
        case TopologicalType::Minimum:
        {
            (*critical_point_type)[i].topo_index = minima->size();
            minima->push_back(i);
            break;
        }
        case TopologicalType::Saddle:
        {
            (*critical_point_type)[i].topo_index = saddles->size();
            saddles->push_back(i);
            break;
        }
        case TopologicalType::TriSaddle: // do these need to be treated specially?
        {
            (*critical_point_type)[i].topo_index = saddles->size();
            saddles->push_back(i);
            tri_saddles_count++;
            break;
        }
        default:
        {
            break;
        }
        }
    }

    // don't sort the min/max/saddle vectors, that would invalidate the topo_index's

    std::cout << "found " << maxima->size() << " maxima" << std::endl;
    std::cout << "found " << minima->size() << " minima" << std::endl;
    std::cout << "found " << saddles->size() << " saddles (" << tri_saddles_count << " of which are trisaddles)" << std::endl;
}



#include <queue>

struct DistanceParentPair
{
    int distance;
    point_index parent;
};

const point_index unknown_parent = -1;
const point_index no_parent = -999;
const point_index no_point = -99999;

const int int_max = std::numeric_limits<int>::max();

template<class DistanceFunc, class SearchPredicate, class VisitorFunc>
void findAdjacentSaddles(std::vector<DistanceParentPair> * const points_searched,
                         minimum_index root_minimum,
                         const std::vector<vmath::Vector3> &points,
                         const std::vector<ConnectionList> &point_to_point_adjacency_list,
                         const std::vector<TopoTypeIndexPair> &critical_point_type,
                         DistanceFunc dist_func,
                         SearchPredicate search_predicate,
                         VisitorFunc visitor_func)
{

    // Create ordering function from dist_func:
    auto lengthSqrdCompByIndex = [&] (point_index i1, point_index i2)
    {
        return dist_func(points[i1]) < dist_func(points[i2]);
    };

    // add the global minimum to the open list
    std::priority_queue<point_index, std::vector<point_index>, decltype(lengthSqrdCompByIndex)> open_points_queue(lengthSqrdCompByIndex);
    open_points_queue.push(root_minimum);

    // explore the graph                                                distance, parent
    (*points_searched) = std::vector<DistanceParentPair>(points.size(), {int_max, unknown_parent});
    (*points_searched)[root_minimum] = {0, no_parent};
    visitor_func(root_minimum, no_parent);

    while (!open_points_queue.empty())
    {
        point_index current_point = open_points_queue.top();
        open_points_queue.pop();

        for (auto const &adjacent_point : point_to_point_adjacency_list[current_point])
        {
            // if already visited
            if ((*points_searched)[adjacent_point].parent != unknown_parent) // already explored
            {
                // do nothing
            }
            else
            {
                // log distance and parent
                (*points_searched)[adjacent_point].distance = (*points_searched)[current_point].distance+1;
                (*points_searched)[adjacent_point].parent = current_point;

                // only keep searching if new point is upwards of parent and not a saddle
                if (search_predicate(adjacent_point, current_point))
                {
                    visitor_func(adjacent_point, current_point);

                    open_points_queue.push(adjacent_point);
                }
                // else don't add this to search queue or do anything
            } // if explored
        } // for adjacent
    } // while search
}

#define DEBUG_WATERSHEDS

void findWatersheds(// std::vector<watershed_index> * const watershed_indices,
                    std::vector<SaddleList> * const minimum_to_saddle_adjacency,
                    std::vector<MinimaList> * const saddle_to_minimum_adjecency,
                    const std::vector<vmath::Vector3> &points,
                    const std::vector<ConnectionList> &point_to_point_adjacency_list,
                    const std::vector<TopoTypeIndexPair> &critical_point_type,
                    const std::vector<point_index> &minima,
                    const std::vector<point_index> &saddles)
{
    // check input

    // initialize vectors
    minimum_to_saddle_adjacency->resize(minima.size());
    saddle_to_minimum_adjecency->resize(saddles.size());

    // start at the global minimum
    auto lengthSqrdCompByIndex = [&] (point_index i1, point_index i2)
    {
        return lengthSqrdComp(points[i1], points[i2]);
    };
    std::vector<point_index> minima_sorted = minima;
    std::sort(minima_sorted.begin(), minima_sorted.end(), lengthSqrdCompByIndex);
    point_index global_min = minima_sorted[0];

//    std::cout << "global minimum, point_index = " << global_min
//              << ", radius = " << vmath::length(points[global_min]()) << std::endl;

    // point_index search_root = global_min;

    // temporary storage for saddle paths (in order to verify consistency)
    //std::vector<std::vector<point_index>> saddle_to_min_paths;

    // list connecting points to adjacent minima
    // needs to be a vector as ridge and saddle points will connect to more than one minimum
    struct ParentMinimumPair { point_index parent; minimum_index minimum; };
    std::vector<std::vector<ParentMinimumPair>> points_parents_minima(points.size());

#ifdef DEBUG_WATERSHEDS
    // temporary storage of searched points for consistency checking
    std::vector<point_index> points_searched_growinglist;
#endif

    // templated approach
    auto dist_func = [](const vmath::Vector3 &p1) {return vmath::length(p1);};

    auto could_in_theory_flow_to_min_predicate = [&](const point_index i_search, const point_index i_parent)
    {
        return dist_func(points[i_search]) > dist_func(points[i_parent]);
    };

    auto prefers_to_flow_to_min_predicate = [&](const point_index i_search, const point_index i_parent)
    {
        // ok if the i_parent is the preferred flow direction from i_search
        float lowest_flow_to_height = dist_func(points[i_parent]);
        point_index i_pref_flow = no_point;
        for (const point_index i_adj : point_to_point_adjacency_list[i_search])
        {
            float adj_point_height = dist_func(points[i_adj]);
            if (adj_point_height<lowest_flow_to_height)
            {
                lowest_flow_to_height = adj_point_height;
                i_pref_flow = i_adj;
            }
        }

        // bool tried_to_flow_from_lower = (i_pref_flow==no_point);
        // assert(!tried_to_flow_from_minimum);

        return i_parent==i_pref_flow;
    };

    auto kernel_func_uncurried = [&](   const point_index i_search,
                                        const point_index i_parent,
                                        const point_index i_search_root,
                                        const point_index i_root_in_minima )
    {

#ifdef DEBUG_WATERSHEDS
        points_searched_growinglist.push_back(i_search);
#endif

        // update the points_parents_minima
        points_parents_minima[i_search].push_back({i_parent, i_root_in_minima});

        if (critical_point_type[i_search].topo_type == TopologicalType::Saddle)
        {
            (*minimum_to_saddle_adjacency)[i_root_in_minima].push_back(critical_point_type[i_search].topo_index);

            // back_trace to parent min and save path (this is not necessary. Should be able to backtrace afterwards...
            // ... through parents saved in points_searched
//            saddle_to_min_paths.push_back(std::vector<point_index>());
//            std::vector<point_index> &saddle_to_min_path = saddle_to_min_paths.back();

//            point_index backtrace_point = i_search;
//            while(points_searched[backtrace_point].parent != no_parent)
//            {
//                // std::cout << "backtrace_point = " << backtrace_point << std::endl;
//                saddle_to_min_path.push_back(backtrace_point);
//                backtrace_point = points_searched[backtrace_point].parent;
//            }
//            // add min index
//            saddle_to_min_path.push_back(i_search_root);
        }
    };


    //
    for (const point_index current_minimum : minima)
    {
        // get the index of the minimim in the "minima" vector
        minimum_index min_in_minima = std::find(minima.begin(), minima.end(), current_minimum) - minima.begin();

        auto kernel_func = std::bind(kernel_func_uncurried, std::placeholders::_1, std::placeholders::_2, current_minimum, min_in_minima);

        std::vector<DistanceParentPair> points_searched;

        findAdjacentSaddles(&points_searched, current_minimum, points, point_to_point_adjacency_list, critical_point_type,
                            dist_func,
                            // could_in_theory_flow_to_min_predicate,
                            prefers_to_flow_to_min_predicate,
                            kernel_func);
    }

#ifdef DEBUG_WATERSHEDS

#define DEBUG_BACKTRACE

    // auto find_parent_towards_minimum = [](ParentMinimumPair pm_pair) {return
    // verify the integrity of each trace... (where number of visits > 1)
    for (int i = 0; i<points_parents_minima.size(); i++)
    //for (const auto &parent_minima_pairs : points_parents_minima)
    {
        const auto &parent_minima_pairs = points_parents_minima[i];
        if (parent_minima_pairs.size()>1)
        //if (true)
        {
            for (const auto &parent_minimum_pair : parent_minima_pairs)
            {
                float prev_height = std::numeric_limits<float>::max();
                point_index current_point = i;
                point_index current_min_target = minima[parent_minimum_pair.minimum];
#ifdef DEBUG_BACKTRACE
                std::cout << "tracing from: " << i << " at " << dist_func(points[i]) << " to min point "
                          << current_min_target << " at " << dist_func(points[current_min_target]) << std::endl;
#endif // #ifdef DEBUG_BACKTRACE
                while (current_point!=current_min_target)
                {
                    float this_height = dist_func(points[current_point]);
#ifdef DEBUG_BACKTRACE
                    std::cout << "  current: " << current_point << " at " << this_height << std::endl;
#endif // #ifdef DEBUG_BACKTRACE
                    assert( this_height <= prev_height); // check that points go in the right direction
                    prev_height = this_height;
                    // current_point = parents_minima_pair[current_point].parent;
                    auto pm_pair_it = points_parents_minima[current_point].begin();
#ifdef DEBUG_BACKTRACE
                    std::cout << "    ways to go: " << std::endl;
#endif // #ifdef DEBUG_BACKTRACE
                    while (pm_pair_it != points_parents_minima[current_point].end())
                    {
#ifdef DEBUG_BACKTRACE
                        std::cout << "      (" << pm_pair_it->parent << ", to min: " << minima[pm_pair_it->minimum] << ")"<< std::endl;
#endif // #ifdef DEBUG_BACKTRACE
                        if (minima[pm_pair_it->minimum] == current_min_target)
                        {
                            current_point = pm_pair_it->parent;
                            break;
                        }
                        pm_pair_it++;
                    }
                    bool backtrace_couldnt_find_minimum = pm_pair_it==points_parents_minima[current_point].end();
                    assert(!backtrace_couldnt_find_minimum);
                }
            }
        }
    }

    // find the distribution of how many times points were visited
    std::map<int, int> visited_counts;
    createHistogramMap(&visited_counts, points_parents_minima,
                       [](const std::vector<ParentMinimumPair> &e) {return e.size();});

    int total_visit_count = 0;
    int total_point_count = 0;
    for (auto const &visited_number_pair : visited_counts)
    {
        std::cout << visited_number_pair.second << " points visited " << visited_number_pair.first << " times" << std::endl;
        total_visit_count += visited_number_pair.first * visited_number_pair.second;
        total_point_count += visited_number_pair.second;
    }
    std::cout << "number of points " << total_point_count << std::endl;
    std::cout << "number of visits " << total_visit_count << std::endl;

    // woops not all points visited

    // find distribution of number of saddles per min
    std::map<int, int> saddles_per_minimum_counts;
    createHistogramMap(&saddles_per_minimum_counts, (*minimum_to_saddle_adjacency),
                       [](const SaddleList &e) {return e.size();});

    for (auto const &saddles_number_pair : saddles_per_minimum_counts)
    {
        std::cout << saddles_number_pair.second << " minima connected to " << saddles_number_pair.first << " saddles" << std::endl;
    }
    // woops surprisingly many saddles for some minima, and some have none

#endif // #ifdef DEBUG_WATERSHEDS

    // for all minima, find connected saddles
    // then connect saddles to minima
    // choose a depth and a minimum for ocean
    // find all saddles (above sea lvl) connected to the flooded minima
    //   these will be the main watersheds//river deltas


}
