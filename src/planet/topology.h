#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "../common/gfx_primitives.h"

namespace vmath = Vectormath::Aos;

namespace Topology
{

typedef int point_index;

typedef std::vector<point_index> ConnectionList;


// should probably move this elsewhere
void createCubeGeometry(std::vector<vmath::Vector3> * const points,
                        std::vector<gfx::Triangle> * const triangles,
                        float half_side);

// should probably move this elsewhere
void createIcosahedronGeometry(std::vector<vmath::Vector3> * const points,
                               std::vector<gfx::Triangle> * const triangles,
                               const float radius);

// should probably move this elsewhere
void createIcoSphereGeometry(std::vector<vmath::Vector3> * const points,
                             std::vector<vmath::Vector3> * const normals,
                             std::vector<gfx::Triangle> * const triangles,
                             std::vector<std::vector<gfx::Triangle>> * const subd_triangles,
                             const float radius,
                             const int num_subdivisions);


void createAdjacencyList(std::vector<ConnectionList> * const edges,
                     const std::vector<vmath::Vector3> &points,
                     const std::vector<gfx::Triangle> &triangles);


namespace Test {

void barycentricCoords();

void multinomialCoefficient();

} // namespace Test

} // namespace Topology

#endif // TOPOLOGY_H
