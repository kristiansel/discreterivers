#ifndef EULER_H
#define EULER_H

#define _VECTORMATH_DEBUG
#include "../../dep/vecmath/vectormath_aos.h"
namespace vmath = Vectormath::Aos;

namespace Euler {

void testAll();

namespace BasicShape {

struct Point
{
    vmath::Vector3 coords;
};

struct Line
{
    vmath::Vector3 start;
    vmath::Vector3 dir;
};

struct Plane
{
    vmath::Vector3 normal;
    float plane_constant;
};

struct Sphere
{
    vmath::Vector3 center;
    float radius;
};

struct AABB
{
    vmath::Vector3 lower;
    vmath::Vector3 upper;
};

struct OBB
{
    vmath::Vector3 lower;
    vmath::Vector3 upper;
    vmath::Quat orientation;
};

// save degenerate cases for later
// point point, point line, point plane, line line

// line plane always intersect unless line is parallell to plane etc...

// need "contact manifolds?"

float inline sqr(float a) { return a*a; }

bool inline intersectPointSphere(const Point &p, const Sphere &s)
{
    float radius_sum = s.radius;
    vmath::Vector3 point_offset = s.center - p.coords;
    return vmath::lengthSqr(point_offset) < radius_sum * radius_sum;
}

bool inline intersectSphereSphere(const Sphere &s0, const Sphere &s1)
{
    float radius_sum = s0.radius + s1.radius;
    vmath::Vector3 centers_offset = s0.center - s1.center;
    return vmath::lengthSqr(centers_offset) < radius_sum * radius_sum;
}

bool inline intersectPointAABB(const Point &p, const AABB &ab)
{
    for (int i=0; i<3; i++)
    {
        bool in = p.coords[i]>ab.lower[i] && p.coords[i]<ab.upper[i];
        if (!in) return false;
    }
    return true;
}

bool inline intersectAABBvAABB(const AABB &ab0, const AABB &ab1)
{
    for (int i=0; i<3; i++) // hope this unwinds or somehow simds...
    {
        // if they are on separate sides... it is ok
        bool all_over  = ab0.upper[i] > ab1.upper[i] && ab0.lower[i] > ab1.upper[i];
        bool all_under = ab0.lower[i] < ab1.lower[i] && ab0.upper[i] < ab1.lower[i];
        if (all_over || all_under) return false;
    }
    return true;
}

float inline distanceVec3AABB(const vmath::Vector3 &p, const AABB &ab)
{
    float dmin = 0.0f;
    for( int i = 0; i < 3; i++ )
    {
        if ( p[i] < ab.lower[i] ) dmin += sqr( p[i] - ab.lower[i] );
        else if ( p[i] > ab.upper[i] ) dmin += sqr( p[i] - ab.upper[i] );
    }
    return dmin;
}

// the below does not work for box corner vs sphere...
bool inline intersectSphereAABB(const Sphere &s, const AABB &ab)
{
    float r2 = s.radius * s.radius;
    float dmin = distanceVec3AABB(s.center, ab);
    return dmin <= r2;
}

bool inline intersectAABBvOBB(const AABB &ab, const OBB &ob)
{
    // ai-ai... needs some though (or some projection...)
}



}

}

#endif // EULER_H
