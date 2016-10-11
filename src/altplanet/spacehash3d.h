#ifndef SPACEHASH3D_H
#define SPACEHASH3D_H

#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"

namespace vmath = Vectormath::Aos;

#include <vector>
#include <cmath>

class SpaceHash3D
{
private:
    //static const int AV_PTS_PER_CELL_PREF = 8;

    inline int localToGlobal(int i, int j, int k) {return i+j*nx+k*nx*ny;}
    // inline int globalToLocal(int I) {...}

    inline int findCell_i(const vmath::Vector3 &point) {return floor((point[0]-min[0])/(max[0]-min[0])*float(nx));}
    inline int findCell_j(const vmath::Vector3 &point) {return floor((point[1]-min[1])/(max[1]-min[1])*float(ny));}
    inline int findCell_k(const vmath::Vector3 &point) {return floor((point[2]-min[2])/(max[2]-min[2])*float(nz));}

    inline int findPointCell(const vmath::Vector3 &point) {
        int i = findCell_i(point);
        int j = findCell_j(point);
        int k = findCell_k(point);

        //std::cout << "p:"; vmath::print(point);
        //std::cout << "i:" << i << "j:" << j << ",k:" << k << std::endl;

        return localToGlobal(i,j,k);
    }

    inline void updatePoints(const std::vector<vmath::Vector3> &points) // woops grid_to_point_map shoud be cleared!!
    {
        for (int i = 0; i<points.size(); i++)
        {
            int I = findPointCell(points[i]);
            grid_to_point_map[I].push_back(i);
        }
    }

public:
    SpaceHash3D(const std::vector<vmath::Vector3> &points,
                vmath::Vector3 min, vmath::Vector3 max,
                int nx, int ny, int nz) :
        min(min), max(max), nx(nx), ny(ny), nz(nz), points(points)
    {
        n_grid = nx*ny*nz;
        grid_to_point_map = new std::vector<int> [n_grid];

        updatePoints(points);

//        int max_cell_size = 0;
//        for (int i_cell = 0; i_cell<n_grid; i_cell++)
//        {
//            if (grid_to_point_map[i_cell].size()>max_cell_size)
//                max_cell_size = grid_to_point_map[i_cell].size();
//        }
//        std::cout << "max cell size: " << max_cell_size << std::endl;
    }

    virtual ~SpaceHash3D() {delete [] grid_to_point_map;}

    static const int search_width = 2;

    std::vector<int> findNeighbors(int i_p) // don't need exactly k nearest
    {
        const auto &point = points[i_p];
        // find cell of 3d point
        int i_mid = findCell_i(point);
        int j_mid = findCell_j(point);
        int k_mid = findCell_k(point);


        std::vector<int> out_pts;
        // search a 3 by 3 grid around middle cell
        for (int i = std::max(i_mid-search_width, 0);i<std::min(i_mid+search_width,nx);i++)
        {
            for (int j = std::max(j_mid-search_width,0);j<std::min(j_mid+search_width,ny);j++)
            {
                for (int k = std::max(k_mid-search_width,0);k<std::min(k_mid+search_width,nz);k++)
                {
                    int I = localToGlobal(i,j,k);
                    //std::cout << "i:" << i << "j:" << j << ",k:" << k << std::endl;
                    for (const auto pt : grid_to_point_map[I])
                        if (pt!=i_p) out_pts.push_back(pt);
                }
            }
        }

        return out_pts;
    }

    void update(const std::vector<vmath::Vector3> &points)
    {
        for (int i = 0; i<n_grid; i++) grid_to_point_map[i].clear();

        updatePoints(points);
    }

private:
    std::vector<vmath::Vector3> points;
    std::vector<int> * grid_to_point_map;
    int n_grid;

    vmath::Vector3 min;
    vmath::Vector3 max;

    const int nx;
    const int ny;
    const int nz;

};

#endif // SPACEHASH3D_H
