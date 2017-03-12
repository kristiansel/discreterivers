#ifndef ADJACENCY_H
#define ADJACENCY_H

#include <algorithm>
#include <vector>
#include "../common/gfx_primitives.h"
#include "../common/macro/macrodebugassert.h"

namespace AltPlanet {

namespace Adjacancy {

typedef int point_index;
typedef int tri_index;

template<class T>
inline void push_back_if_unique(std::vector<T> &v, const T &e)
{
    if (std::find(v.begin(), v.end(), e) == v.end()) v.push_back(e);
}

inline std::vector<std::vector<int>> createAdjacencyList( const std::vector<vmath::Vector3> &points,
                                                          const std::vector<gfx::Triangle> &triangles)
{
    // TODO: check integrity of inputs


    std::vector<std::vector<int>> edges;

    // add edges for all triangles
    edges.resize(points.size());

    for (const auto &triangle : triangles)
    {
        for (int i = 0; i<3; i++)
        {
            int j = (i+1)%3; // 1,2,3,0
            push_back_if_unique(edges[triangle[i]], triangle[j]);
            push_back_if_unique(edges[triangle[j]], triangle[i]);
        }
    }

    return edges;
}


inline std::vector<std::vector<int>> createPointToTriAdjacency( const std::vector<vmath::Vector3> &points,
                                                                const std::vector<gfx::Triangle> &triangles )
{
    std::vector<std::vector<int>> point_tri_adjacency;
    point_tri_adjacency =  std::vector<std::vector<int>>(points.size());
    for (int i = 0; i<triangles.size(); i++)
    {
        point_tri_adjacency[triangles[i][0]].push_back(i);
        point_tri_adjacency[triangles[i][1]].push_back(i);
        point_tri_adjacency[triangles[i][2]].push_back(i);
    }

    return point_tri_adjacency;
}

inline std::vector<std::vector<int>> createTriToTriAdjacency( const std::vector<gfx::Triangle> &triangles,
                                                              const std::vector<std::vector<int>> &point_tri_adjacency)
{
    std::vector<std::vector<int>> tri_tri_adjacency;
    tri_tri_adjacency =  std::vector<std::vector<int>>(triangles.size());
    for (int i = 0; i<triangles.size(); i++)
    {
        // create big list of all tri indices adjacent
        std::vector<int> thirteen_adj;
        thirteen_adj.reserve(18); // some will be added twice
        for (int j = 0; j<3; j++)
        {
            point_index i_p = triangles[i][j];
            for (const tri_index i_t : point_tri_adjacency[i_p]) thirteen_adj.push_back(i_t);
        }

        std::sort(thirteen_adj.begin(), thirteen_adj.end());
        auto last = std::unique(thirteen_adj.begin(), thirteen_adj.end());
        thirteen_adj.erase(last, thirteen_adj.end());

        // DEBUG_ASSERT(thirteen_adj.size()==13); // can't assert this because of pentagonal points

        // filter list on two common points
        for (const int i_t_adj: thirteen_adj)
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
                if (common_counter==2) tri_tri_adjacency[i].push_back(i_t_adj);
            }
        }

        DEBUG_ASSERT(tri_tri_adjacency[i].size()==3);
    }

    return tri_tri_adjacency;
}

} // namespace Adjacency

} // namespace AltPlanet

#endif // ADJACENCY_H
