#ifndef PROJECTION_H
#define PROJECTION_H 

#define _VECTORMATH_DEBUG
#include "../../dep/vecmath/vectormath_aos.h"
#include <iostream>
#include <array>
#include <algorithm>

namespace vmath = Vectormath::Aos;

namespace Projection
{
	inline vmath::Vector3 linePlaneIntersection(vmath::Vector3 line_dir, vmath::Vector3 line_point,
										 vmath::Vector3 plane_normal, vmath::Vector3 plane_point)
	{
		float discr = vmath::dot(plane_normal, line_dir);
		assert((discr>0.0f));
		float d = vmath::dot(plane_point-line_point, plane_normal)/discr;
		return line_point + d*line_dir;
	}

	inline vmath::Vector3 projectPointIntoPlane(const vmath::Vector3 &point,
                                     const vmath::Vector3 &plane_normal,
                                     const vmath::Vector3 &plane_point)
	{
		return linePlaneIntersection(plane_normal, point, plane_normal, plane_point);
	}
	
	inline float pointIntoLine(	const vmath::Vector3 &point,
								const vmath::Vector3 &line_point,
								const vmath::Vector3 &line_dir)
	{
		return vmath::dot(point-line_point, line_dir) / vmath::dot(line_dir, line_dir); 
	}

	bool trianglesOverlap(	const std::array<vmath::Vector3, 3> &triangle1, 
							const std::array<vmath::Vector3, 3> &triangle2)
	{
		vmath::Vector3 normal1 = vmath::cross(triangle1[1]-triangle1[0], triangle1[2]-triangle1[0]);
		
		// project the points of one triangle onto the plane of the other
		std::array<vmath::Vector3, 3> tri2proj;
		for (int i = 0; i < 3; ++i) {
			tri2proj[i] = projectPointIntoPlane(triangle2[i], normal1, triangle1[0]);	
		}

		// find all the axes to be tested. Do all axes of both need to be tested?
		std::array<vmath::Vector3, 6> projAxes;

		for (int i = 0; i < 3; ++i) {
			projAxes[i] = vmath::cross(triangle1[i%3]-triangle1[i], normal1);
		}

		for (int i = 0; i < 3; ++i) {
			projAxes[i+3] = vmath::cross(tri2proj[i%3]-tri2proj[i], normal1);	
		}

		// project the points to each axis
		// check if they are separate
		// if there is one axis where they are separate then it is not a collision

		// otherwise it is

		return true;
	}

} /* Projection */ 

#endif /* ifndef PROJECTION_H */
