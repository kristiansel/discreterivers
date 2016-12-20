#ifndef GUITRANSFORM_H
#define GUITRANSFORM_H

#include <array>
#include "../../common/gfx_primitives.h"

namespace gfx {

namespace gui {

class GUITransform
{
public:
    using Position = std::array<float, 2>;
    using Size = std::array<float, 2>;

    GUITransform(Position &&pos, Size &&size) : mPos(std::move(pos)), mSize(std::move(size)) {}

    vmath::Matrix4 getTransformMatrix() const
    {
        return vmath::Matrix4::translation({2.0f*mPos[0]-1.0f, -2.0f*mPos[1]+1.0f, 0.0f}) * vmath::Matrix4::scale({mSize[0], mSize[1], 1.0f});
    }

    const Position &getPos() const { return mPos; }
    const Position &getSize() const { return mSize; }

    void setPos(Position &&pos) { mPos = std::move(pos); }
    void setSize(Size &&size) { mSize = std::move(size); }

private:
    GUITransform();

    Position mPos;
    Size mSize;
};

} // namespace gui

} // namespace gfx

#endif // GUITRANSFORM_H
