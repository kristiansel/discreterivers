#ifndef VERTEXCOLORMATERIAL_H
#define VERTEXCOLORMATERIAL_H

#include <vector>
#include "gfxcommon.h"
#include "../common/gfx_primitives.h"

namespace gfx {

class VertexColorMaterial {
public:
    explicit VertexColorMaterial(const std::vector<float> &vertex_colors);
    explicit VertexColorMaterial(const std::vector<vmath::Vector4> &vertex_colors);

private:
    std::vector<vmath::Vector4> mVertexColors;
};

}

#endif // VERTEXCOLORMATERIAL_H
