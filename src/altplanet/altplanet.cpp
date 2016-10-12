#include "altplanet.h"

#include <iostream>
#include <vector>
#include <cstdlib>

#include "spacehash3d.h"


namespace AltPlanet
{

inline float frand(float LO, float HI)
{
    return LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
}


std::vector<gfx::Triangle> triangulate(const std::vector<vmath::Vector3> &points,
                                       const SpaceHash3D &spacehash, const Shape::BaseShape &planet_shape);

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
    vmath::Vector3 max_corner = {aabb.width/2.0f, aabb.height/2.0f, aabb.width/2.0f};
    SpaceHash3D spacehash(points, -max_corner*1.5f, max_corner*1.5f, 20, 8, 20);


    // Distribute them (more) evenly
    std::vector<vmath::Vector3> pt_force;
    pt_force.resize(points.size());

    //int k_nn = 5; // nearest neighbors
    float repulse_factor = 0.0006f; // repulsive force factor

    int n_redistribute_iterations = 16;
    for (int i_red=0; i_red<n_redistribute_iterations; i_red++)
    {
        float largest_force = 0.f;

        float it_repulse_factor = repulse_factor/(sqrt(float(i_red)));

        for (int i_p = 0; i_p<points.size(); i_p++)
        {
            pt_force[i_p] = {0.0f, 0.0f, 0.0f};
            std::vector<int> neighbors = spacehash.findNeighbors(i_p);
            for (const auto &i_n : neighbors)
            {
                const auto diff_vector = points[i_p]-points[i_n];
                float diff_length = vmath::length(diff_vector);
                if (diff_length>0.0f)
                    pt_force[i_p] += it_repulse_factor * vmath::normalize(diff_vector)/(diff_length);
            }

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

        //std::cout << "largest force:"<<largest_force << std::endl;

        // check if nan
        for (const auto point : points)
        {
            for (int i = 0; i<3; i++) if(isnan(point[i])) std::cerr << "nan detected" << std::endl;
        }

        // reproject
        for (unsigned int i=0; i<n_points; i++)
        {
            // project onto shape
            points[i] = planet_shape.projectPoint(points[i]);
        }

        // update the spacehash
        spacehash.update(points);
    }

    // finished distributing them evenly...

    // triangulate... :(
    triangles = triangulate(points, spacehash, planet_shape);

    std::cout << "found " << triangles.size() << " triangles" << std::endl;

    return geometry;
}



std::vector<gfx::Triangle> triangulate(const std::vector<vmath::Vector3> &points,
                                       const SpaceHash3D &spacehash, const Shape::BaseShape &planet_shape)
{
    std::vector<gfx::Triangle> tris_out;

    // go block by block through the spacehash and
    // check if all combinations of center block to whole block form delaunay triangles
    int neighborhood_size = 3;

//    int all_cells = 0;

    spacehash.forEachCellNeighborhood(neighborhood_size,
        [&](const std::vector<int> &cell_pts, const std::vector<int> &neighbor_pts)
        {
//            all_cells +=cell_pts.size();
//            std::cout << "called: cellpts " << cell_pts.size() << ", neigh " << neighbor_pts.size() << std::endl;
            for (const auto cell_pt : cell_pts)
            {
                for (const auto neigh_pt1 : neighbor_pts)
                {
                    for (const auto neigh_pt2 : neighbor_pts)
                    {
                        //std::cout << "inside loop: " << std::endl;
                        //std::cout << "cell_pt: " << cell_pt << std::endl;
                        //std::cout << "neigh_pt1: " << neigh_pt1 << std::endl;
                        //std::cout << "neigh_pt2: " << neigh_pt2 << std::endl;

                        // all three points must be distinct
                        if (cell_pt!=neigh_pt1 && cell_pt!=neigh_pt2 && neigh_pt2!=neigh_pt1)
                        {
                            //std::cout << "inside if check" << std::endl;

                            // form a triangle and check if it is delaunay
                            if (spacehash.checkDelaunay(cell_pt, neigh_pt1, neigh_pt2, neighbor_pts))
                            {
                                tris_out.push_back({cell_pt, neigh_pt1, neigh_pt2});
                            }
                        }
                    }
                }
            }
        });

    return tris_out;
}

}
