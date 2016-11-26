#include "irradiance.h"

namespace AltPlanet {

namespace Irradiance {

// function for determining whether a point is visible from the sun (need to account for self shadowing)

// function for calculating irradiance given normal and direction to sun
inline float calcIrradiance(const vmath::Vector3 &normal, const vmath::Vector3 sun_dir)
{
    return std::max(0.0f, static_cast<float>(vmath::dot(normal, sun_dir)));
}

std::vector<float> irradianceYearMean(const std::vector<vmath::Vector3> &points,
                                      const std::vector<vmath::Vector3> &normals,
                                      const std::vector<gfx::Triangle> &triangles,
                                      float planet_tilt_rad)
{
    assert(points.size()>0);
    assert(triangles.size()>0);
    // also triangles should only have indices in the valid range of the points vector size
    // planet tilt can be anything as radians wrap nicely both in positive and negative direction

    // calculate quaternion corresponding to tilt rotation (around x-axis)
    vmath::Quat tilt_rotation = vmath::Quat::rotation(planet_tilt_rad, vmath::Vector3(1.0, 0.0, 0.0));

    std::vector<float> irradiance(points.size(), 0.0f);

    for (int i_sun = 0; i_sun<N_SUN_ROTATION; i_sun++)
    {
        float sun_angle = static_cast<float>(i_sun)/(static_cast<float>(N_SUN_ROTATION))*2.0f*M_PI;

        // calculate direction to sun
        vmath::Quat sun_rotation = vmath::Quat::rotation(sun_angle, vmath::Vector3(0.0, 1.0, 0.0));
        vmath::Matrix3 sun_rotation_matrix = vmath::Matrix3(sun_rotation);
        vmath::Vector3 sun_direction = sun_rotation_matrix * vmath::Vector3(1.0, 0.0, 0.0);


        for (int i_self = 0; i_self<N_SELF_ROTATION; i_self++)
        {
            float self_angle = static_cast<float>(i_self)/(static_cast<float>(N_SELF_ROTATION))*2.0f*M_PI;

            // with tilt and self rotation, calculate normal rotation matrix
            vmath::Quat self_rotation = vmath::Quat::rotation(self_angle, vmath::Vector3(0.0, 1.0, 0.0));
            vmath::Matrix3 normal_rotation_matrix = vmath::Matrix3(tilt_rotation * self_rotation);

            for (int i_point = 0; i_point <points.size(); i_point ++)
            {
                vmath::Vector3 normal_rotated = normal_rotation_matrix * normals[i_point];
                irradiance[i_point] += calcIrradiance(normal_rotated, sun_direction);
            }
        }
    }

    //      normalize the irradiance number somehow... (between one and zero for example?)
    float norm_factor = 1.0f/static_cast<float>(N_SUN_ROTATION*N_SELF_ROTATION);
    for (float &irr : irradiance) irr *= norm_factor;

    return irradiance;
}

}

}
