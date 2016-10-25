#ifndef PROJECTION_H
#define PROJECTION_H 

#define _VECTORMATH_DEBUG
#include "../../dep/vecmath/vectormath_aos.h"
#include <iostream>

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


} /* Projection */ 

#endif /* ifndef PROJECTION_H */
