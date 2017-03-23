#ifndef EULER_H
#define EULER_H

#include <array>

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
