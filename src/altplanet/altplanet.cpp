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

std::vector<vmath::Vector3>  generate(unsigned int n_points, const Shape::BaseShape &planet_shape)
{
    std::cout << "testing testing..." << std::endl;

    // generate random points
    Shape::AABB aabb = planet_shape.getAABB();

    std::vector<vmath::Vector3> points;
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
    SpaceHash3D spacehash(points, -max_corner, max_corner, 30, 10, 30);



    return points;
}

}
