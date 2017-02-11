#ifndef GUITRANSFORM_H
#define GUITRANSFORM_H

#include <array>
#include "../../common/gfx_primitives.h"

namespace gfx {

namespace gui {

// Define some types
enum class Units { Percentage/*, StdPixel */};
enum class HorzAnchor : int { Left, Middle, Right };
enum class VertAnchor : int { Top, Middle, Bottom };

template <typename Anchor>
struct PosSpec
{
    // implicit constructor
    PosSpec(float v, Units u = Units::Percentage, Anchor a = Anchor::Middle) : anchor(a), units(u), value(v) {}
    Anchor anchor;
    Units units;
    float value;
private:
    PosSpec();
};

using HorzPos = PosSpec<HorzAnchor>;
using VertPos = PosSpec<VertAnchor>;


struct SizeSpec
{
    // implicit constructor
    SizeSpec(float v, Units u = Units::Percentage) : value(v) {}
    Units units;
    float value;
private:
    SizeSpec();
};

struct AABB
{
    float xMin, xMax, yMin, yMax;
private:
    //AABB();
};


class GUITransform
{
public:
    struct Position
    {
        HorzPos x;
        VertPos y;
    };

    struct Size
    {
        SizeSpec x;
        SizeSpec y;
    };

    inline GUITransform(Position &&pos, Size &&size) : mPos(std::move(pos)), mSize(std::move(size)) {}

    inline vmath::Matrix4 getTransformMatrix() const
    {
        std::array<float, 2> top_left = getTopLeftCorner();
        return vmath::Matrix4::translation({top_left[0], top_left[1], 0.0f}) *
               vmath::Matrix4::scale({mSize.x.value, mSize.y.value, 1.0f});
    }

    inline static vmath::Matrix4 getScreenSpaceTransform()
    {
        return vmath::Matrix4::scale({1.0f, -1.0f, 1.0f}) *
               vmath::Matrix4::translation({-1.0f, -1.0f, 0.0f}) *
               vmath::Matrix4::scale({2.0f, 2.0f, 1.0}) *
               vmath::Matrix4::identity();
    }

    inline static vmath::Matrix4 getInverseScreenSpaceTransform()
    {
        return vmath::inverse(getScreenSpaceTransform());
    }


    inline const Position &getPos() const { return mPos; }
    inline const Size &getSize() const { return mSize; }

    inline void setPos(Position &&pos) { mPos = std::move(pos); }
    inline void setSize(Size &&size) { mSize = std::move(size); }

    inline AABB getAABB() const
    {
        std::array<float, 2> top_left = getTopLeftCorner();
        return { top_left[0], top_left[0] + mSize.x.value, top_left[1], top_left[1] + mSize.y.value };
    }

private:
    GUITransform();

    inline std::array<float, 2> getTopLeftCorner() const
    {
        std::array<float, 2> out;
        out[0] = (mPos.x.anchor == HorzAnchor::Left)      ? mPos.x.value                      :
                 (mPos.x.anchor == HorzAnchor::Middle)    ? mPos.x.value - 0.5*mSize.x.value  :
                                                            mPos.x.value -     mSize.x.value;

        out[1] = (mPos.y.anchor == VertAnchor::Top)       ? mPos.y.value                      :
                 (mPos.y.anchor == VertAnchor::Middle)    ? mPos.y.value - 0.5*mSize.y.value  :
                                                            mPos.y.value -     mSize.y.value;
        return out;
    }

    Position mPos;
    Size mSize;

};

} // namespace gui

} // namespace gfx

#endif // GUITRANSFORM_H
