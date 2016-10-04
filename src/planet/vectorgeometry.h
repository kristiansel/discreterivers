#ifndef MULTICALCULUS
#define MULTICALCULUS_H

#include "../common/gfx_primitives.h"

// should make this header only...

namespace vmath = Vectormath::Aos;

namespace VectorGeometry
{
vmath::Vector3 linePlaneIntersection(vmath::Vector3 line_dir, vmath::Vector3 line_point,
                                     vmath::Vector3 plane_normal, vmath::Vector3 plane_point);

vmath::Vector3 baryPointInTriangle(const vmath::Vector3 &p,
                                   const vmath::Vector3 &tp0,
                                   const vmath::Vector3 &tp1,
                                   const vmath::Vector3 &tp2);


vmath::Vector3 baryPointInTriangle(const vmath::Vector3 &p,
                                   const vmath::Vector3 &tp0,
                                   const vmath::Vector3 &tp1,
                                   const vmath::Vector3 &tp2,
                                   const vmath::Vector3 &tri_cross);


vmath::Vector3 triangleNormal(const vmath::Vector3 &tp0,
                             const vmath::Vector3 &tp1,
                             const vmath::Vector3 &tp2);

vmath::Vector3 triangleCross(const vmath::Vector3 &tp0,
                             const vmath::Vector3 &tp1,
                             const vmath::Vector3 &tp2);

vmath::Vector3 triangleUnitNormal(const vmath::Vector3 &tp0,
                             const vmath::Vector3 &tp1,
                             const vmath::Vector3 &tp2);

//vmath::Vector3 barycentricCoords(const vmath::Vector3 &pt,
//                                const vmath::Vector3 &tp0,
//                                const vmath::Vector3 &tp1,
//                                const vmath::Vector3 &tp2);

bool pointInTriangle(const vmath::Vector3 &barycentric_coords);

int multinomialCoefficient2(int i, int j, int k);

vmath::Vector3 findPointInTriangle(const vmath::Vector3 &point,
                                   const gfx::Triangle &triangle,
                                   const vmath::Vector3 &barycentric_coords,
                                   const std::vector<vmath::Vector3> &points,
                                   const std::vector<vmath::Vector3> &point_normals);

}

#endif // MULTICALCULUS_H
