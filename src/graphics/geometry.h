#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "../common/gfx_primitives.h"
#include "primitives.h"
#include "vertices.h"

namespace gfx {

class Geometry
{
public:
    explicit Geometry(const Vertices &vertices, const Primitives &primitives) :
        mVertices(vertices), mPrimitives(primitives) {}

    template<class PrimitiveType>
    explicit Geometry(const std::vector<vmath::Vector4> &position_data,
             const std::vector<vmath::Vector4> &normal_data,
             const std::vector<PrimitiveType> &primitive_data) :
        mVertices(Vertices(position_data, normal_data)), mPrimitives(primitive_data) {}

    // so that they cannot be reassigned after creation
    inline const Vertices& getVertices() const {return mVertices;}
    inline const Primitives& getPrimitives() const {return mPrimitives;}

    struct DrawData {
        struct Vertices {
            GLuint mVertexArrayObject;
        } vertices;

        struct Primitives {
            GLuint mElementArrayBuffer; // Pointer type, Primitives is not a POD
            GLsizeiptr mNumIndices;
            gl_primitive_type mPrimitiveType;
        } primitives;

    };

    DrawData getDrawData() const
    {
        return {
            {
                mVertices.getVertexArrayObject(),
            },
            {
                mPrimitives.getElementArrayBuffer(),
                mPrimitives.getNumIndices(),
                mPrimitives.getPrimitiveType()
            }
        };
    }

private:
    Geometry();

    Vertices mVertices;
    Primitives mPrimitives;
};

/*
template<class PrimitiveType>
Geometry::Geometry(const std::vector<vmath::Vector4> &position_data,
                   const std::vector<vmath::Vector4> &normal_data,
                   const std::vector<PrimitiveType> &primitive_data) :
    mVertices(Vertices(position_data, normal_data)), mPrimitives(primitive_data) {}

*/

}


#endif // GEOMETRY_H
