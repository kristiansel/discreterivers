#ifndef GFX_PRIMITIVES_H
#define GFX_PRIMITIVES_H

#define _VECTORMATH_DEBUG
#include "../dep/vecmath/vectormath_aos.h"

#include <vector>
#include <algorithm>
#include <iostream>

namespace vmath = Vectormath::Aos;

namespace gfx
{
    struct Triangle
    {
        int indices[3];//should this be padded?

    public:
        inline int& operator[] (const int index)
        {
            return indices[index];
        }

        inline const int& operator[] (const int index) const
        {
            return indices[index];
        }

        inline bool operator==(const Triangle &other) const
        {
            return indices[0]==other.indices[0] && indices[1]==other.indices[1] && indices[2]==other.indices[2];
        }
    };

    struct Line
    {
        int indices[2];

    public:
        inline int& operator[] (const int index)
        {
            return indices[index];
        }

        inline const int& operator[] (const int index) const
        {
            return indices[index];
        }
    };

    struct Point
    {
        int index;
    };

    template<class PrimitiveType>
    void generateNormals(std::vector<vmath::Vector4> * const normal_data,
                         const std::vector<vmath::Vector4> &position_data,
                         const std::vector<PrimitiveType> &primitive_data)
    {
        std::cout << "generating default normals " << std::endl;
        if (normal_data->size() < position_data.size())
        {
            normal_data->resize(position_data.size());
        }

        // vertex normal is average of surrounding triangle normals
        for (int i = 0; i<position_data.size(); i++)
        {
            (*normal_data)[i] = vmath::Vector4(vmath::normalize(position_data[i].getXYZ()), 0.0f);
        }
    }

    template<>
    void generateNormals<Triangle>(std::vector<vmath::Vector4> * const normals,
                         const std::vector<vmath::Vector4> &vertices,
                         const std::vector<Triangle> &triangles);

    // shameful: code not reused... Vector4 vs Vector3 problems
    void generateNormals(std::vector<vmath::Vector3> * const normals,
                         const std::vector<vmath::Vector3> &vertices,
                         const std::vector<Triangle> &triangles);
}

#endif // GFX_PRIMITIVES_H
