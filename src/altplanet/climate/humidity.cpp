#include "humidity.h"

#include "../../common/procedural/noise3d.h"
#include "../../common/mathext.h"

namespace AltPlanet {

namespace Humidity {

std::vector<float> humidityYearMean(const std::vector<vmath::Vector3> &points,
                                    const Shape::BaseShape &planet_shape)
{
    std::vector<float> out(points.size());

    Shape::AABB aabb = planet_shape.getAABB();
    float smallest_noise_scale = 1.0f; // TODO: meters....
    Noise3D noise3d(aabb.width*1.1f, aabb.height*1.1f, smallest_noise_scale, 58234);

    for (int i=0; i<out.size(); i++)
    {
        out[i] = noise3d.sample(points[i]);
    }

    // normalize values to between 0 and 1
    MathExt::normalizeFloatVec(out);

    return out;
}

}

}

