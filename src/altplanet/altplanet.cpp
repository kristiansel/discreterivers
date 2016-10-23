#include "altplanet.h"
#include "triangulate.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <tuple>
#include <unordered_set>
#include <vector>


namespace AltPlanet
{

inline float frand(float LO, float HI)
{
    return LO + static_cast <float> (rand())/( static_cast <float> (RAND_MAX/(HI-LO)));
}

bool isnan_lame(float x)
{
    return x != x;
}


std::vector<gfx::Triangle> triangulateAndOrient(const std::vector<vmath::Vector3> &points,
                                                const SpaceHash3D &spacehash,
                                                const Shape::BaseShape &planet_shape);


Geometry generate(unsigned int n_points, const Shape::BaseShape &planet_shape)
{
    std::cout << "testing testing..." << std::endl;

    Geometry geometry;
    std::vector<vmath::Vector3> &points = geometry.points;
    std::vector<gfx::Triangle> &triangles = geometry.triangles;

    // generate random points
    Shape::AABB aabb = planet_shape.getAABB();


    points.reserve(n_points);

    for (unsigned int i = 0; i < n_points; i++)
    {
        vmath::Vector3 point = {frand(-aabb.width *0.75, aabb.width *0.75),
                                frand(-aabb.height*0.75, aabb.height*0.75),
                                frand(-aabb.width *0.75, aabb.width *0.75)};

        // project onto shape
        point = planet_shape.projectPoint(point);

        points.push_back(point);
    }

    // hash the points onto a 3d grid
    SpaceHash3D spacehash(points);

    // Distribute them (more) evenly
    std::vector<vmath::Vector3> pt_force;
    pt_force.resize(points.size());

    //int k_nn = 5; // nearest neighbors
    //float repulse_factor = 0.006f; // repulsive force factor

    std::cout << "distributing points evenly..." << std::endl;
    int n_redistribute_iterations = 25;
    for (int i_red = 0; i_red < n_redistribute_iterations; i_red++)
    {
        float it_repulsion_factor = i_red > 2 ? 0.003f : 0.008f;
        std::cout << i_red << "/" << n_redistribute_iterations << std::endl;
        //float it_repulse_factor = repulse_factor/(sqrt(float(i_red)));
        pointsRepulse(points, spacehash, planet_shape, it_repulsion_factor);
    }

    // finished distributing them evenly...

    // triangulate... :)
    triangles = triangulateAndOrient(points, spacehash, planet_shape);

    std::cout << "found " << triangles.size() << " triangles" << std::endl;

    //auto dummy = triangulate2(points, spacehash, planet_shape);

    return geometry;
}

void pointsRepulse(std::vector<vmath::Vector3> &points, SpaceHash3D &spacehash, const Shape::BaseShape &planet_shape, float repulse_factor)
{
    float largest_force = 0.0f;

    std::vector<vmath::Vector3> pt_force;
    pt_force.resize(points.size());

    for (int i_p = 0; i_p<points.size(); i_p++)
    {
        pt_force[i_p] = {0.0f, 0.0f, 0.0f};
        //std::vector<int> neighbors = spacehash.findNeighbors(i_p);
        //for (const auto &i_n : neighbors)
        spacehash.forEachPointInSphere(points[i_p], 0.5f,
                                       [&](const int &i_n) -> bool
                                       {
                                           const auto diff_vector = points[i_p] - points[i_n];
                                           float diff_length = vmath::length(diff_vector);
                                           if (diff_length > 0.0f)
                                           {
                                               pt_force[i_p] += repulse_factor * vmath::normalize(diff_vector)/(diff_length);
                                           }
                                           return false;
                                       });

        //for (int i = 0; i < 3; i++) if(isnan_lame(point[i])) std::cerr << "nan detected" << std::endl;

        // reduce the force size
        float force_size = vmath::length(pt_force[i_p]);
        float use_length = std::min(0.2f, force_size);
        pt_force[i_p] = use_length*vmath::normalize(pt_force[i_p]);

        // fix NaN
        for (int i = 0; i < 3; i++) if(isnan_lame(pt_force[i_p][i])) pt_force[i_p] = {0.f, 0.f, 0.f};

        // apply the force
        points[i_p] += pt_force[i_p];

        if (force_size > largest_force) largest_force = force_size;

        //std::cout << "n_neighbors: " << neighbors.size() << std::endl;
        //std::cout << "length(force):" << force_size << std::endl;
    }

    // check if nan
    for (const auto point : points)
    {
        for (int i = 0; i < 3; i++) if(isnan_lame(point[i])) std::cerr << "nan detected" << std::endl;
    }

    // reproject
    for (unsigned int i = 0; i < points.size(); i++)
    {
        // project onto shape
        points[i] = planet_shape.projectPoint(points[i]);
    }

    // rehash the points
    spacehash.rehash(points);
}

void pointsRepulse(std::vector<vmath::Vector3> &points, const Shape::BaseShape &planet_shape, float repulse_factor)
{
    SpaceHash3D spacehash(points);
    pointsRepulse(points, spacehash, planet_shape, repulse_factor);
}

//bool custom_tri_equal(gfx::Triangle t1, gfx::Triangle t2)
//{
//    std::array<int,3> inds = {t1[0], t1[1], t1[2]};
//    std::sort(inds.begin(), inds.end());
//}

inline vmath::Vector3 triangleCross(const gfx::Triangle &triangle, const std::vector<vmath::Vector3> &points)
{
    const auto v1 = points[triangle[1]] - points[triangle[0]];
    const auto v2 = points[triangle[2]] - points[triangle[0]];
    return vmath::cross(v1,v2);
}

inline vmath::Vector3 triangleNormal(const gfx::Triangle &triangle, const std::vector<vmath::Vector3> &points)
{
    return vmath::normalize(triangleCross(triangle, points));
}

void orientTriangles(const std::vector<vmath::Vector3> &points,
                     std::vector<gfx::Triangle> &triangles,
                     const Shape::BaseShape &planet_shape)
{
    for (int i_t = 0; i_t < triangles.size(); i_t++)
    {
        gfx::Triangle &tri = triangles[i_t];
        const auto v1 = points[tri[1]] - points[tri[0]];
        const auto v2 = points[tri[2]] - points[tri[0]];

        if (vmath::dot(vmath::cross(v1,v2), planet_shape.getGradDir(points[tri[0]])) < 0.0f)
        {
            // reverse the order of the triangle indices
            std::swap(tri[0], tri[2]);
        }
    }
}

inline void surfaceGradFilterTriangles( const std::vector<vmath::Vector3> &points,
                                        std::vector<gfx::Triangle> &triangles,
                                        const Shape::BaseShape &planet_shape)
{
    float n_dot_thresh = 0.9f;
    auto remove_predicate = [&](const gfx::Triangle &tri) -> bool
            {
                vmath::Vector3 tri_n = triangleNormal(tri, points);
                vmath::Vector3 av_pt = 0.33f*(points[tri[0]] + points[tri[1]] + points[tri[2]]);
                vmath::Vector3 grad_planet_n = vmath::normalize(planet_shape.getGradDir(av_pt));
                return vmath::dot(tri_n, grad_planet_n) < n_dot_thresh;
            };
    triangles.erase(std::remove_if(triangles.begin(), triangles.end(), remove_predicate), triangles.end());
}

std::vector<gfx::Triangle> triangulateAndOrient(const std::vector<vmath::Vector3> &points,
                                                const SpaceHash3D &spacehash, const Shape::BaseShape &planet_shape)
{
    // create a triangulation
    Triangulate::ReturnType trisandhash = Triangulate::trianglesWithHash(points, 0.27f, spacehash);
    std::vector<gfx::Triangle> &triangles = trisandhash.triangles;
    //std::unordered_set<gfx::Triangle> &existing_tris = trisandhash.trianglesHash;

    // orient triangles
    orientTriangles(points, triangles, planet_shape);

    // filter away triangles with normals different from planet shape normal
    surfaceGradFilterTriangles(points, triangles, planet_shape);

    // for each points, check if any adjacent (slightly shrunk) triangles intersect
    // if they do, then remove one...

    // or change to doubles... (for that need to change library... or roll own)

    // analyse edges
    int num_edges = 0;
    std::unordered_set<gfx::Line> existing_edges;
    std::vector<std::vector<int>> point_point_adj(points.size());
    for (const auto &tri : triangles)
    {
        for (int i = 0; i < 3; i++)
        {
            int i_next =(i + 1) % 3;
            std::array<int, 2> inds = {tri[i], tri[i_next]};
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

    // if the edges are sorted, should be enough to check if each consecutive edge form a triangle with mid point...

    // // std::cout << "sorting point connections ccw" << std::endl;
    // // challenge, sort edges counter clockwise...
    // // first create a function that monotonously increases as vectors go ccw around a centerpoint
    // auto ccw_rank = [](const vmath::Vector3 &v, const vmath::Vector3 &v_ref, const vmath::Vector3 &n_ref)
    //         {
    //             float angle = acos(vmath::dot(v, v_ref)/(vmath::length(v)*vmath::length(v_ref)));
    //             float l_cross = vmath::dot(vmath::cross(v,v_ref), n_ref);
    //             return (l_cross > 0.0f) ? angle : M_2_PI-angle;
    //         };

    // // then make a custom comparison function
    // auto ccw_cmp = [&](const vmath::Vector3 &v1, const vmath::Vector3 &v2, const vmath::Vector3 &v_ref, const vmath::Vector3 &n_ref)
    //         {
    //             return ccw_rank(v1, v_ref, n_ref) < ccw_rank(v2, v_ref, n_ref);
    //         };

    // // iterate through all adjacency list and sort them in counter clockwise rotation
    // for (int i_p = 0; i_p<point_point_adj.size(); i_p++)
    // {
    //     auto adj_list = point_point_adj[i_p];

    //     auto v1 = points[adj_list[0]]-points[i_p];
    //     auto v2 = points[adj_list[1]]-points[i_p];

    //     auto v_ref = 0.5f*(v1+v2);
    //     auto n_ref = vmath::normalize(planet_shape.getGradDir(points[i_p]));

    //     auto ccw_cmp_indexbased = [&](int i_n1, int i_n2)
    //             {
    //                 return ccw_cmp(points[i_n1]-points[i_p], points[i_n2]-points[i_p], v_ref, n_ref);
    //             };
    //     std::sort(adj_list.begin(), adj_list.end(), ccw_cmp_indexbased);
    // }

    // std::cout << "finished sorting point connections ccw" << std::endl;

    // compute the euler characteristic
    int euler_characteristic = points.size()-num_edges+triangles.size();

    std::cout << "num points = " << points.size() << std::endl;
    std::cout << "num triangles = " << triangles.size() << std::endl;
    std::cout << "num edges = " << num_edges << std::endl;
    std::cout << "euler characteristic = " << euler_characteristic << std::endl;

    /*
    // compare it with what it should be for a closed surface of the same topology as the planet shape
    // complicated...

    // check if the surface is closed..
    //  for all points
    //      go to a (first) neighbor
    //        go to next neighbor,
    //          check if it also has first pt as neighbor and
    //          find out if the two neighbors and pt share a triangle...
    //              if either false, then we have a hole...
    //      next neighbor now becomes first neighbor,
    //          find new next neighbor etc
    //      until back at first neighbor

    for (int i_p=0; i_p<points.size(); i_p++)
    {
    const auto &p_adj = point_point_adj[i_p];
    int n_adj = p_adj.size();

    for (int i_n1=0; i_n1<n_adj; i_n1++)
    {
    int i_n2 = (i_n1+1)%n_adj;
    std::array<int,3> inds = {i_p, i_n1, i_n2};
    std::sort(inds.begin(), inds.end());

    gfx::Triangle tri{inds[0], inds[1], inds[2]};
    if (existing_tris.find(tri) == existing_tris.end())
    {
    std::cout << "found a hole" << std::endl;
    }
    }
    }

    */

    return triangles;
}




}
