#ifndef EULER_H
#define EULER_H

#include <array>
#include <limits>

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
    vmath::Vector3 half_extents;
    vmath::Vector3 translation;
    vmath::Matrix3 rot_mat;
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

// wow this is some minkowski difference, GJK shit... :O
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

bool inline intersectSphereAABB(const Sphere &s, const AABB &ab)
{
    float r2 = s.radius * s.radius;
    float dmin = distanceVec3AABB(s.center, ab);
    return dmin <= r2;
}

// separating axis theorem implementation

// need algo to partition points on line
// static...

float inline minmax(float &min, float &max, const float * const p, unsigned int n)
{
    min = std::numeric_limits<float>::max();
    max = std::numeric_limits<float>::lowest();

    for (unsigned int i = 0; i<n; i++)
    {
        min = p[i] < min ? p[i] : min;
        max = p[i] > max ? p[i] : max;
    }
}

bool inline separate(const float* const p0, unsigned int n0,
                     const float* const p1, unsigned int n1) // how should the args to this go?
{
    // find max and min..
    float min0, max0, min1, max1;
    minmax(min0, max0, p0, n0);
    minmax(min1, max1, p1, n1);

    // are they separate?
    return (max1 < min0) || (max0 < min1);
}

// assume that p2 has allocated to n, use Vector3 in lack of Vector2... ugly as hell..
// assume that tan0, tan1 are normalized and orthogonal!
void inline project3to2(const vmath::Vector3 &basis0, // plane tangent 0, define plane through origin
                        const vmath::Vector3 &basis1, // plane tangent 1
                        vmath::Vector3 * const p2,
                        const vmath::Vector3 * const p3, std::size_t n)
{
    for (int i = 0; i<n; i++)
         p2[i] = vmath::Vector3(vmath::dot(basis0, p3[i]), vmath::dot(basis1, p3[i]), 0.0f);
}

float inline dot2(const vmath::Vector3 &a, const vmath::Vector3 &b) { return a[0]*b[0]+a[1]*b[1]; }

// assume that p2 has allocated to n, use Vector3 in lack of Vector2... ugly as hell..
// assume n2 is normalized
void inline project2to1(const vmath::Vector3 &basis0, // line direction, defined through origin
                        float * const p1,
                        const vmath::Vector3 * const p2, std::size_t n)
{
    for (int i = 0; i<n; i++)
        p1[i] = dot2(basis0, p2[i]);
}

//void intersectConvexTriMeshes()



// dynamic...

// too hard..
/*

inline void getOBBTransformedPts(const OBB &ob, std::array<vmath::Vector3, 8> &pts)
{
    for (int i = 0; i<2; i++)
        for (int j = 0; j<2; j++)
            for (int k = 0; k<2; k++)
                pts[i+2*j+4*k] = ob.translation +
                        ob.rot_mat * vmath::Vector3((2*i-1)*ob.half_extents[0],
                                                    (2*j-1)*ob.half_extents[1],
                                                    (2*k-1)*ob.half_extents[2]);
}

bool inline intersectAABBvOBB(const AABB &ab, const OBB &ob)
{
    std::array<vmath::Vector3, 8> ob_pts;
    getOBBTransformedPts(ob, ob_pts);
    for (int i=0; i<3; i++)
    {
        for (int j = 0; j<8; j++)
        {
            bool inside = ob_pts[j][i] > ab.upper[i];
        }
    }
}*/

} // basicshape

namespace Dynamics {

class RigidBody
{
// physical properties
    float inverse_mass;

// first order...
    vmath::Vector3 position;
    vmath::Quat rotation;

// second order...
    vmath::Vector3 velocity;

// third order...
    vmath::Vector3 force;

    RigidBody() = delete;
public:
    RigidBody(const vmath::Vector3 &pos, const vmath::Quat &rot, float mass) :
        position(pos), rotation(rot), inverse_mass(1.0f/mass), velocity(0.0f), force(0.0f) {}

    void setVelocity(const vmath::Vector3 &vel) { velocity = vel; }
    void applyForce(const vmath::Vector3 &f) { force = force + f; }
    void clearForce() { force = vmath::Vector3(0.0f, 0.0f, 0.0f); }

    void integrateRigidBody(float time_step)
    {
        // forward euler for the win...
        velocity = velocity + time_step * force * inverse_mass;
        position = position + time_step * velocity;
    }
};




}


}

#endif // EULER_H
