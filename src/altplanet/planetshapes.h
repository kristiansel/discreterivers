#ifndef PLANETSHAPES_H
#define PLANETSHAPES_H

#include <cmath>

#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"

#include "../common/gfx_primitives.h"

namespace vmath = Vectormath::Aos;

namespace AltPlanet
{

namespace Shape
{

struct AABB {float width; float height;};

class BaseShape
{
public:
    virtual ~BaseShape() {}

    /**
     * @brief shapeFunction: Override this function to define planet shape.
     * @param point: Input point to transform to planet shape surface
     * @param projected_point: Output transformed point on shape surface
     * @return: The "Height" equivalent of the implemented shape. For example:
     *         for a sphere this would be the distance to the center
     */
    //virtual float shapeFunction(const vmath::Vector3 &point, vmath::Vector3 &projected_point) const = 0;

    struct SurfaceLine
    {
        vmath::Vector3 zeroHeightPt;
        vmath::Vector3 surfNormal;
        float refHeight;

        vmath::Vector3 evalRefHeight() const { return zeroHeightPt + surfNormal * refHeight; }
        vmath::Vector3 evalHeight(float height) const { return zeroHeightPt + surfNormal * height; }
    };

    virtual SurfaceLine shapeFunction(const vmath::Vector3 &point) const = 0;


    /**
     * @brief getAABB: Axis-aligned bounding box
     * @return: The AABB struct contains the width and height of the
     *          (origin-centered) axis-aligned bounding box for the shape
     */
    virtual AABB getAABB() const = 0;

    /**
     * @brief getAABB: Get the direction of the gradient to the surface
     *          at an arbitrary point.
     * @return: The direction of the gradient is not normalized.
     */
    virtual vmath::Vector3 getGradDir(const vmath::Vector3 &point) const = 0;

    /**
     * @brief getEulerCharacteristic: This function is used in the triangulation
     *          step to verify that the triangulation has the same topology as the
     *          desired shape
     * @return: The euler characteristic of the shape
     */
    //virtual int getEulerCharacteristic() const = 0;

    virtual std::vector<gfx::TexCoords> getUV(const std::vector<vmath::Vector3> &points) const = 0;

    //////////////////////////////////////////////////
    // Functions that utilize the virtual functions //
    //////////////////////////////////////////////////
    inline vmath::Vector3 projectPoint(const vmath::Vector3 &point) const
    {
        SurfaceLine surface_line = shapeFunction(point);
        return surface_line.evalRefHeight();
    }

    inline void scalePointHeight(vmath::Vector3 &point, float scale_factor) const
    {
        SurfaceLine surface_line = shapeFunction(point);
        float newHeight = surface_line.refHeight*scale_factor;
        point = surface_line.evalHeight(newHeight);
    }

    inline void setHeight(vmath::Vector3 &point, float new_height) const
    {
        SurfaceLine surface_line = shapeFunction(point);
        point = surface_line.evalHeight(new_height);
    }

    inline float getHeight(const vmath::Vector3 &point) const
    {
        SurfaceLine surface_line = shapeFunction(point);
        return (vmath::length(point - surface_line.zeroHeightPt));
    }

    inline float getHeightSqr(const vmath::Vector3 &point) const
    {
        SurfaceLine surface_line = shapeFunction(point);
        return (vmath::lengthSqr(point - surface_line.zeroHeightPt));
    }
};

/* // need shape to be 3d, reimagine the disk as a squashed sphere (with two sides, exciting)
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
};*/


class Sphere : public BaseShape
{
public:
    Sphere(float radius) : radius(radius) {}

    SurfaceLine shapeFunction(const vmath::Vector3 &point) const
    {
        return {vmath::Vector3(0.f, 0.f, 0.f), vmath::normalize(point), radius};
    }

    AABB getAABB() const
    {
        return {2.0f*radius, 2.0f*radius};
    }

    vmath::Vector3 getGradDir(const vmath::Vector3 &point) const
    {
        return point;
    }

    std::vector<gfx::TexCoords> getUV(const std::vector<vmath::Vector3> &points) const
    {
        std::vector<gfx::TexCoords> texco_out(points.size());

        for(int i = 0; i<texco_out.size(); i++)
        {
            auto d = vmath::normalize(points[i]);
            float u = 0.5+atan2(d[2], d[0])/(2.0f*M_PI);
            float v = 0.5-asin(d[1])/M_PI;
            texco_out[i] = {u, v};
        }
        return texco_out;
    }

private:
    float radius;
};

class Torus : public BaseShape
{
public:
    Torus(float major_radius, float minor_radius) : major_radius(major_radius), minor_radius(minor_radius) {}

    SurfaceLine shapeFunction(const vmath::Vector3 &point) const
    {
        vmath::Vector3 circle_point = major_radius*vmath::normalize({point[0], 0.0f, point[2]});
        vmath::Vector3 pt_diff = point - circle_point;

        return {circle_point, vmath::normalize(pt_diff), minor_radius};
    }

    AABB getAABB() const
    {
        return {(2.0f*major_radius+2.0f*minor_radius), 2.0f*minor_radius};
    }

    vmath::Vector3 getGradDir(const vmath::Vector3 &point) const
    {
        vmath::Vector3 circle_point = major_radius*vmath::normalize({point[0], 0.0f, point[2]});
        return point-circle_point;
    }

    std::vector<gfx::TexCoords> getUV(const std::vector<vmath::Vector3> &points) const
    {
        std::vector<gfx::TexCoords> texco_out(points.size());

        for(int i = 0; i<texco_out.size(); i++)
        {
            /*const vmath::Vector3 &point = points[i];
            vmath::Vector3 circle_point = major_radius*vmath::normalize({point[0], 0.0f, point[2]});
            vmath::Vector3 circle_to_point = point-circle_point;
            float minor_angle = getOrientedAngle(circle_point, circle_to_point, circle_tangent);
            flat major_angle = getOrientedAngle(circle_point, vmath::Vector3(1.0f, 0.0f, 0.0f), vmath::Vector3(0.0f, 1.0f, 0.0f));*/

            texco_out[i] = {0, 0};
        }
        return texco_out;
    }

private:
    float major_radius;
    float minor_radius;

};

} // namespace Shape

} // namespace AltPlanet

#endif // PLANETSHAPES_H
