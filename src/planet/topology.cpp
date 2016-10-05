#include "topology.h"

#include "vectorgeometry.h"

#include <map>

namespace vgeom = VectorGeometry;

namespace Topology

{

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
                                const vmath::Vector3 &p2,
                                const float terrain_roughness)
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

    float frac_scale = terrain_roughness*(0.025+0.045*log_normal(5.0f*x, 0.0f, 0.5f)); // officially best
    //float frac_scale = x > 0.3 ? 0.075f : 0.0;
    //float frac_scale = 0.035f;
    //float frac_scale = 0.00f;
    float frac_magnitude = horz_length_scale * frac_scale;

    // create offset with magnitude based on the distance between parent points
    return randFloatInRange(-frac_magnitude, frac_magnitude);
}

} // namespace fractal



template<class fractal>
inline vmath::Vector3 getMidpoint(const vmath::Vector3 &p1,
                                  const vmath::Vector3 &p2,
                                  const vmath::Vector3 &n1,
                                  const vmath::Vector3 &n2,
                                  fractal frac,
                                  const float terrain_roughness)
{
    // should assert that normalized quantity is not zero

    // infer radius from parent points:
    float length_p1 = vmath::length(p1);
    float length_p2 = vmath::length(p2);

    // do something fancy with normal here
//    vmath::Vector3 average_normal = vmath::normalize((n1+n2)*0.5f);

    // new method
    vmath::Vector3 p_mid = 0.5f*(p1 + p2);
    vmath::Vector3 p_n1  = vgeom::linePlaneIntersection(p_mid, vmath::Vector3(0.0f), n1, p1);
    vmath::Vector3 p_n2  = vgeom::linePlaneIntersection(p_mid, vmath::Vector3(0.0f), n2, p2);

    float b02 = 0.25; // hardcoded bernstein polynomial coefficient for midpoint
    float b12 = 0.50; // hardcoded bernstein polynomial coefficient for midpoint
    float b22 = 0.25; // hardcoded bernstein polynomial coefficient for midpoint
    vmath::Vector3 p_spline_interp = b02 * p_n1 + b12 * p_mid + b22 * p_n2;

    float old_height = vmath::length(p_spline_interp);
    float new_height = old_height + frac(p1, p2, terrain_roughness);

    vmath::Vector3 midpoint = new_height * vmath::normalize(p_spline_interp);


// old method

//    float mean_radius = ( length_p1 + length_p2 ) * 0.5f;

//    float fractal_offset = frac(p1, p2);

//    vmath::Vector3 midpoint_sphere_projected = vmath::normalize((p1 + p2) * 0.5f);
//    float new_radius = (mean_radius + fractal_offset);

//    vmath::Vector3 midpoint = vmath::Vector3( new_radius * midpoint_sphere_projected );

//     std::cout << "length of subdivision point = " << vmath::length(midpoint()) << std::endl;

    return midpoint;
}

int getSubdPointIndex(const point_index i1,
                      const point_index i2,
                      const float radius,
                      const float terrain_roughness,
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
                        fractal::midpoint_displacement,
                        terrain_roughness)
        );
        int last_element_index = points->size() - 1;

        // add it to the cache
        midpoints_cache->insert( {midpoint_key, last_element_index} );
        return last_element_index;
    }
}


void createIcoSphereGeometry(std::vector<vmath::Vector3> * const points,
                             std::vector<vmath::Vector3> * const normals,
                             std::vector<gfx::Triangle> * const triangles,
                             std::vector<std::vector<gfx::Triangle>> * const subd_triangles,
                             const float radius,
                             const int num_subdivisions,
                             const float terrain_roughness)
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

    //std::vector<vmath::Vector3> normals;
    gfx::generateNormals(normals, *points, triangles_subd_lvls[0]);

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
                getSubdPointIndex(prev_triangle[0], prev_triangle[1], radius, terrain_roughness, points, *normals, &midpoints_cache),
                getSubdPointIndex(prev_triangle[1], prev_triangle[2], radius, terrain_roughness, points, *normals, &midpoints_cache),
                getSubdPointIndex(prev_triangle[2], prev_triangle[0], radius, terrain_roughness, points, *normals, &midpoints_cache)
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

        gfx::generateNormals(normals, *points, triangles_subd_lvls[k]);

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



