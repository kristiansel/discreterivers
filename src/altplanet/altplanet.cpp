#include "altplanet.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <tuple>
#include <unordered_set>


namespace AltPlanet
{

inline float frand(float LO, float HI)
{
    return LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
}


std::vector<gfx::Triangle> triangulate(const std::vector<vmath::Vector3> &points,
                                       const SpaceHash3D &spacehash);


Geometry generate(unsigned int n_points, const Shape::BaseShape &planet_shape)
{
    std::cout << "testing testing..." << std::endl;

    Geometry geometry;
    std::vector<vmath::Vector3> &points = geometry.points;
    std::vector<gfx::Triangle> &triangles = geometry.triangles;

    // generate random points
    Shape::AABB aabb = planet_shape.getAABB();


    points.reserve(n_points);

    for (unsigned int i=0; i<n_points; i++)
    {
        vmath::Vector3 point = {frand(-aabb.width*0.75, aabb.width*0.75),
                                frand(-aabb.height*0.75, aabb.height*0.75),
                                frand(-aabb.width*0.75, aabb.width*0.75)};

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
    int n_redistribute_iterations = 10;
    for (int i_red=0; i_red<n_redistribute_iterations; i_red++)
    {
        float it_repulsion_factor = i_red > 2 ? 0.003 : 0.008;
        std::cout << i_red << "/" << n_redistribute_iterations << std::endl;
        //float it_repulse_factor = repulse_factor/(sqrt(float(i_red)));
        pointsRepulse(points, spacehash, planet_shape, it_repulsion_factor);
    }

    // finished distributing them evenly...

    // triangulate... :)
    triangles = triangulate(points, spacehash);

    std::cout << "found " << triangles.size() << " triangles" << std::endl;

    //auto dummy = triangulate2(points, spacehash, planet_shape);

    return geometry;
}

void pointsRepulse(std::vector<vmath::Vector3> &points, SpaceHash3D &spacehash, const Shape::BaseShape &planet_shape, float repulse_factor)
{
    float largest_force = 0.f;

    std::vector<vmath::Vector3> pt_force;
    pt_force.resize(points.size());

    for (int i_p = 0; i_p<points.size(); i_p++)
    {
        pt_force[i_p] = {0.0f, 0.0f, 0.0f};
        //std::vector<int> neighbors = spacehash.findNeighbors(i_p);
        //for (const auto &i_n : neighbors)
        spacehash.forEachPointInSphere(points[i_p], 0.5f,[&](const int &i_n) -> bool
        {
            const auto diff_vector = points[i_p]-points[i_n];
            float diff_length = vmath::length(diff_vector);
            if (diff_length>0.0f)
            {
                pt_force[i_p] += repulse_factor * vmath::normalize(diff_vector)/(diff_length);
            }
            return false;
        });

        //for (int i = 0; i<3; i++) if(isnan(point[i])) std::cerr << "nan detected" << std::endl;

        // reduce the force size
        float force_size = vmath::length(pt_force[i_p]);
        float use_length = std::min(0.2f,force_size);
        pt_force[i_p] = use_length*vmath::normalize(pt_force[i_p]);

        // fix NaN
        for (int i = 0; i<3; i++) if(isnan(pt_force[i_p][i])) pt_force[i_p] = {0.f, 0.f, 0.f};

        // apply the force
        points[i_p] += pt_force[i_p];

        if (force_size>largest_force) largest_force=force_size;

        //std::cout << "n_neighbors: " << neighbors.size() << std::endl;
        //std::cout << "length(force):" << force_size << std::endl;
    }

    // check if nan
    for (const auto point : points)
    {
        for (int i = 0; i<3; i++) if(isnan(point[i])) std::cerr << "nan detected" << std::endl;
    }

    // reproject
    for (unsigned int i=0; i<points.size(); i++)
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

std::vector<gfx::Triangle> triangulate(const std::vector<vmath::Vector3> &points,
                                       const SpaceHash3D &spacehash)
{
    std::vector<gfx::Triangle> tris_out;
    std::unordered_set<gfx::Triangle> existing_tris;

    float search_sphere_rad = 0.24f;
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

    // find the number of edges
    int num_edges = 0;
    std::unordered_set<gfx::Line> existing_edges;
    for (const auto &tri : tris_out)
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
            }
        }
    }

    // compute the euler characteristic
    int euler_characteristic = points.size()-num_edges+tris_out.size();

    std::cout << "num points = " << points.size() << std::endl;
    std::cout << "num triangles = " << tris_out.size() << std::endl;
    std::cout << "num edges = " << num_edges << std::endl;
    std::cout << "euler characteristic = " << euler_characteristic << std::endl;

    // compare it with what it should be for a closed surface of the same topology as the planet shape


    return tris_out;
}

void orientTriangles(Geometry &geometry, const Shape::BaseShape &planet_shape)
{

}



}
