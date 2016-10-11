#ifndef SPACEHASH3D_H
#define SPACEHASH3D_H

#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"

namespace vmath = Vectormath::Aos;

#include <vector>
#include <cmath>

class SpaceHash3D
{
public:
    //static const int AV_PTS_PER_CELL_PREF = 8;

    SpaceHash3D(const std::vector<vmath::Vector3> &points,
                vmath::Vector3 min, vmath::Vector3 max,
                unsigned int nx, unsigned int ny, unsigned int nz) :
        min(min), max(max), nx(nx), ny(ny), nz(nz)
    {
        grid_to_point_map = new std::vector<int> [nx*ny*nz];

        /*for (const auto &point : points)
        {
            unsigned int i = (point.x-min)
            point_to_grid_map.push_back();
        }*/
    }

    virtual ~SpaceHash3D() {delete [] grid_to_point_map;}
private:
    std::vector<int> point_to_grid_map;
    std::vector<int> * grid_to_point_map;

    vmath::Vector3 min;
    vmath::Vector3 max;

    unsigned int nx, ny, nz;

};

#endif // SPACEHASH3D_H
