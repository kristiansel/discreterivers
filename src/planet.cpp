#include "planet.h"

#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>

#include "gfx_primitives.h"

namespace vmath = Vectormath::Aos;

// should probably move this elsewhere
void createCubeGeometry(std::vector<vmath::Vector3> * const points,
                        std::vector<gfx::Triangle> * const triangles,
                        float half_side);

// should probably move this elsewhere
void createIcosahedronGeometry(std::vector<vmath::Vector3> * const points,
                               std::vector<gfx::Triangle> * const triangles,
                               const float radius);

// should probably move this elsewhere
void createIcoSphereGeometry(std::vector<vmath::Vector3> * const points,
                             std::vector<gfx::Triangle> * const triangles,
                             std::vector<std::vector<gfx::Triangle>> * const subd_triangles,
                             const float radius,
                             const int num_subdivisions);

void createAdjacencyList(std::vector<ConnectionList> * const edges,
                     const std::vector<vmath::Vector3> &points,
                     const std::vector<gfx::Triangle> &triangles);

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

void createRidgeGeometry(std::vector<vmath::Vector3> * const ridge_points,
                         std::vector<gfx::Line> * const ridge_lines,
                         const std::vector<vmath::Vector3> &points,
                         const std::vector<ConnectionList> &point_to_point_adjacency_list,
                         const std::vector<ConnectionList> &flow_down_adjacency);

// helper adjacency list methods
void createPointToTriAdjacency(std::vector<std::vector<tri_index>> * point_tri_adjacency,
                              const std::vector<gfx::Triangle> &triangles,
                              const std::vector<vmath::Vector3> &points);

void createTriToTriAdjacency(std::vector<std::vector<tri_index>> * tri_tri_adjacency,
                             const std::vector<gfx::Triangle> &triangles,
                             const std::vector<vmath::Vector3> &points,
                             const std::vector<std::vector<tri_index>> &point_tri_adjacency);

void createTriToTriAdjacency(std::vector<std::vector<tri_index>> * tri_tri_adjacency,
                              const std::vector<gfx::Triangle> &triangles,
                              const std::vector<vmath::Vector3> &points);

void createOceanGeometry(std::vector<vmath::Vector3> * const ocean_points,
                         std::vector<gfx::Triangle> * const ocean_triangles,
                         std::vector<LandWaterType>  * const point_land_water_types,
                         const std::vector<gfx::Triangle> &triangles,
                         const std::vector<vmath::Vector3> &points,
                         const std::vector<std::vector<tri_index>> &point_tri_adjacency,
                         const std::vector<std::vector<tri_index>> &tri_tri_adjacency,
                         float sealevel_radius);

void createLake(std::vector<vmath::Vector3> * const lake_points,
                     std::vector<gfx::Triangle> * const lake_triangles,
                     std::vector<LandWaterType>  * const point_land_water_types,
                     float &lake_level_radius,
                     const std::vector<gfx::Triangle> &triangles,
                     const std::vector<vmath::Vector3> &points,
                     const std::vector<std::vector<tri_index>> &point_tri_adjacency,
                     const std::vector<std::vector<tri_index>> &tri_tri_adjacency,
                     const std::vector<point_index> &contained_minima);

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
                          float sealevel_radius);

float get_height(const vmath::Vector3 &p)
{
    return vmath::length(p);
}

