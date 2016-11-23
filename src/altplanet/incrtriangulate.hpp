#ifndef INCRTRIANGULATE_HPP
#define INCRTRIANGULATE_HPP

#include "planetshapes.h"
#include "triangulate.hpp"

namespace Triangulate
{

/*

// analyse edges

struct GraphState
{
    std::unordered_set<gfx::Triangle> existing_tris;
    std::unordered_set<gfx::Line> existing_edges;
    std::vector<gfx::Triangle> triangles,
};


inline void analyseEdges(const std::vector<vmath::Vector3> &points,
                         const std::vector<gfx::Triangle> &triangles,
                         int &num_edges,
                         std::vector<std::vector<int>> &point_point_adj)
{
    num_edges = 0;
    point_point_adj.resize(points.size());

    std::unordered_set<gfx::Line> existing_edges;
    for (const auto &tri : triangles)
    {
        for (int i=0; i<3; i++)
        {
            int i_next =(i+1)%3;
            std::array<int,2> inds = {tri[i], tri[i_next]};
            std::sort(inds.begin(), inds.end());

            gfx::Line line = {inds[0], inds[1]};

            if (existing_edges.find(line) == existing_edges.end())
            {
                existing_edges.insert(line);
                num_edges++;
                point_point_adj[inds[0]].push_back(inds[1]);
                point_point_adj[inds[1]].push_back(inds[0]);
            }
        }
    }
}


inline int eulerCharacteristic(int n_pts, int n_edges, int n_tris) {return n_pts-n_edges+n_tris;}


void incrTrisWithHash(float search_sphere_rad, int target_euler_char,                          // copied input
                      const std::vector<vmath::Vector3> &points, const SpaceHash3D &spacehash, // immutable inputs
                      std::unordered_map<gfx::Line>
                      std::unordered_set<gfx::Triangle> &existing_tris,                        // mutable input/output
                      std::vector<gfx::Triangle> &tris_out)                                    // mutable output
{
    int num_edges;
    std::vector<std::vector<int>> point_point_adj;
    analyseEdges(points, tris_out, num_edges, point_point_adj);
    int euler_char = eulerCharacteristic(points.size(), num_edges, tris_out.size());

    if (euler_char == target_euler_char)
    {
        std::cout << "target euler char reached: " << euler_char << ". Ending search" << std::endl;
        return; // finished
    }
    else //
    {
        std::cout << "euler char = " << euler_char << " continuing search with rad " << search_sphere_rad << std::endl;
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
        return incrTrisWithHash(search_sphere_rad+0.005, target_euler_char, points, spacehash, existing_tris, tris_out);
    }
}


void incrTrisWithHash(float search_sphere_rad,                                                 // copied input
                      const std::vector<vmath::Vector3> &points, const SpaceHash3D &spacehash, // immutable inputs
                      std::unordered_set<gfx::Triangle> &existing_tris,                        // mutable input/output
                      std::vector<gfx::Triangle> &tris_out)                                    // mutable output
{
    int num_edges;
    std::vector<std::vector<int>> point_point_adj;
    analyseEdges(points, tris_out, num_edges, point_point_adj);
    int euler_char = eulerCharacteristic(points.size(), num_edges, tris_out.size());

    if (euler_char == target_euler_char)
    {
        std::cout << "target euler char reached: " << euler_char << ". Ending search" << std::endl;
        return; // finished
    }
    else //
    {
        std::cout << "euler char = " << euler_char << " continuing search with rad " << search_sphere_rad << std::endl;
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
        return incrTrisWithHash(search_sphere_rad+0.005, target_euler_char, points, spacehash, existing_tris, tris_out);
    }
}



ReturnType incrTriangulate(const std::vector<vmath::Vector3> &points,
                           float search_sphere_rad, const SpaceHash3D &spacehash)
{
    ReturnType return_data;
    std::vector<gfx::Triangle> &tris_out = return_data.triangles;
    std::unordered_set<gfx::Triangle> &existing_tris = return_data.trianglesHash;

    incrTrisWithHash(0.01, 0, points, spacehash, existing_tris, tris_out);

    return return_data;
} //

*/

} // namespace Triangulate

#endif // INCRTRIANGULATE_HPP
