#ifndef IRRADIANCE_H
#define IRRADIANCE_H

#include <cmath>
#include <vector>
#include "../../common/gfx_primitives.h"

namespace AltPlanet {

namespace Irradiance {

const int N_SELF_ROTATION = 32;
const int N_SUN_ROTATION = 32;

/**
 * @brief irradianceYearMean: Calculate the year mean solar irradiance at each point on a planet
 * @param points: planet points
 * @param normals: planet normals
 * @param triangles: planet triangles
 * @param planet_tilt_rad: Planet tilt angle in radians
 * @return: a vector of floats representing intensity at each point. Has same size as points.
 */
std::vector<float> irradianceYearMean(const std::vector<vmath::Vector3> &points,
                                      const std::vector<vmath::Vector3> &normals,
                                      const std::vector<gfx::Triangle> &triangles,
                                      float planet_tilt_rad);

} // namespace Irradiance

} // namespace AltPlanet

#endif // IRRADIANCE_H