//-----------------------------TESTS---------------------------------------------

namespace Test {
void barycentricCoords()
{
    // do some unit testing of barycentric coordinates
    vmath::Vector3 tp0(0.0f, 0.0f, 0.0f);
    vmath::Vector3 tp1(1.0f, 0.0f, 0.0f);
    vmath::Vector3 tp2(0.0f, 1.0f, 0.0f);

    vmath::Vector3 b100 = vgeom::baryPointInTriangle(tp0, tp0, tp1, tp2);
    vmath::Vector3 b010 = vgeom::baryPointInTriangle(tp1, tp0, tp1, tp2);
    vmath::Vector3 b001 = vgeom::baryPointInTriangle(tp2, tp0, tp1, tp2);

    // assertion failed
    //vmath::Vector3 sometest = MultiCalculus::baryPointInTriangle({1.f,1.f,1.f}, tp0, tp1, tp2);


    std::cout << "b100 = (" << b100[0] << ", " << b100[1] << ", " << b100[2] << ")" << std::endl;
    std::cout << "b010 = (" << b010[0] << ", " << b010[1] << ", " << b010[2] << ")" << std::endl;
    std::cout << "b001 = (" << b001[0] << ", " << b001[1] << ", " << b001[2] << ")" << std::endl;

    vmath::Vector3 b110 = vgeom::baryPointInTriangle(0.5f*(tp0+tp1), tp0, tp1, tp2);
    vmath::Vector3 b011 = vgeom::baryPointInTriangle(0.5f*(tp1+tp2), tp0, tp1, tp2);
    vmath::Vector3 b101 = vgeom::baryPointInTriangle(0.5f*(tp2+tp0), tp0, tp1, tp2);

    std::cout << "b110 = (" << b110[0] << ", " << b110[1] << ", " << b110[2] << ")" << std::endl;
    std::cout << "b011 = (" << b011[0] << ", " << b011[1] << ", " << b011[2] << ")" << std::endl;
    std::cout << "b101 = (" << b101[0] << ", " << b101[1] << ", " << b101[2] << ")" << std::endl;

    vmath::Vector3 b111 = vgeom::baryPointInTriangle(0.333333333f*(tp0+tp1+tp2), tp0, tp1, tp2);

    std::cout << "b111 = (" << b111[0] << ", " << b111[1] << ", " << b111[2] << ")" << std::endl;

    vmath::Vector3 b = vgeom::baryPointInTriangle({0.4832,0.3421, 0.0}, tp0, tp1, tp2);

    std::cout << "b = (" << b[0] << ", " << b[1] << ", " << b[2] << ")" << std::endl;

    //    should give:
    //    b100 = (1, 0, 0)
    //    b010 = (0, 1, 0)
    //    b001 = (0, 0, 1)
    //    b110 = (0.5, 0.5, 0)
    //    b011 = (0, 0.5, 0.5)
    //    b101 = (0.5, 0, 0.5)
    //    b111 = (0.333333, 0.333333, 0.333333)
    //    b = (0.1747, 0.4832, 0.3421)

    vmath::Vector3 br = vgeom::baryPointInTriangle({-0.1f, -0.1f, 0.0}, tp0, tp1, tp2);

    std::cout << "br = (" << br[0] << ", " << br[1] << ", " << br[2] << ")" << std::endl;
    //    bary centric coordinates outside triangle do not satisfy sum(b)=1
    //    br = (1.2, 0.1, 0.1)

}

void multinomialCoefficient()
{
    std::cout << "(2, 2, 0, 0): " << vgeom::multinomialCoefficient2(2, 0, 0) << std::endl;
    std::cout << "(2, 0, 2, 0): " << vgeom::multinomialCoefficient2(0, 2, 0) << std::endl;
    std::cout << "(2, 0, 0, 2): " << vgeom::multinomialCoefficient2(0, 0, 2) << std::endl;
    std::cout << "(2, 1, 0, 1): " << vgeom::multinomialCoefficient2(1, 0, 1) << std::endl;
    std::cout << "(2, 1, 1, 0): " << vgeom::multinomialCoefficient2(1, 1, 0) << std::endl;
    std::cout << "(2, 0, 1, 1): " << vgeom::multinomialCoefficient2(0, 1, 1) << std::endl;
}

} // namespace Test


} // namespace Topology
