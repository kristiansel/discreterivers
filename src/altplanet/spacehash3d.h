#ifndef SPACEHASH3D_H
#define SPACEHASH3D_H

#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"
#include "../common/macro/macrodebugassert.h"

namespace vmath = Vectormath::Aos;

#include <vector>
#include <cmath>
#include <limits>

class SpaceHash3D
{
public:
    SpaceHash3D(const std::vector<vmath::Vector3> &points);
    virtual ~SpaceHash3D();

    void update(const std::vector<vmath::Vector3> &points);
    void rehash(const std::vector<vmath::Vector3> &points);

    template<class Func>
    void forEachPointInSphere(vmath::Vector3 center, float radius, Func func) const;

    bool sphereCheckDelaunayGlobal(int tri_pt0, int tri_pt1, int tri_pt2) const;

    float getPointDensity() const;
    /*
    template<class F>
    void forEachCellNeighborhood(int neighborhood_size, F func) const;
    */

    // bool checkDelaunay(int tri_pt0, int tri_pt1, int tri_pt2, const std::vector<int> &neighbors) const;

protected:


private:
    std::vector<vmath::Vector3> mPoints;
    std::vector<int> * mGridToPointMap;
    int mNGrid;

    vmath::Vector3 mMin;
    vmath::Vector3 mMax;

    int mNx;
    int mNy;
    int mNz;

    float mPointCubeVolDensity;
    float mPointCubeAreaDensity;

private:
    static constexpr float AV_PTS_PER_CELL_PREF = 0.2f;

    inline int localToGlobal(int i, int j, int k) const {return i+j*mNx+k*mNx*mNy;}

    inline int findCell_i(const vmath::Vector3 &point) const {return floor((point[0]-mMin[0])/(mMax[0]-mMin[0])*float(mNx));}
    inline int findCell_j(const vmath::Vector3 &point) const {return floor((point[1]-mMin[1])/(mMax[1]-mMin[1])*float(mNy));}
    inline int findCell_k(const vmath::Vector3 &point) const {return floor((point[2]-mMin[2])/(mMax[2]-mMin[2])*float(mNz));}

    inline int findPointCell(const vmath::Vector3 &point) const {
        int i = findCell_i(point);
        int j = findCell_j(point);
        int k = findCell_k(point);

        return localToGlobal(i,j,k);
    }
};

struct CircumDisk
{
    vmath::Vector3 center;
    vmath::Vector3 normal;
    float radius;
};

inline CircumDisk getCircumDisk(const vmath::Vector3 &pt1, const vmath::Vector3 &pt2, const vmath::Vector3 &pt3);
inline bool checkInCircumDisk(const vmath::Vector3 &pt, const CircumDisk &cdisk);

/*
bool SpaceHash3D::checkDelaunay(int tri_pt0, int tri_pt1, int tri_pt2, const std::vector<int> &neighbors) const
{
    CircumDisk circum_disk = getCircumDisk(points[tri_pt0], points[tri_pt1], points[tri_pt2]);

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
*/


inline vmath::Vector3 triplePlaneIntersection(vmath::Vector3 n1, vmath::Vector3 p1,
                                       vmath::Vector3 n2, vmath::Vector3 p2,
                                       vmath::Vector3 n3, vmath::Vector3 p3)
{
    vmath::Matrix3 n_mat(n1, n2, n3);
    vmath::Vector3 b = vmath::dot(p1,n1)*vmath::cross(n2, n3) +
                       vmath::dot(p2,n2)*vmath::cross(n3, n1) +
                       vmath::dot(p3,n3)*vmath::cross(n1, n2);

    float det_n = vmath::determinant(n_mat);
    DEBUG_ASSERT((det_n!=0.0f));
    return b / det_n;
}

inline CircumDisk getCircumDisk(const vmath::Vector3 &pt1, const vmath::Vector3 &pt2, const vmath::Vector3 &pt3)
{
    vmath::Vector3 v1 = vmath::normalize(pt2-pt1);
    vmath::Vector3 v2 = vmath::normalize(pt3-pt1);
    vmath::Vector3 tri_norm = vmath::cross(v1, v2);

    vmath::Vector3 center = triplePlaneIntersection(v1, 0.5f*(pt2+pt1), // perpendicular bisector plane 1
                                                    v2, 0.5f*(pt3+pt1), // perpendicular bisector plane 2
                                                    tri_norm, pt1); // triangle plane

    float radius = vmath::length(pt1-center);

    return {center, tri_norm, radius};

}

inline vmath::Vector3 projectPointIntoPlane(const vmath::Vector3 &point,
                                     const vmath::Vector3 &plane_normal,
                                     const vmath::Vector3 &plane_point)
{
    float discr = vmath::dot(plane_normal, plane_normal);
    assert((discr>0.0f));
    float d = vmath::dot(plane_point-point, plane_normal)/discr;
    return point + d*plane_normal;
}

/*
inline bool checkInCircumDisk(const vmath::Vector3 &pt, const CircumDisk &cdisk)
{
    // project point onto circumdisk plane
    projectPointIntoPlane(pt, cdisk.normal, cdisk.center);

    // check if distance less than radius
    return ( vmath::length(pt-cdisk.center) < cdisk.radius );
}*/

template<class Func>
void SpaceHash3D::forEachPointInSphere(vmath::Vector3 center, float radius, Func func) const
{
    int i_mid = findCell_i(center);
    int j_mid = findCell_j(center);
    int k_mid = findCell_k(center);

    float cell_size_x = (mMax[0]-mMin[0])/(float)(mNx);
    float cell_size_y = (mMax[1]-mMin[1])/(float)(mNy);
    float cell_size_z = (mMax[2]-mMin[2])/(float)(mNz);

    int offset_why_needed = 1;
    int n_half_x = ceil(radius/cell_size_x)+offset_why_needed;
    int n_half_y = ceil(radius/cell_size_y)+offset_why_needed;
    int n_half_z = ceil(radius/cell_size_z)+offset_why_needed;

    for (int i = std::max(i_mid-n_half_x, 0);i<std::min(i_mid+n_half_x, mNx);i++)
    {
        for (int j = std::max(j_mid-n_half_y,0);j<std::min(j_mid+n_half_y, mNy);j++)
        {
            for (int k = std::max(k_mid-n_half_z,0);k<std::min(k_mid+n_half_z, mNz);k++)
            {
                // should check aabb sphere intersection here...
                int I = localToGlobal(i,j,k);
                for (const auto &pt : mGridToPointMap[I])
                {
                    if (vmath::lengthSqr(mPoints[pt]-center)<radius*radius)
                    {
                        bool early_return = func(pt);
                        if (early_return) return;
                    }
                }
            }
        }
    }
}

inline bool SpaceHash3D::sphereCheckDelaunayGlobal(int tri_pt0, int tri_pt1, int tri_pt2) const
{
    // create the triangle circumsphere
    CircumDisk circum_disk = getCircumDisk(mPoints[tri_pt0], mPoints[tri_pt1], mPoints[tri_pt2]);
    const vmath::Vector3 &csphere_center = circum_disk.center;
    const float &csphere_radius = circum_disk.radius;

    // check if any points in circumsphere
    bool is_delaunay = true;
    forEachPointInSphere(csphere_center, csphere_radius, [&](const int &p) -> bool {
        // if another point is inside the circumsphere, then it is not a delaunay triangle
        bool early_return = false;
        if (p!=tri_pt0 && p!=tri_pt1 && p!=tri_pt2)
        {
            is_delaunay = false;
            early_return = true;
        }
        return early_return;
    });

    return is_delaunay;
}

#endif // SPACEHASH3D_H
