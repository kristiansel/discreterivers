#ifndef JITTERPOINTS_H
#define JITTERPOINTS_H

#include <vector>

#include "adjacency.h"
#include "../common/macro/debuglog.h"
#include "../common/gfx_primitives.h"
#include "../common/mathext.h"


namespace AltPlanet {

void jitterPoints(std::vector<vmath::Vector3> &points,
                  std::vector<gfx::Triangle> &triangles)
{
    // construct the point-point adjacency
    std::vector<std::vector<int>> point_to_point_adjacency = Adjacancy::createAdjacencyList(points, triangles);

    // for each point
    for (int i = 0; i<points.size(); i++)
    {
    //      find the point-point edge with the lowest distance
        int i_closest = -1;
        float closest_dist_sqr = std::numeric_limits<float>::max();
        for (int j = 0; j<point_to_point_adjacency[i].size(); j++)
        {
            int i_neighbor = point_to_point_adjacency[i][j];
            const vmath::Vector3 &neighbor = points[i_neighbor];
            const vmath::Vector3 to_neighbor = neighbor-points[i];
            float dist = vmath::lengthSqr(to_neighbor);
            if (dist < closest_dist_sqr)
            {
                i_closest = i_neighbor;
                closest_dist_sqr = dist;
            }
        }
        DEBUG_ASSERT(i_closest >= 0 && i_closest<points.size());

        vmath::Vector3 smallest_vec = points[i_closest]-points[i];
        //float jitter_len = MathExt::frand(0.0f, 0.5f*vmath::length(smallest_vec));
        float jitter_len = 0.3f*vmath::length(smallest_vec);

        float rand_angle = MathExt::frand(0.0f, 2.0*DR_M_PI);


        vmath::Vector3 adj0 = points[point_to_point_adjacency[i][0]];
        vmath::Vector3 adj1 = points[point_to_point_adjacency[i][1]];
        vmath::Vector3 norm = vmath::normalize(vmath::cross(adj0-points[i], adj1-points[i]));

        vmath::Matrix3 rot_mat = vmath::Matrix3::rotation(rand_angle, norm);
        vmath::Vector3 rot_vec = jitter_len * vmath::normalize(adj0);

        vmath::Vector3 jitter_vec = rot_mat * rot_vec;

        if (i==3)
        {
            DEBUG_LOG("points[i]");
            vmath::print(points[i]);
            DEBUG_LOG("points[i_closest]");
            vmath::print(points[i_closest]);
            DEBUG_LOG("lenght between = " << vmath::length(smallest_vec) <<
                      ", jitter = " << jitter_len << ", rand_angle = " << rand_angle <<
                      ", jitter_vec = ")
            vmath::print(jitter_vec);
            DEBUG_LOG(", new points[i] = ");
            vmath::print(points[i] + jitter_vec);

        }


        points[i] = points[i] + jitter_vec; // aiai, should maybe not be in place, result depends on points ordering

    }
}

}

#endif // JITTERPOINTS_H
