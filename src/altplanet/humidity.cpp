#include "humidity.h"

#include "../common/procedural/noise3d.h"

namespace AltPlanet {

namespace Humidity {

std::vector<float> humidityYearMean(const std::vector<vmath::Vector3> &points,
                                    const AltPlanet::Shape &planet_shape)
{
    std::vector<float> out(points.size());

    Shape::AABB aabb = planet_shape.getAABB();
    float smallest_noise_scale = 0.2f; // TODO: meters....
    Noise3D noise3d(aabb.width, aabb.height, smallest_noise_scale, 198327);

    for (int i=0; i<points.size(); i++)
    {
        out[i] = noise3d.sample(points[i]);
    }

    return out;
}

}

}