Planet::Planet(const float radius, const int subdivision_level, const int seed)
    //: mPointsDirty(true)
{    
    // create a cube's triangles
    // float half_side = M_SQRT2 * radius * 0.2;
    // createCubeGeometry(&mPoints, &mTriangles, 1.0f);

    // create icosahedron
    // createIcosahedronGeometry(&mPoints, &mTriangles, 1.0f);

    srand(seed);

    //srand(21314);


    //createIcoSphereGeometry(&mPoints, &mTriangles, radius, 8);
    createIcoSphereGeometry(&mPoints, &mTriangles, &mSubdTriangles, radius, subdivision_level);
    //mPointsDirty = true;

    // assertion failure (flow_up_or_down) at:
    //srand(76585);
    //createIcoSphereGeometry(&mPoints, &mTriangles, radius, 8);
    //createIcoSphereGeometry(&mPoints, &mTriangles, radius, 7);

    createAdjacencyList(&mPointToPointAdjacencyList, mPoints, mTriangles);

    createFlowAdjacencyLists(&mFlowDownAdjacency, &mFlowUpAdjacency, mPoints,
                             mPointToPointAdjacencyList, get_height);

    createRidgeGeometry(&mRidgePoints, &mRidgeLines, mPoints, mPointToPointAdjacencyList,
                        mFlowDownAdjacency);

    findCriticalPoints(&mPointsTopologicalType, &mMaxima, &mMinima, &mSaddles,
                       mPoints, mTriangles, mPointToPointAdjacencyList);

    createPointToTriAdjacency(&mPointToTriAdjacencyList, mTriangles, mPoints);
    createTriToTriAdjacency(&mTriToTriAdjacencyList, mTriangles, mPoints, mPointToTriAdjacencyList);

    // depth first search triangles from deepest minima to a ocean level in order to
    // find the set of triangles needed for ocean rendering

    createOceanGeometry(&mOceanPoints, &mOceanTriangles, &mPointsLandWaterType,
                        mTriangles, mPoints, mPointToTriAdjacencyList, mTriToTriAdjacencyList,
                        radius);

    createLakesAndRivers(&mLakePoints, &mLakeTriangles, &mRiverLines, &mPointsLandWaterType, mTriangles, mPoints, mPointToPointAdjacencyList,
                         mFlowDownAdjacency, mFlowUpAdjacency,
    //createLakesAndRivers(&mLakePoints, &mLakeTriangles, &mRiverLines, mTriangles, mPoints, mFlowDownAdjacency,
                         mPointToTriAdjacencyList, mTriToTriAdjacencyList, radius);


    //findWatersheds(&mMinimumToSaddleAdjecency, &mSaddleToMinimumAdjecency,
    //               mPoints, mPointToPointAdjacencyList, mPointsTopologicalType,
    //               mMinima, mSaddles);

//    // list critical points
//    std::cout << "listing maxima: " << std::endl;
//    for (const auto &i : mMaxima)
//    {
//        vmath::Vector3 p = mPoints[i];
//        std::cout << "  " << i << ", r = " << vmath::length(p()) << std::endl;
//    }

//    std::cout << "listing minima: " << std::endl;
//    for (const auto &i : mMinima)
//    {
//        vmath::Vector3 p = mPoints[i];
//        std::cout << "  " << i << ", r = " << vmath::length(p()) << std::endl;
//    }
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

//inline const vmath::Vector4 * const coerceVec3toVec4Ptr(std::vector<vmath::Vector3> &vec3s_in)
//{
//    // dirty dirty hack, depends on underlying implementation of Vector3 as a 128 bit aligend vector
//    assert(sizeof(vmath::Vector3)==sizeof(vmath::Vector4) && sizeof(vmath::Vector3)==4*sizeof(float) );
//    for (auto &v3 : vec3s_in)
//    {
//        vmath::Vector4 &v4_ref = (vmath::Vector4&)(v3);
//        v4_ref.setW(1.0f);
//    }

//    return (vmath::Vector4 *)(&vec3s_in[0]);
//}


const std::vector<vmath::Vector4> * const Planet::getPointsPtr()
{
    return coerceVec3toVec4(mPoints);
}

const std::vector<vmath::Vector4> * const Planet::getRidgePointsPtr()
{
    return coerceVec3toVec4(mRidgePoints);
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

// Other functions
// should probably move this elsewhere
void createCubeGeometry(std::vector<vmath::Vector3> * const points,
                        std::vector<gfx::Triangle> * const triangles,
                        float half_side)
{
    // check pointer validity
    // check half_side positive?

    *points = { // 8 corners
        {-half_side, -half_side, -half_side},
        { half_side,  half_side, -half_side},
        { half_side, -half_side, -half_side},
        {-half_side,  half_side, -half_side},
        { half_side, -half_side,  half_side},
        {-half_side,  half_side,  half_side},
        {-half_side, -half_side,  half_side},
        { half_side,  half_side,  half_side}
    };

    *triangles = {
        {0, 2, 1}, {0, 1, 3}, // far face
        {5, 6, 4}, {5, 4, 7}, // near face
        {3, 0, 6}, {3, 6, 5}, // left face
        {2, 7, 4}, {2, 1, 7}, // right face
        {4, 0, 2}, {4, 6, 0}, // bottom face
        {1, 5, 7}, {1, 3, 5}, // top face
    };
}

// should probably move this elsewhere
void createIcosahedronGeometry(std::vector<vmath::Vector3> * const points,
                               std::vector<gfx::Triangle> * const triangles,
                               float radius = 1.0f)
{
    // check pointer validity
    // check half_side positive?

    float tau = (1.0f + sqrt(5.0f)) / 2.0f;

    *points = { // 12 corners
        {-1.0f,  tau,  0.0},
        { 1.0f,  tau,  0.0},
        {-1.0f, -tau,  0.0},
        { 1.0f, -tau,  0.0},

        { 0.0, -1.0f,  tau},
        { 0.0,  1.0f,  tau},
        { 0.0, -1.0f, -tau},
        { 0.0,  1.0f, -tau},

        { tau,  0.0, -1.0f},
        { tau,  0.0,  1.0f},
        {-tau,  0.0, -1.0f},
        {-tau,  0.0,  1.0f}
    };

    // scale points to desired radius
    float radius_scale_factor = vmath::length((*points)[0]);
    for (auto &point : *points)
    {
        // float scaled_rand_offset = randFloatInRange(-0.15, 0.15);
        float scaled_rand_offset = 0.0f;
        //std::cout << scaled_rand_offset << std::endl;
        vmath::Vector4 point4(point, 1.0f);
        point4 = vmath::Matrix4::scale(vmath::Vector3((radius+scaled_rand_offset)/(radius_scale_factor))) * point4;
        point = point4.getXYZ();
        //std::cout << "length of icosahedron point = " << vmath::length(point()) << std::endl;
    }

    *triangles = {
        // 5 faces around point 0
        {0, 11, 5},
        {0, 5, 1},
        {0, 1, 7},
        {0, 7, 10},
        {0, 10, 11},

        // 5 adjacent faces
        {1, 5, 9},
        {5, 11, 4},
        {11, 10, 2},
        {10, 7, 6},
        {7, 1, 8},

        // 5 faces around point 3
        {3, 9, 4},
        {3, 4, 2},
        {3, 2, 6},
        {3, 6, 8},
        {3, 8, 9},

        // 5 adjacent faces
        {4, 9, 5},
        {2, 4, 11},
        {6, 2, 10},
        {8, 6, 7},
        {9, 8, 1}
    };
}



inline int getNumPointsAtSubdivisionLevel(const int subdivision_level)
{
    // Proof of the below formula is left as an excercise for the reader
    return (4+5*pow(2, 2*(subdivision_level+1)))/2;
}

inline int getNumTrisAtSubdivisionLevel(const int subdivision_level)
{
    return 20*(pow(4, subdivision_level));
}

//struct pairhash {
//public:
//    template <typename T, typename U>
//    std::size_t operator()(const std::pair<T, U> &x) const
//    {
//        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
//    }
//};

//#include <unordered_map>
//typedef std::unordered_map<std::pair<int, int>, int, pairhash> IntpairIntMapType;

// a map is good here because find is O(log n) worst case
#include <map>
typedef std::map<std::pair<int, int>, int> IntpairIntMapType;

namespace fractal
{

float none(  const vmath::Vector3 &p1,
                const vmath::Vector3 &p2)
{
    return 0.0f;
}

inline float log_normal(float x, float mu, float sigma)
{
    return 1.0f/(x*sigma*sqrt(2.0f*M_PI))*exp(-(log(x)-mu)*(log(x)-mu)/(2.0f*(sigma*sigma)));
}

inline float evaluate_radius(   const vmath::Vector3 &p1,
                                const vmath::Vector3 &p2)
{
    float length_p1 = vmath::length(p1);
    float length_p2 = vmath::length(p2);

    return ( length_p1 + length_p2 ) * 0.5f;
}

//inline float spline_radius(   const vmath::Vector3 &p1,
//                              const gfx::Triangle &tri,
//                              const vector<vmath::Vector3> &positions,
//                              const vector<vmath::Vector3> &normals,)
//{
//    float length_p1 = vmath::length(p1);
//    float length_p2 = vmath::length(p2);

//    return ( length_p1 + length_p2 ) * 0.5f;
//}


float midpoint_displacement(    const vmath::Vector3 &p1,
                                const vmath::Vector3 &p2 )
{
    // infer radius from parent points:
    float length_p1 = vmath::length(p1);
    float length_p2 = vmath::length(p2);

    float mean_radius = evaluate_radius(p1,p2);

    vmath::Vector3 norm_p1 = p1/length_p1;
    vmath::Vector3 norm_p2 = p2/length_p2;

    float unit_sphere_distance_p1p2 = vmath::length(norm_p2 - norm_p1);
    float mean_radius_distance_p1p2 = mean_radius * unit_sphere_distance_p1p2;
    float horz_length_scale = mean_radius_distance_p1p2;

    float length_scale_as_frac_radius = unit_sphere_distance_p1p2;
    float x = length_scale_as_frac_radius;

    float frac_scale = 0.025+0.045*log_normal(5.0f*x, 0.0f, 0.5f);
    //float frac_scale = x > 0.3 ? 0.075f : 0.0;
    //float frac_scale = 0.035f;
    //float frac_scale = 0.00f;
    float frac_magnitude = horz_length_scale * frac_scale;

    // create offset with magnitude based on the distance between parent points
    return randFloatInRange(-frac_magnitude, frac_magnitude);
}

} // namespace fractal

inline vmath::Vector3 linePlaneIntersection(vmath::Vector3 line_dir, vmath::Vector3 line_point,
                                     vmath::Vector3 plane_normal, vmath::Vector3 plane_point)
{
    // from wikipedia
    float d = vmath::dot(plane_point-line_point, plane_normal)/vmath::dot(plane_normal, line_dir);
    return line_point + d*line_dir;
}

inline vmath::Vector3 linePlaneIntersectionNormalized(vmath::Vector3 line_dir_normalized, vmath::Vector3 line_point,
                                               vmath::Vector3 plane_normal_normalized, vmath::Vector3 plane_point)
{
    assert(abs(vmath::length(line_dir_normalized)-1.f)<0.0001);
    assert(abs(vmath::length(plane_normal_normalized)-1.f)<0.0001);

    float d = vmath::dot(plane_point-line_point, plane_normal_normalized);
    return line_point + d*line_dir_normalized;
}

inline vmath::Vector3 findPointInTriangle(point_index i_p1, point_index i_p2, point_index i_p3,
                                   const std::vector<vmath::Vector3> &points,
                                   const std::vector<vmath::Vector3> &normals)
{
    // find barycentric coordinates

    // evaluate bernstein basis at barycentric coords (6 polys)

    // find the... fuark need edge normals for 3 edge planes
}


template<class fractal>
inline vmath::Vector3 getMidpoint(const vmath::Vector3 &p1,
                                  const vmath::Vector3 &p2,
                                  const vmath::Vector3 &n1,
                                  const vmath::Vector3 &n2,
                                  fractal frac)
{
    // should assert that normalized quantity is not zero

    // infer radius from parent points:
    float length_p1 = vmath::length(p1);
    float length_p2 = vmath::length(p2);

    // do something fancy with normal here
//    vmath::Vector3 average_normal = vmath::normalize((n1+n2)*0.5f);

    // new method
    vmath::Vector3 p_mid = 0.5f*(p1 + p2);
    vmath::Vector3 p_n1  = linePlaneIntersection(p_mid, vmath::Vector3(0.0f), n1, p1);
    vmath::Vector3 p_n2  = linePlaneIntersection(p_mid, vmath::Vector3(0.0f), n2, p2);

    float b02 = 0.25; // hardcoded bernstein polynomial coefficient for midpoint
    float b12 = 0.50; // hardcoded bernstein polynomial coefficient for midpoint
    float b22 = 0.25; // hardcoded bernstein polynomial coefficient for midpoint
    vmath::Vector3 p_spline_interp = b02 * p_n1 + b12 * p_mid + b22 * p_n2;

    float old_height = vmath::length(p_spline_interp);
    float new_height = old_height + frac(p1, p2);

    vmath::Vector3 midpoint = new_height * vmath::normalize(p_spline_interp);


// old method

//    float mean_radius = ( length_p1 + length_p2 ) * 0.5f;

//    float fractal_offset = frac(p1, p2);

//    vmath::Vector3 midpoint_sphere_projected = vmath::normalize((p1 + p2) * 0.5f);
//    float new_radius = (mean_radius + fractal_offset);

//    vmath::Vector3 midpoint = vmath::Vector3( new_radius * midpoint_sphere_projected );

////     std::cout << "length of subdivision point = " << vmath::length(midpoint()) << std::endl;

    return midpoint;
}

int getSubdPointIndex(const point_index i1,
                      const point_index i2,
                      const float radius,
                      std::vector<vmath::Vector3> * const points,
                      std::vector<vmath::Vector3> &normals,
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
        // create the point
        points->push_back(
            getMidpoint((*points)[i_lo], (*points)[i_hi],
                          normals[i_lo], normals[i_hi],
                        fractal::midpoint_displacement)
        );
        int last_element_index = points->size() - 1;

        // add it to the cache
        midpoints_cache->insert( {midpoint_key, last_element_index} );
        return last_element_index;
    }
}


void createIcoSphereGeometry(std::vector<vmath::Vector3> * const points,
                             std::vector<gfx::Triangle> * const triangles,
                             std::vector<std::vector<gfx::Triangle>> * const subd_triangles,
                             const float radius,
                             const int num_subdivisions)
{
    // check input
    // check pointer validity
    // check radius positive
    // check num_subdivisions positive

    int num_points_max = getNumPointsAtSubdivisionLevel(num_subdivisions);
    points->reserve(num_points_max); // woops, reeks premature optimization

    // need one set of triangles for each subdivision level
    auto &triangles_subd_lvls = *(subd_triangles);
    // std::vector<std::vector<gfx::Triangle>> triangles_subd_lvls;
    triangles_subd_lvls.reserve(num_subdivisions+1);

    // Create subdivision level zero by creating Icosahedron
    triangles_subd_lvls.push_back(std::vector<gfx::Triangle>());
    triangles_subd_lvls[0].reserve(getNumTrisAtSubdivisionLevel(0));

    createIcosahedronGeometry(points, &triangles_subd_lvls[0], radius);

    std::vector<vmath::Vector3> normals;
    gfx::generateNormals(&normals, *points, triangles_subd_lvls[0]);

    // Subsequently subdivide triangles
    for (int k = 1; k < num_subdivisions + 1; k++)
    {
        std::cout << "subdividing level " << k << ": ";
        triangles_subd_lvls.push_back(std::vector<gfx::Triangle>());
        triangles_subd_lvls[k].reserve(getNumTrisAtSubdivisionLevel(k));

        // Use a cache to keep points resulting from subidiving a shared edge
        IntpairIntMapType midpoints_cache;
        //int num_cache_points_max = getNumPointsAtSubdivisionLevel(k) - getNumPointsAtSubdivisionLevel(k-1);
        //midpoints_cache.reserve( num_cache_points_max );

        // for progress tracking
        std::cout << "[" << std::string(10, ' ') << "]";

        // get index of current last triangle
        int prev_subdlvl_size = triangles_subd_lvls[k-1].size();
        for (int i = 0; i < prev_subdlvl_size; i++)
        {
            // progress bar
            int progress_often= 10.0f*((float)(i)/(float)(prev_subdlvl_size))+1;
            if ((i)%(prev_subdlvl_size/10)==0) {
                std::cout << std::string(11, '\b')
                          << std::string(progress_often, '=')
                          << std::string(10-progress_often, ' ')
                          << "]" << std::flush;
            }

            // get indices of the three new points (organized by sides)
            gfx::Triangle const &prev_triangle = triangles_subd_lvls[k-1][i];
            point_index new_points_indices[] = {
                getSubdPointIndex(prev_triangle[0], prev_triangle[1], radius, points, normals, &midpoints_cache),
                getSubdPointIndex(prev_triangle[1], prev_triangle[2], radius, points, normals, &midpoints_cache),
                getSubdPointIndex(prev_triangle[2], prev_triangle[0], radius, points, normals, &midpoints_cache)
            };

            // create the subdivided triangles, using previously existing and new corners
            triangles_subd_lvls[k].push_back(
                {
                    prev_triangle[0],
                    new_points_indices[0],
                    new_points_indices[2]
                }
            );

            triangles_subd_lvls[k].push_back(
                {
                    prev_triangle[1],
                    new_points_indices[1],
                    new_points_indices[0]
                }
            );

            triangles_subd_lvls[k].push_back(
                {
                    prev_triangle[2],
                    new_points_indices[2],
                    new_points_indices[1]
                }
            );

            triangles_subd_lvls[k].push_back(
                {
                    new_points_indices[0],
                    new_points_indices[1],
                    new_points_indices[2]
                }
            );
        } // for (int i = 0; i < prev_subdlvl_size; i++)

        gfx::generateNormals(&normals, *points, triangles_subd_lvls[k]);

        std::cout << std::endl;

//        // compare cache size with cache initial reserved capacity
//        std::cout << "size of cached points ends up as" << midpoints_cache.size()
//                  << " cached points" << std::endl;
//        std::cout << "reserved capacity for " << num_cache_points_max
//                  << " cached points" << std::endl;
    }

    // copy the highest subdivision level to the resulting triangles
    *triangles = triangles_subd_lvls.back();

    std::cout << "created icosphere with " << points->size()
              << " vertices and " << triangles->size() << " triangles"
              << std::endl;
//    std::cout << "num_subdivisions = " << num_subdivisions << std::endl;
//    std::cout << "reserved capacity for " << getNumPointsAtSubdivisionLevel(num_subdivisions)
//              << " vertices and " << getNumTrisAtSubdivisionLevel(num_subdivisions) << " triangles"
//              << std::endl;
}

template<class T>
void push_back_if_unique(std::vector<T> &v, const T &e)
{
    if (std::find(v.begin(), v.end(), e) == v.end()) v.push_back(e);
}

typedef std::pair<point_index, point_index> edge_t;
void createAdjacencyList(std::vector<ConnectionList> * const edges,
                     const std::vector<vmath::Vector3> &points,
                     const std::vector<gfx::Triangle> &triangles)
{
    // TODO: check integrity of inputs



    // add edges for all triangles
    edges->resize(points.size());

    for (const auto &triangle : triangles)
    {
        for (int i = 0; i<3; i++)
        {
            int j = (i+1)%3; // 1,2,3,0
            push_back_if_unique((*edges)[triangle[i]], triangle[j]);
            push_back_if_unique((*edges)[triangle[j]], triangle[i]);
        }
    }

    // here is the heavy lifting, ordering the connections (counter-clockwise looking from outside)
    auto angle_between_edges = [&](const point_index i, const point_index j, const point_index i_base)
    {
        vmath::Vector3 vec_edge_i = points[i]-points[i_base];
        vmath::Vector3 vec_edge_j = points[j]-points[i_base];

        return acos(vmath::dot(vec_edge_i, vec_edge_j)/(vmath::length(vec_edge_i)*vmath::length(vec_edge_j)));
    };

    auto cross_prod_outwards = [&](const point_index i, const point_index j, const point_index i_base)
    {
        vmath::Vector3 vec_edge_i = points[i]-points[i_base];
        vmath::Vector3 vec_edge_j = points[j]-points[i_base];

        vmath::Vector3 cross_prod = vmath::cross(vec_edge_i, vec_edge_j);
        return vmath::dot(vmath::normalize(points[i_base]), cross_prod);
    };

    for (int i_point = 0; i_point<edges->size(); i_point++)
    {
        auto &point_edges = (*edges)[i_point];

        //std::cout << "point: " << i_point << std::endl;

        for (int i_edge = 0; i_edge<point_edges.size()-1; i_edge++)
        {
            //std::cout << "  i_edge = " << i_edge << std::endl;

            float smallest_angle = std::numeric_limits<float>::max();
            int i_edge_smallest_angle = -1; // be set;
            for (int i_edge_lookahead = i_edge+1; i_edge_lookahead < point_edges.size(); i_edge_lookahead++)
            {
                float edge_angle = angle_between_edges(point_edges[i_edge], point_edges[i_edge_lookahead], i_point);
                float cross_prod_out = cross_prod_outwards(point_edges[i_edge], point_edges[i_edge_lookahead], i_point);
                //std::cout << "    theta: " << edge_angle*180.0f/M_PI << "/" << max_allowed_angle*180.0f/M_PI << " deg"
                //          << ", cpo: " << cross_prod_out << std::endl;
                if (  edge_angle < smallest_angle && cross_prod_out > 0.0f )
                {
                    smallest_angle = edge_angle;
                    i_edge_smallest_angle = i_edge_lookahead;
                    //std::cout << "     best so far!" << std::endl;
                }
            }
            assert(i_edge_smallest_angle!=-1);
            std::swap(point_edges[i_edge_smallest_angle], point_edges[i_edge+1]);
        }
    }

#define DEBUG_ADJACECYLIST

#ifdef DEBUG_ADJACECYLIST

    // confirm that the angle between adjacent edges is less than 2*(360/num_edges)
    for (int i_point = 0; i_point<edges->size(); i_point++)
    {
        auto &point_edges = (*edges)[i_point];
        float max_allowed_angle = 2.0f*(2.0f*M_PI/(float)(point_edges.size()));

//        std::cout << "point: " << i_point << std::endl;

        for (int i_edge = 0; i_edge<point_edges.size(); i_edge++)
        {
            int j_edge = (i_edge+1)%(point_edges.size()); // 1,2,3,4,5,0

            float angle = angle_between_edges(point_edges[i_edge], point_edges[j_edge], i_point);

//            std::cout << "angle = " << angle*180.0f/M_PI << " degrees"
//                      //<< ", cdn = " << cross_dot_norm(point_edges[i_edge], i_point, ref_vector)
//                      << std::endl;
            assert(angle<max_allowed_angle);
        }
    }

    // confirm no points have more than 6 edges and no less than 5
    for (int i = 0; i<edges->size(); i++)
    {
        const auto &edge_list = (*edges)[i];

        assert(edge_list.size()==6 || edge_list.size()==5); // only applicable for the current way of connecting

    }
#endif // #ifdef DEBUG_ADJACECYLIST
}

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

#include "typetag.h"
struct ridge_point_tag{};
typedef ID<ridge_point_tag, int, -1> ridge_point_id;

struct ridge_line_tag{};
typedef ID<ridge_line_tag, int, -1> ridge_line_id;

void createRidgeGeometry(std::vector<vmath::Vector3> * const ridge_points,
                         std::vector<gfx::Line> * const ridge_lines,
                         const std::vector<vmath::Vector3> &points,
                         const std::vector<ConnectionList> &point_to_point_adjacency_list,
                         const std::vector<ConnectionList> &flow_down_adjacency)
{


    // cache of already added points (could exchange for map to conserve memory, at the cost of log(n) lookup)
    std::vector<ridge_point_id> already_added_point(points.size());

    // point helper functions
    auto is_ridge_point = [&](point_index i_p) { return flow_down_adjacency[i_p].size() > 1; };

    auto point_not_added = [&](point_index i_p) { return already_added_point[i_p]==ridge_point_id::invalid(); };

    auto add_point = [&](point_index i_p)
    {
        ridge_point_id j_p = ridge_point_id(ridge_points->size());
        ridge_points->push_back(points[i_p]);
        already_added_point[i_p] = j_p;
        return j_p;
    };

    // line cache
    std::map<std::pair<point_index, point_index>, gfx::Line> already_added_line;

    // line helper functions
    auto line_not_added = [&](std::pair<point_index, point_index> ii)
    {
        point_index i_lo = ii.first < ii.second ? ii.first : ii.second;
        point_index i_hi = ii.first > ii.second ? ii.first : ii.second;
        auto set_it = already_added_line.find({i_lo, i_hi});
        return set_it == already_added_line.end();
    };

    auto add_line = [&](std::pair<point_index, point_index> ii, gfx::Line l_jj)
    {
        ridge_line_id k_l = ridge_line_id(ridge_lines->size());
        ridge_lines->push_back(l_jj);
        already_added_line[ii] = l_jj;
        return k_l;
    };

    // start at a ridge points, pick a direction
    for (int i = 0; i<points.size(); i++)
    {
        if (is_ridge_point(i) && point_not_added(i)) // it is a ridge point and should be added
        {
            ridge_point_id j = add_point(i);

            // start tracing
            auto adj_list = point_to_point_adjacency_list[i];
            for (const auto &i_adj : adj_list)
            {
                point_index i_lo = i_adj < i ? i_adj : i;
                point_index i_hi = i_adj > i ? i_adj : i;
                if (is_ridge_point(i_adj) && line_not_added({i_lo, i_hi}))
                {
                    ridge_point_id j_adj = point_not_added(i_adj) ? add_point(i_adj) : already_added_point[i_adj];
                    ridge_line_id k = add_line({i, i_adj}, {int(j), int(j_adj)});
                }
            }
        }
    }

    // terminate if a point has been marked as ridge before

    // verify that all ridge points are connected
#define DEBUG_RIDGELINES

#ifdef DEBUG_RIDGELINES
    for (int i = 0; i<points.size(); i++)
    {
        // assert only ridge points are added
        if (is_ridge_point(i)) assert(!point_not_added(i)); else assert(point_not_added(i));
    }

    std::cout << "number of ridge points: " << ridge_points->size() << std::endl;
    std::cout << "number of ridge lines: " << ridge_lines->size() << std::endl;

    // check index bounds
    for (const auto &line : (*ridge_lines))
    {
        bool ridge_line_index_overflow_up = line[0] > ridge_points->size()-1 ||  line[1] > ridge_points->size()-1;
        bool ridge_line_index_overflow_down = line[0] < 0 ||  line[1] < 0;
        assert(!(ridge_line_index_overflow_up || ridge_line_index_overflow_down));
    }

    // find out how many dead ends there are...?

#endif // #ifdef DEBUG_RIDGELINES
}


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



void createPointToTriAdjacency(std::vector<std::vector<tri_index>> * point_tri_adjacency,
                              const std::vector<gfx::Triangle> &triangles,
                              const std::vector<vmath::Vector3> &points)
{
    (*point_tri_adjacency) =  std::vector<std::vector<tri_index>>(points.size());
    for (int i = 0; i<triangles.size(); i++)
    {
        (*point_tri_adjacency)[triangles[i][0]].push_back(tri_index(i));
        (*point_tri_adjacency)[triangles[i][1]].push_back(tri_index(i));
        (*point_tri_adjacency)[triangles[i][2]].push_back(tri_index(i));
    }
}

void createTriToTriAdjacency(std::vector<std::vector<tri_index>> * tri_tri_adjacency,
                             const std::vector<gfx::Triangle> &triangles,
                             const std::vector<vmath::Vector3> &points,
                             const std::vector<std::vector<tri_index>> &point_tri_adjacency)
{
    (*tri_tri_adjacency) =  std::vector<std::vector<tri_index>>(triangles.size());
    for (int i = 0; i<triangles.size(); i++)
    {
        // create big list of all tri indices adjacent
        std::vector<tri_index> thirteen_adj;
        thirteen_adj.reserve(18); // some will be added twice
        for (int j = 0; j<3; j++)
        {
            point_index i_p = triangles[i][j];
            for (const tri_index i_t : point_tri_adjacency[i_p]) thirteen_adj.push_back(i_t);
        }

        std::sort(thirteen_adj.begin(), thirteen_adj.end());
        auto last = std::unique(thirteen_adj.begin(), thirteen_adj.end());
        thirteen_adj.erase(last, thirteen_adj.end());

        // assert(thirteen_adj.size()==13); // can't assert this because of pentagonal points

        // filter list on two common points
        for (const tri_index i_t_adj: thirteen_adj)
        {
            if (int(i_t_adj)!=i)
            {
                // check if two indices are common
                int common_counter = 0;
                for (int j_adj = 0; j_adj<3; j_adj++)
                {
                    for (int j = 0; j<3; j++)
                    {
                        if(triangles[i][j]==triangles[int(i_t_adj)][j_adj]) common_counter++;
                    }
                }

                // if they are, add the adjacency
                if (common_counter==2) (*tri_tri_adjacency)[i].push_back(i_t_adj);
            }
        }

        assert((*tri_tri_adjacency)[i].size()==3);
    }
}

void createTriToTriAdjacency(std::vector<std::vector<tri_index>> * tri_tri_adjacency,
                              const std::vector<gfx::Triangle> &triangles,
                              const std::vector<vmath::Vector3> &points)
{
    std::vector<std::vector<tri_index>> point_tri_adjacency;
    createPointToTriAdjacency(&point_tri_adjacency, triangles, points);

    createTriToTriAdjacency(tri_tri_adjacency, triangles, points, point_tri_adjacency);
}

#include "graph_tools.h"
using namespace graphtools;

void createOceanGeometry(std::vector<vmath::Vector3> * const ocean_points,
                         std::vector<gfx::Triangle> * const ocean_triangles,
                         std::vector<LandWaterType>  * const point_land_water_types,
                         const std::vector<gfx::Triangle> &triangles,
                         const std::vector<vmath::Vector3> &points,
                         const std::vector<std::vector<tri_index>> &point_tri_adjacency,
                         const std::vector<std::vector<tri_index>> &tri_tri_adjacency,
                         float sealevel_radius)
{
    // initialize the point_sea_water_types
    *point_land_water_types = std::vector<LandWaterType>(points.size(), LandWaterType::Land);

    // start at a triangle adjacent to the global minimum
    int i_globalmin = -1;
    float smallest_length = std::numeric_limits<float>::max();
    for (int i = 0 ; i<points.size(); i++)
    {
        float length = vmath::length(points[i]);
        if (length<smallest_length) {i_globalmin=i; smallest_length=length;}
    }
    tri_index tri_globalmin = point_tri_adjacency[i_globalmin][0];

    // create a graph to search
    auto tri_graph = make_graph(triangles, tri_tri_adjacency);

    // create a heuristic function for prioritizing nodes to search
    auto heuristic = [&](const tri_index &i)
    {
        const gfx::Triangle &tri = triangles[int(i)];
        float h0 = vmath::length(points[tri[0]]);
        float h1 = vmath::length(points[tri[1]]);
        float h2 = vmath::length(points[tri[2]]);
        return std::min(h0, std::min(h1, h2));
    };

//    int visit_counter = 0;
//    for ( auto it = tri_graph.search(tri_globalmin, heuristic);
//          /*it.heuristic_eval() < sealevel_radius && */!it.search_end();
//          ++it )
//    {
//        visit_counter++;
//        std::cout << "counter " << visit_counter << "/" << triangles.size()
//                  << " or "<< float(visit_counter)/float(triangles.size())*100.0f << " % complete" << std::endl;
//    } // test complete search


    for ( auto it = tri_graph.search(tri_globalmin, heuristic);
          it.heuristic_eval() < sealevel_radius && !it.search_end();
          ++it )
    {
        ocean_triangles->push_back(*it);
    } // test complete search

    // remap the triangles/points to create a sparse point/triangle representation for just the ocean
    std::vector<point_index> point_ocean_map(points.size(), -1);

    for (int i = 0 ; i<ocean_triangles->size(); i++)
    {
        for (int j = 0; j<3; j++)
        {
            point_index i_p_unmapped = (*ocean_triangles)[i][j];

            // Not all points in a sea triangle are below sealevel, therefore extra check
            if (vmath::length(points[i_p_unmapped]) < sealevel_radius)
            {
                (*point_land_water_types)[i_p_unmapped] = LandWaterType::Sea;
            }

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
        (*ocean_points)[i] = sealevel_radius * vmath::normalize((*ocean_points)[i]);
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
                          float sealevel_radius)
{
    for (int i_springs = 0; i_springs<100; i_springs++)
    {
        point_index start_point;

        //srand(213213123);
        do
        {
            start_point = rand() % points.size();
        } while ((*point_land_water_types)[int(start_point)] != LandWaterType::Land);
        // The start_point should be guaranteed to be on land

        std::cout << "start_point = " << start_point << std::endl;

        //start_point = 3119; // nice and visible point for debug

        //start_point = 2500; // nice and visible point for debug

        auto flow_graph = make_graph(points, point_to_point_adjacency);

        auto heuristic = [&](const point_index &i)
        {
            //std::cout << "i = " << i << std::endl;
            return vmath::length(points[i]);
        };

        auto it = flow_graph.search(start_point, heuristic);

        // assume that optional parents are initialized none, and children vectors empty
        std::vector<optional<point_index>> search_parents(points.size());
        std::vector<std::vector<point_index>> search_children(points.size());

        std::vector<point_index> drainage_system_points;
        drainage_system_points.push_back(it.get_index());
        //std::cout << "start_point: " << start_point << ", get_index: " << it.get_index() << std::endl;

        do
        {
            ++it;

            drainage_system_points.push_back(it.get_index());

            optional<point_index> op_parent_index = it.get_parent_index(); // definitly was a bug in the parent_index
            if (op_parent_index.exists())
            {
                point_index parent_index = op_parent_index.get();
                point_index this_index = it.get_index();

                search_parents[this_index] = make_optional(parent_index);
                search_children[parent_index].push_back(this_index);

                if (parent_index != this_index)
                {
                    // if the search was on its way down, then it is a river
                    if (vmath::length(points[parent_index]) > vmath::length(points[this_index]))
                    {
                        //river_lines->push_back(gfx::Line{parent_index, this_index});
                    }
                    else // if the search was on its way up, then it is a lake
                    {
    //                    // iterate over all triangles adjacent to point
    //                    for (const auto &adj_tri : point_tri_adjacency[this_index])
    //                    {
    //                        push_back_if_unique(lake_triangle_indices, adj_tri);
    //                    }
                    } // if length of parent point greater than this point
                } // if parent index is not the same as this index
            } // if parent  exists
        } while ((*point_land_water_types)[it.get_index()] == LandWaterType::Land && !it.search_end());

        // sort out the lake triangles

        point_index end_index = it.get_index();

        std::vector<gfx::Triangle> this_lake_triangles;

        // IMPORTANT: in the reverse search backtrack by search ORDER

        // reverse the search and set the height water height
        std::vector<optional<float>> water_height(points.size());
        {
            point_index rev_search_index = end_index;
            float highest_water_level = vmath::length(points[rev_search_index]);
            do
            {
                // set the highest water level
                water_height[rev_search_index] = make_optional(highest_water_level);

                // update search
                rev_search_index = search_parents[rev_search_index].get();
                if (vmath::length(points[rev_search_index]) > highest_water_level)
                {
                    highest_water_level = vmath::length(points[rev_search_index]);
                }
            }
            while (search_parents[rev_search_index].exists());
        }


        // IMPORTANT: in the the tracking of rivers search by search search HIERARCHY
        // TODO: Switch from ORDER to hierarchy in river points collection

        // do the forward search again, and set water height on points not necessarily
        // on shortest path between sea and starting point.
        point_index for_search_index = start_point;
        {
            float lowest_water_level = vmath::length(points[for_search_index]);

            std::function<void (point_index)> do_for_children = [&] (point_index i_p)
            {
                if (!water_height[i_p].exists())
                {
                    water_height[i_p] = make_optional(lowest_water_level);
                }
                else
                {
                    if (water_height[i_p].get() < lowest_water_level)
                    {
                        lowest_water_level = water_height[i_p].get();
                    }
                }

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
                        point_index par = search_parents[i_p].get();
                        // ...and is a river point...
                        if (!(water_height[par].get() > vmath::length(points[par])))
                        {
                            // ...add a river line as well!
                            river_lines->push_back(gfx::Line{par, i_p});
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

        std::cout << "added " << this_lake_triangles.size() << " lake triangles" << std::endl;
        std::cout << "with " << lake_points->size() << " lake points" << std::endl;

        std::cout << "drainage system point: " << std::endl;
        for (const auto &i_p : drainage_system_points)
        {
            std::cout << "i_p: " << i_p << " p: ";
            if (search_parents[i_p].exists()) std::cout << search_parents[i_p].get();
            std::cout << " ch: ";
            for (const auto &c: search_children[i_p]) std::cout << c << ",";
            std::cout << "\b " << "tl: " << vmath::length(points[i_p]) << " wl: ";
            if (water_height[i_p].exists())
            {
                std::cout << water_height[i_p].get() << ", ";
                if (water_height[i_p].get() > vmath::length(points[i_p]))
                {
                    std::cout << "lake ";
                }
                else
                {
                    std::cout << "river ";
                }
            }
            std::cout << std::endl;
        }
        lake_triangles->insert(lake_triangles->end(), this_lake_triangles.begin(), this_lake_triangles.end());
    }
}

//createLakesAndRivers backup
//{
//// pick a point on land
//// ... // random point
////point_index start_point = rand() % points.size();

//point_index start_point;
//do
//{
//    start_point = rand() % points.size();
//} while (point_land_water_types[int(start_point)] != LandWaterType::Land);

////std::cout << "start_point = " << start_point << std::endl;

//start_point = 3119; // nice and visible point for debug

//// follow downflow to min
//// ... // some iterator/heuristic DFS to min

//// create a graph to search
//auto flow_graph = make_graph(points, point_to_point_adjacency); // TODO: change to point_point_adjacency

//// create a heuristic function for prioritizing nodes to search
//auto heuristic = [&](const point_index &i)
//{
//    //std::cout << "i = " << i << std::endl;
//    return vmath::length(points[i]);
//};

//point_index prev_river_point = start_point;

//auto it = flow_graph.search(start_point, heuristic);
////while (it.heuristic_eval() <= heuristic(prev_river_point) && !it.search_end())
////while (it.heuristic_eval() > sealevel_radius && !it.search_end())
//while (it.heuristic_eval() <= heuristic(prev_river_point) && !it.search_end() &&
//       point_land_water_types[prev_river_point] == LandWaterType::Land)
//{
//    ++it;

//    // river_lines->push_back(gfx::Line{it.get_parent_index(), it.get_index()}); // end TODO: implement get_parent_index
//    river_lines->push_back(gfx::Line{prev_river_point, it.get_index()});
//    prev_river_point = it.get_index();


//}

//std::cout << "printing " << "river_lines" << ", length: " << river_lines->size() << std::endl;
//for (const auto &el : *river_lines) { std::cout << el[0] << ", " << el[1] << " : " <<
//                                                   heuristic(el[0]) << ", " << heuristic(el[1]) << std::endl; }



//// flood etc
//}


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
