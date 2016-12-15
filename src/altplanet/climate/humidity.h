#ifndef HUMIDITY_H
#define HUMIDITY_H

#include <vector>

#include "../../common/gfx_primitives.h"
#include "../planetshapes.h"

namespace AltPlanet {

namespace Humidity
{

std::vector<float> humidityYearMean(const std::vector<vmath::Vector3> &points,
                                    const Shape::BaseShape &planet_shape);

}

}

#endif // HUMIDITY_H
