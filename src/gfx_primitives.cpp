#include "gfx_primitives.h"

namespace gfx
{

template<>
void generateNormals<Triangle>(std::vector<vmath::Vector4> * const normals,
                     const std::vector<vmath::Vector4> &vertices,
                     const std::vector<Triangle> &triangles)
{
    std::cout << "generating triangle normals " << std::endl;
    if (normals->size() < vertices.size())
    {
        normals->resize(vertices.size());
    }

    // create vertex to triangle adjacency
    std::vector<std::vector<int>> vertices_triangles_adjacency(vertices.size());
    for (int i = 0; i<triangles.size(); i++)
    {
        for (int j = 0; j<3; j++)
        {
            auto &tri_adj_list = vertices_triangles_adjacency[triangles[i][j]];
            auto it = std::find(tri_adj_list.begin(), tri_adj_list.end(), i);
            if (it==tri_adj_list.end()) tri_adj_list.push_back(i);
        }
    }

    // create per triangle normals
    std::vector<vmath::Vector3> triangle_normals(triangles.size());
    for (int i = 0; i<triangles.size(); i++)
    {
        auto v1 = vertices[triangles[i][1]].getXYZ()-vertices[triangles[i][0]].getXYZ();
        auto v2 = vertices[triangles[i][2]].getXYZ()-vertices[triangles[i][0]].getXYZ();
        triangle_normals[i] = vmath::normalize(vmath::cross(v1, v2));

        // make sure they go outwards (argh... should really sort the triangle indices)
        // the following code assumes a spherical shape. Wait they are sorted! following line not necessary
        // if (vmath::dot(triangle_normals[i], vertices[triangles[i][0]])<0) triangle_normals[i]=-triangle_normals[i];
    }

    // vertex normal is average of surrounding triangle normals
    for (int i = 0; i<vertices.size(); i++)
    {
        auto &tri_adj_list = vertices_triangles_adjacency[i];
        auto n_tri_sum = vmath::Vector3(0.0f);
        for (int j = 0; j<tri_adj_list.size(); j++)
        {
            n_tri_sum += triangle_normals[tri_adj_list[j]];
        }
        (*normals)[i] = vmath::Vector4(vmath::normalize(n_tri_sum), 0.0f);
    }
}

void generateNormals(std::vector<vmath::Vector3> * const normals,
                     const std::vector<vmath::Vector3> &vertices,
                     const std::vector<Triangle> &triangles)
{
    //std::cout << "generating triangle normals " << std::endl;
    if (normals->size() < vertices.size())
    {
        normals->resize(vertices.size());
    }

    // create vertex to triangle adjacency
    std::vector<std::vector<int>> vertices_triangles_adjacency(vertices.size());
    for (int i = 0; i<triangles.size(); i++)
    {
        for (int j = 0; j<3; j++)
        {
            auto &tri_adj_list = vertices_triangles_adjacency[triangles[i][j]];
            auto it = std::find(tri_adj_list.begin(), tri_adj_list.end(), i);
            if (it==tri_adj_list.end()) tri_adj_list.push_back(i);
        }
    }

    // create per triangle normals
    std::vector<vmath::Vector3> triangle_normals(triangles.size());
    for (int i = 0; i<triangles.size(); i++)
    {
        auto v1 = vertices[triangles[i][1]]-vertices[triangles[i][0]];
        auto v2 = vertices[triangles[i][2]]-vertices[triangles[i][0]];
        triangle_normals[i] = vmath::normalize(vmath::cross(v1, v2));

        // make sure they go outwards (argh... should really sort the triangle indices)
        // the following code assumes a spherical shape. Wait they are sorted! following line not necessary
        // if (vmath::dot(triangle_normals[i], vertices[triangles[i][0]])<0) triangle_normals[i]=-triangle_normals[i];
    }

    // vertex normal is average of surrounding triangle normals
    for (int i = 0; i<vertices.size(); i++)
    {
        auto &tri_adj_list = vertices_triangles_adjacency[i];
        auto n_tri_sum = vmath::Vector3(0.0f);
        for (int j = 0; j<tri_adj_list.size(); j++)
        {
            n_tri_sum += triangle_normals[tri_adj_list[j]];
        }
        (*normals)[i] = vmath::normalize(n_tri_sum);
    }
}

} // namespace gfx
