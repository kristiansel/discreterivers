#include "barycentric.h"

namespace MultiCalculus {


vmath::Vector3 linePlaneIntersection(vmath::Vector3 line_dir, vmath::Vector3 line_point,
                                     vmath::Vector3 plane_normal, vmath::Vector3 plane_point)
{
    // from wikipedia
    float d = vmath::dot(plane_point-line_point, plane_normal)/vmath::dot(plane_normal, line_dir);
    return line_point + d*line_dir;
}

vmath::Vector3 linePlaneIntersectionNormalized(vmath::Vector3 line_dir_normalized, vmath::Vector3 line_point,
                                               vmath::Vector3 plane_normal_normalized, vmath::Vector3 plane_point)
{
    assert(abs(vmath::length(line_dir_normalized)-1.f)<0.0001);
    assert(abs(vmath::length(plane_normal_normalized)-1.f)<0.0001);

    float d = vmath::dot(plane_point-line_point, plane_normal_normalized);
    return line_point + d*line_dir_normalized;
}

inline vmath::Vector3 projectPointIntoPlane(vmath::Vector3 point,
                                            vmath::Vector3 plane_normal,
                                            vmath::Vector3 plane_point)
{
    return linePlaneIntersection(plane_normal, point, plane_normal, plane_point);
}

//can't inline?
vmath::Vector3 barycentricCoords(const vmath::Vector3 &pt,
                                        const vmath::Vector3 &tp0,
                                        const vmath::Vector3 &tp1,
                                        const vmath::Vector3 &tp2)
{
    // This method is an optimization candidate

    //find the total triangle area ( = half the length of cross product of two side vectors)
    vmath::Vector3 vt01 = tp1 - tp0;
    vmath::Vector3 vt02 = tp2 - tp0;
    vmath::Vector3 cross_vt = vmath::cross(vt01, vt02);
    float ta = vmath::length(cross_vt)/2.0f; // can't be zero!

    // project the point onto the triangle
    vmath::Vector3 normal = vmath::normalize(cross_vt);
    vmath::Vector3 p = projectPointIntoPlane(pt, normal, tp0);

    // find vectors from triangle corners to point
    vmath::Vector3 vpt0 = tp0 - p;
    vmath::Vector3 vpt1 = tp1 - p;
    vmath::Vector3 vpt2 = tp2 - p;

    // find the areas of the triangles between edges and the point
    float ap01 = vmath::length(vmath::cross(vpt0, vpt1))/2.0f;
    float ap02 = vmath::length(vmath::cross(vpt0, vpt2))/2.0f;
    float ap12 = vmath::length(vmath::cross(vpt1, vpt2))/2.0f;

    // barycentric coordinates ( = area opposing corner/total triangle area)
    float b0 = ap12/ta;
    float b1 = ap02/ta;
    float b2 = ap01/ta;

    return vmath::Vector3(b0, b1, b2);
}

int multinomialCoefficient2(int i, int j, int k)
{
    int n = 2;

    assert(( (i+j+k==n) && (n>0) ));

    return std::max(i,std::max(j,k))==2 ? 1 : 2;

/* general algo for n not necessarily two:

//     factor1 = sum(log(1:n));
    float factor1 = 0;
    for (int i_n = 0; i_n<n; i_n++)
    {
        factor1 += log((float)(i_n) + 1.0f); // natural log
    }

//     c = length(x);

//    f = [];
//    for i = 1:c,
//        f = [f log(1:x(i))];
//    end;

//    factor2 = sum(f);

    float factor2 = std::max(i,std::max(j,k))==2 ? 0.69315f : 0.0f; // wtf?

//    y = exp(factor1-factor2);
//    y = round(y);

    float yf = exp(factor1-factor2);
    int y = yf+0.5f;

    return y;

*/
}

inline float bernesteinBarycentric(vmath::Vector3 b, int i, int j, int k)
{

    return (float)(multinomialCoefficient2(i, j, k)) * pow(b[0], i) * pow(b[1], j) * pow(b[2], k);
}

vmath::Vector3 findPointInTriangle(const vmath::Vector3 &point,
                                   const gfx::Triangle &triangle,
                                   const std::vector<vmath::Vector3> &points,
                                   const std::vector<vmath::Vector3> &point_normals)
{
    // project all points to unit sphere
    vmath::Vector3 pt = vmath::normalize(point);
    vmath::Vector3 tp0 = vmath::normalize(points[triangle[0]]);
    vmath::Vector3 tp1 = vmath::normalize(points[triangle[1]]);
    vmath::Vector3 tp2 = vmath::normalize(points[triangle[2]]);

    // find barycentric coordinates
    vmath::Vector3 b = barycentricCoords(pt, tp0, tp1, tp2);

    assert((b[0]+b[1]+b[2]<=1.0f));

    // evaluate bernstein basis at barycentric coords (6 polys)
    float bb200 = bernesteinBarycentric(b, 2,0,0); // bary, n = 2, i, j, k -> n is degree of polynomial
    float bb020 = bernesteinBarycentric(b, 0,2,0);
    float bb002 = bernesteinBarycentric(b, 0,0,2);

    float bb110 = bernesteinBarycentric(b, 1,1,0);
    float bb101 = bernesteinBarycentric(b, 1,0,1);
    float bb011 = bernesteinBarycentric(b, 0,1,1);

    // find the... fuark need edge normals for 3 edge planes
    // take the edge normals to be avg of point normals for now...
    vmath::Vector3 n01 = 0.5f*(point_normals[triangle[0]]+point_normals[triangle[1]]);
    vmath::Vector3 n02 = 0.5f*(point_normals[triangle[0]]+point_normals[triangle[2]]);
    vmath::Vector3 n12 = 0.5f*(point_normals[triangle[1]]+point_normals[triangle[2]]);

    // find the plane intersections
    vmath::Vector3 origin = vmath::Vector3(0.f, 0.f, 0.f);
    float z_n0 = vmath::length(linePlaneIntersection(pt, origin, point_normals[triangle[0]], points[triangle[0]]));
    float z_n1 = vmath::length(linePlaneIntersection(pt, origin, point_normals[triangle[1]], points[triangle[1]]));
    float z_n2 = vmath::length(linePlaneIntersection(pt, origin, point_normals[triangle[2]], points[triangle[2]]));

    float z_n01 = vmath::length(linePlaneIntersection(pt, origin, n01, 0.5f*(points[triangle[0]]+points[triangle[1]])));
    float z_n02 = vmath::length(linePlaneIntersection(pt, origin, n02, 0.5f*(points[triangle[0]]+points[triangle[2]])));
    float z_n12 = vmath::length(linePlaneIntersection(pt, origin, n12, 0.5f*(points[triangle[1]]+points[triangle[2]])));

    float z = bb200*z_n0 + bb020*z_n1 + bb002*z_n2 + bb110*z_n01 + bb101*z_n02 + bb011*z_n12;

    return z*pt;
}

}
