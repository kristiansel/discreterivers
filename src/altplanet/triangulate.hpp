#ifndef TRIANGULATE_HPP
#define TRIANGULATE_HPP

#include <array>
#include <vector>
#include <unordered_set>

#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"

#include "../common/gfx_primitives.h"
#include "spacehash3d.h"

namespace Triangulate
{

struct ReturnType
{
    std::vector<gfx::Triangle> triangles;
    std::unordered_set<gfx::Triangle> trianglesHash;
};

ReturnType trianglesWithHash(const std::vector<vmath::Vector3> &points,
                                       float search_sphere_rad, const SpaceHash3D &spacehash)
{
    ReturnType return_data;
    std::vector<gfx::Triangle> &tris_out = return_data.triangles;
    std::unordered_set<gfx::Triangle> &existing_tris = return_data.trianglesHash;

    for (int i_p = 0; i_p<points.size(); i_p++)
    {
        std::vector<int> neighbor_pts;
        spacehash.forEachPointInSphere(points[i_p], search_sphere_rad, [&](const int &p) -> bool
        {
            neighbor_pts.push_back(p);
            bool early_return = false;
            return early_return;
        });

        //std::cout << "neighbors: " << neighbors.size() << std::endl;

        for (const auto neigh_pt1 : neighbor_pts)
        {
            for (const auto neigh_pt2 : neighbor_pts)
            {
                //std::cout << "inside loop: " << std::endl;
                //std::cout << "cell_pt: " << cell_pt << std::endl;
                //std::cout << "neigh_pt1: " << neigh_pt1 << std::endl;
                //std::cout << "neigh_pt2: " << neigh_pt2 << std::endl;

                // sort the indices, this is needed for proper hashing and equivalince checking
                std::array<int,3> inds = {i_p, neigh_pt1, neigh_pt2};
                std::sort(inds.begin(), inds.end());
                auto last = std::unique(inds.begin(), inds.end());

                // all three points must be unique for a proper triangle
                if (last==inds.end())
                {
                    gfx::Triangle tri = {inds[0], inds[1], inds[2]};

                    // check if the triangle is already created
                    if (existing_tris.find(tri) == existing_tris.end())
                    {
                        // check if the triangle is delaunay w.r.t neighboring points
                        if (spacehash.sphereCheckDelaunayGlobal(inds[0], inds[1], inds[2]))
                        {
                            tris_out.push_back(tri);

                            existing_tris.insert(tri);
                        }
                    }
                }
            }
        }
    }

    return return_data;
}

std::vector<gfx::Triangle> triangulate(const std::vector<vmath::Vector3> &points,
                                       float search_sphere_rad, const SpaceHash3D &spacehash)
{
    return trianglesWithHash(points, search_sphere_rad, spacehash).triangles;
}


std::vector<gfx::Triangle> triangulate(const std::vector<vmath::Vector3> &points,
                                       float search_sphere_rad)
{
    SpaceHash3D spacehash(points);
    triangulate(points, search_sphere_rad, spacehash);
}

}

#endif // TRIANGULATE_HPP
