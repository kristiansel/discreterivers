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

    inline int localToGlobal(int i, int j, int k) const {return i+j*nx+k*nx*ny;}
    // inline int globalToLocal(int I) {...}

    inline int findCell_i(const vmath::Vector3 &point) const {return floor((point[0]-min[0])/(max[0]-min[0])*float(nx));}
    inline int findCell_j(const vmath::Vector3 &point) const {return floor((point[1]-min[1])/(max[1]-min[1])*float(ny));}
    inline int findCell_k(const vmath::Vector3 &point) const {return floor((point[2]-min[2])/(max[2]-min[2])*float(nz));}

    inline int findPointCell(const vmath::Vector3 &point) const {
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

    std::vector<int> findNeighbors(int i_p)  const // don't need exactly k nearest
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

    template<class F>
    void forEachCellNeighborhood(int neighborhood_size, F func) const;

    bool checkDelaunay(int tri_pt0, int tri_pt1, int tri_pt2, const std::vector<int> &neighbors) const;

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

// implement template methods
template<class F>
void SpaceHash3D::forEachCellNeighborhood(int neighborhood_size, F func) const
{
    assert((neighborhood_size>=0));
    int sw = neighborhood_size/2;

    std::vector<int> neighbors;

    for (int i_mid = 0; i_mid<nx; i_mid++)
    {
        for (int j_mid = 0; j_mid<ny; j_mid++)
        {
            for (int k_mid = 0; k_mid<nz; k_mid++)
            {
                neighbors.clear();
                for (int i = std::max(i_mid-sw, 0);i<std::min(i_mid+sw, nx);i++)
                {
                    for (int j = std::max(j_mid-sw,0);j<std::min(j_mid+sw, ny);j++)
                    {
                        for (int k = std::max(k_mid-sw,0);k<std::min(k_mid+sw, nz);k++)
                        {
                            int I = localToGlobal(i,j,k);
                            for (const auto pt : grid_to_point_map[I])
                                neighbors.push_back(pt);
                        }
                    }
                }

                // call the supplied lambda on this cell's points and neighborhood points
                int I = localToGlobal(i_mid,j_mid,k_mid);
                func(grid_to_point_map[I], neighbors);
            }
        }
    }
}


struct CircumDisk
{
    vmath::Vector3 center;
    vmath::Vector3 normal;
    float radius;
};

inline CircumDisk getCircumDisk(const vmath::Vector3 &pt1, const vmath::Vector3 &pt2, const vmath::Vector3 &pt3);
inline bool checkInCircumDisk(const vmath::Vector3 &pt, const CircumDisk &cdisk);

bool SpaceHash3D::checkDelaunay(int tri_pt0, int tri_pt1, int tri_pt2, const std::vector<int> &neighbors) const
{
    CircumDisk circum_disk = getCircumDisk(points[tri_pt0], points[tri_pt1], points[tri_pt2]);

//    std::cout << "circum_disk: " << std::endl;
//    std::cout << "center: "; vmath::print(circum_disk.center);
//    std::cout << "normal: "; vmath::print(circum_disk.normal);
//    std::cout << "radius: " << circum_disk.radius << std::endl;

    // for all points in neighborhood, check if circumdisk, if any point in circumdisk, then false otherwise true
    for (const auto i_n : neighbors)
    {
        if (i_n!=tri_pt0 && i_n!=tri_pt1 && i_n!=tri_pt2) // do not check tri itself
        {
            if (checkInCircumDisk(points[i_n], circum_disk))
            {
                return false;
            }
        }
    }

    return true;
}


inline vmath::Vector3 triplePlaneIntersection(vmath::Vector3 n1, vmath::Vector3 p1,
                                       vmath::Vector3 n2, vmath::Vector3 p2,
                                       vmath::Vector3 n3, vmath::Vector3 p3)
{
    vmath::Matrix3 n_mat(n1, n2, n3);
    vmath::Vector3 b = vmath::dot(p1,n1)*vmath::cross(n2, n3) +
                       vmath::dot(p2,n2)*vmath::cross(n3, n1) +
                       vmath::dot(p3,n3)*vmath::cross(n1, n2);

    return vmath::inverse(n_mat) * b;
}

inline CircumDisk getCircumDisk(const vmath::Vector3 &pt1, const vmath::Vector3 &pt2, const vmath::Vector3 &pt3)
{
    vmath::Vector3 v1 = vmath::normalize(pt2-pt1);
    vmath::Vector3 v2 = vmath::normalize(pt3-pt1);
    vmath::Vector3 tri_norm = vmath::cross(v1, v2);

    vmath::Vector3 center = triplePlaneIntersection(v1, 0.5f*(pt2+pt1), // perpendicular bisector plane 1
                                                    v2, 0.5f*(pt3+pt1), // perpendicular bisector plane 2
                                                    tri_norm, pt1); // triangle plane

    float radius = vmath::length(pt1-pt2);

    return {center, tri_norm, radius};

}

vmath::Vector3 projectPointIntoPlane(const vmath::Vector3 &point,
                                     const vmath::Vector3 &plane_normal,
                                     const vmath::Vector3 &plane_point)
{
    float discr = vmath::dot(plane_normal, plane_normal);
    assert((discr>0.0f));
    float d = vmath::dot(plane_point-point, plane_normal)/discr;
    return point + d*plane_normal;
}

inline bool checkInCircumDisk(const vmath::Vector3 &pt, const CircumDisk &cdisk)
{
    // project point onto circumdisk plane
    projectPointIntoPlane(pt, cdisk.normal, cdisk.center);

    // check if distance less than radius
    return ( vmath::length(pt-cdisk.center) < cdisk.radius );
}


#endif // SPACEHASH3D_H
