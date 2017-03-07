#include "subivide.h"
#include <map>

#include "../common/mathext.h"
#include "../common/macro/macrodebugassert.h"

using namespace MathExt;

namespace AltPlanet
{

using IntpairIntMapType = std::map<std::pair<int, int>, int>;

inline vmath::Vector3 getMidpoint(const vmath::Vector3 &p1,
                                  const vmath::Vector3 &p2)
{
    vmath::Vector3 out = 0.5f*(p1 + p2);

    // could add a random component in every direction
    //float d = vmath::length(p2-p1)*0.125f;
    //
    //DEBUG_ASSERT(!(d!=d)&&"length between p1 and p2 is NaN");
    //
    //out = out + vmath::Vector3(frand(-d, d), frand(-d, d), frand(-d, d));
    //
    // problem with this approach: Triangles will end up crossing over each other...

    out = out + (p2-p1)*0.125f*frand(-1, 1); // this is good enough for now

    return out;
}

int getSubdPointIndex(const int i1,
                      const int i2,
                      std::vector<vmath::Vector3> &points,
                      IntpairIntMapType &midpoints_cache)
{
    // ordering of subdivision edge points is irrelevant, so
    // order i1 and i2 according to relative magnitude to avoid
    // duplicates in cache, i.e. don't want both (i1,i2) and (i2,i1)
    int i_lo = i1<i2 ? i1 : i2;
    int i_hi = i1>i2 ? i1 : i2;
    std::pair<int, int> midpoint_key = {i_lo, i_hi};

    // check if point exists in cache
    auto midpoint_it = midpoints_cache.find(midpoint_key);

    if (midpoint_it != midpoints_cache.end())
    {
        // just return the point from the cache
        return midpoint_it->second;
    }
    else
    {
        // create the point
        vmath::Vector3 mid_pt = getMidpoint(points[i_lo], points[i_hi]);
        points.push_back( mid_pt );
        int last_element_index = points.size() - 1;

        // add it to the cache
        midpoints_cache.insert( {midpoint_key, last_element_index} );
        return last_element_index;
    }
}

void subdivideOnce(  std::vector<vmath::Vector3> &points,
                     std::vector<gfx::Triangle> &triangles)
{
    // check input
    // check num_subdivisions positive

    // create a copy of old triangles
    std::vector<gfx::Triangle> old_triangles = triangles;
    triangles.clear();

    // Use a cache to keep points resulting from subidiving a shared edge
    IntpairIntMapType midpoints_cache;

    // for progress tracking
    std::cout << "[" << std::string(10, ' ') << "]";

    // get index of current last triangle
    int prev_subdlvl_size = old_triangles.size();
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
        gfx::Triangle const &prev_triangle = old_triangles[i];
        int new_points_indices[] = {
            getSubdPointIndex(prev_triangle[0], prev_triangle[1], points, midpoints_cache),
            getSubdPointIndex(prev_triangle[1], prev_triangle[2], points, midpoints_cache),
            getSubdPointIndex(prev_triangle[2], prev_triangle[0], points, midpoints_cache)
        };

        // create the subdivided triangles, using previously existing and new corners
        triangles.push_back(
            {
                prev_triangle[0],
                new_points_indices[0],
                new_points_indices[2]
            }
        );

        triangles.push_back(
            {
                prev_triangle[1],
                new_points_indices[1],
                new_points_indices[0]
            }
        );

        triangles.push_back(
            {
                prev_triangle[2],
                new_points_indices[2],
                new_points_indices[1]
            }
        );

        triangles.push_back(
            {
                new_points_indices[0],
                new_points_indices[1],
                new_points_indices[2]
            }
        );
    } // for (int i = 0; i < prev_subdlvl_size; i++)

    std::cout << std::endl;
}


void subdivideGeometry(  std::vector<vmath::Vector3> &points,
                         std::vector<gfx::Triangle> &triangles,
                         std::vector<std::vector<gfx::Triangle>> &subd_triangles,
                         const int num_subdivisions)
{
    // check input
    // check num_subdivisions positive

    // need one set of triangles for each subdivision level
    auto &triangles_subd_lvls = subd_triangles;

    // Create subdivision level zero by creating Icosahedron
    triangles_subd_lvls.push_back(triangles);

    // Subsequently subdivide triangles
    for (int k = 1; k < num_subdivisions + 1; k++)
    {
        std::cout << "subdividing level " << k << ": ";
        triangles_subd_lvls.push_back(std::vector<gfx::Triangle>());

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
            int new_points_indices[] = {
                getSubdPointIndex(prev_triangle[0], prev_triangle[1], points, midpoints_cache),
                getSubdPointIndex(prev_triangle[1], prev_triangle[2], points, midpoints_cache),
                getSubdPointIndex(prev_triangle[2], prev_triangle[0], points, midpoints_cache)
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

        std::cout << std::endl;

//        // compare cache size with cache initial reserved capacity
//        std::cout << "size of cached points ends up as" << midpoints_cache.size()
//                  << " cached points" << std::endl;
//        std::cout << "reserved capacity for " << num_cache_points_max
//                  << " cached points" << std::endl;
    }

    // copy the highest subdivision level to the resulting triangles
    triangles = triangles_subd_lvls.back();

    std::cout << "subdivided geometry to " << points.size()
              << " vertices and " << triangles.size() << " triangles"
              << std::endl;
    //    std::cout << "num_subdivisions = " << num_subdivisions << std::endl;
    //    std::cout << "reserved capacity for " << getNumPointsAtSubdivisionLevel(num_subdivisions)
    //              << " vertices and " << getNumTrisAtSubdivisionLevel(num_subdivisions) << " triangles"
    //              << std::endl;
}

} // namespace AltPlanet
