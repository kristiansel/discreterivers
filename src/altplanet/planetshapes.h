#ifndef PLANETSHAPES_H
#define PLANETSHAPES_H

#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"

namespace vmath = Vectormath::Aos;

namespace AltPlanet
{

namespace Shape
{

struct AABB {float width; float height;};

class BaseShape
{
public:
    virtual vmath::Vector3 projectPoint(const vmath::Vector3 &point) const = 0;
    virtual AABB getAABB() const = 0;
};

class Disk : public BaseShape
{
public:
    Disk(float radius) : radius(radius) {}
    vmath::Vector3 projectPoint(const vmath::Vector3 &point) const
    {
        return {point[0], 0.0f, point[2]}; // simply remove the y-component
    }

    AABB getAABB() const
    {
        return {2.0f*radius, 0.1f}; // height is zero really...
    }

private:
    float radius;
};

class Sphere : public BaseShape
{
public:
    Sphere(float radius) : radius(radius) {}
    vmath::Vector3 projectPoint(const vmath::Vector3 &point) const
    {
        return radius*vmath::normalize(point); // simply remove the y-component
    }

    AABB getAABB() const
    {
        return {2.0f*radius, 2.0f*radius};
    }

private:
    float radius;
};

class Torus : public BaseShape
{
public:
    Torus(float major_radius, float minor_radius) : major_radius(major_radius), minor_radius(minor_radius) {}
    vmath::Vector3 projectPoint(const vmath::Vector3 &point) const
    {
        vmath::Vector3 circle_point = major_radius*vmath::normalize({point[0], 0.0f, point[2]});
        return circle_point + minor_radius*vmath::normalize(point-circle_point);
    }

    AABB getAABB() const
    {
        return {(2.0f*major_radius+minor_radius), 2.0f*minor_radius};
    }

private:
    float major_radius;
    float minor_radius;

};

} // namespace Shape

} // namespace AltPlanet

#endif // PLANETSHAPES_H
