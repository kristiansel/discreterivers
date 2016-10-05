#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "../common/gfx_primitives.h"
#include "../common/typetag.h"

namespace vmath = Vectormath::Aos;

// old non-type-safe "int" tags
typedef int point_index;

// typesafe tag type tags
struct triangle_tag{};
typedef ID<triangle_tag, int, -1> tri_index;

// typedefs
typedef std::vector<point_index> ConnectionList;

namespace Topology
{

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
                             const int num_subdivisions,
                             const float terrain_roughness);


void createAdjacencyList(std::vector<ConnectionList> * const edges,
                     const std::vector<vmath::Vector3> &points,
                     const std::vector<gfx::Triangle> &triangles);

void createPointToTriAdjacency(std::vector<std::vector<tri_index>> * point_tri_adjacency,
                              const std::vector<gfx::Triangle> &triangles,
                              const std::vector<vmath::Vector3> &points);

void createTriToTriAdjacency(std::vector<std::vector<tri_index>> * tri_tri_adjacency,
                             const std::vector<gfx::Triangle> &triangles,
                             const std::vector<vmath::Vector3> &points,
                             const std::vector<std::vector<tri_index>> &point_tri_adjacency);

void createTriToTriAdjacency(std::vector<std::vector<tri_index>> * tri_tri_adjacency,
                              const std::vector<gfx::Triangle> &triangles,
                              const std::vector<vmath::Vector3> &points);


namespace Test {

void barycentricCoords();

void multinomialCoefficient();

} // namespace Test

} // namespace Topology

#endif // TOPOLOGY_H
