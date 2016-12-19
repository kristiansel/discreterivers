#ifndef GUITRANSFORM_H
#define GUITRANSFORM_H

#include <array>

namespace gfx {

namespace gui {

class GUITransform
{
public:
    using Position = std::array<float, 2>;
    using Size = std::array<float, 2>;

    GUITransform(Position &&pos, Size &&size) : mPos(std::move(pos)), mSize(std::move(pos)) {}

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
