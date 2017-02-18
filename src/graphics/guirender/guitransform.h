#ifndef GUITRANSFORM_H
#define GUITRANSFORM_H

#include <array>
#include "../../common/gfx_primitives.h"

namespace gfx {

namespace gui {

// Define some types
enum class Units { Relative, Absolute };

enum class HorzAnchor { Left, Middle, Right };
enum class VertAnchor { Top, Middle, Bottom };

enum class AgnosticFrom : int { Near = 0, Middle = 1, Far = 2};

enum class HorzFrom : int { Left = int(AgnosticFrom::Near),
                            Middle = int(AgnosticFrom::Middle),
                            Right  = int(AgnosticFrom::Far)};

enum class VertFrom : int { Top = int(AgnosticFrom::Near),
                            Middle = int(AgnosticFrom::Middle),
                            Bottom  = int(AgnosticFrom::Far)};

struct SizeSpec
{
    // implicit constructor
    SizeSpec(float v, Units u = Units::Relative, bool parent_minus = false) :
        value(v), units(u), mParentMinus(parent_minus) {}

    Units units;
    bool mParentMinus;

    inline float getRelative(float abs_val) const
    {
        if (units != Units::Relative)
        {
            if (mParentMinus)
            {
                return (abs_val - value)/abs_val;
            }
            else
            {
                return value/abs_val;
            }
        }
        else
        {
            return value;
        }
    }

private:
    float value;

    SizeSpec() = delete;
};

template <typename Anchor, typename AbsFrom>
struct PosSpec
{
    // implicit constructor
    PosSpec(float v, Units u = Units::Relative, Anchor a = Anchor::Middle, AbsFrom f = AbsFrom(AgnosticFrom::Near)) :
        value(v), units(u), anchor(a), abs_from(f) {}

    Units units;
    Anchor anchor;

    inline float getRelative(float abs_val) const
    {
        float out;

        if (units != Units::Relative)
        {
            out = value/abs_val;
            if (abs_from == AbsFrom(AgnosticFrom::Far))
            {
                out = 1.0f - out;
            }
            else if (abs_from == AbsFrom(AgnosticFrom::Middle))
            {
                out = 0.5f + out;
            }
        }
        else
        {
            out = value;
        }

        return out;
    }

private:
    float value;
    AbsFrom abs_from;
    PosSpec() = delete;
};


using HorzPos = PosSpec<HorzAnchor, HorzFrom>;
using VertPos = PosSpec<VertAnchor, VertFrom>;

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
    inline GUITransform(const Position &pos, const Size &size) : mPos(pos), mSize(size) {}

    inline vmath::Matrix4 getTransformMatrix(float x_abs, float y_abs)  const
    {
        std::array<float, 2> top_left = getTopLeftCorner(x_abs, y_abs);
        return vmath::Matrix4::translation({top_left[0], top_left[1], 0.0f}) *
               vmath::Matrix4::scale({mSize.x.getRelative(x_abs), mSize.y.getRelative(y_abs), 1.0f});
    }

    /*inline vmath::Matrix4 getTransformMatrix()  const
    {
        std::array<float, 2> top_left = getTopLeftCorner();
        return vmath::Matrix4::translation({top_left[0], top_left[1], 0.0f}) *
               vmath::Matrix4::scale({mSize.x.value, mSize.y.value, 1.0f});
    }*/

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

    inline AABB getAABB(float x_abs, float y_abs) const
    {
        std::array<float, 2> top_left = getTopLeftCorner(x_abs, y_abs);
        return { top_left[0], top_left[0] + mSize.x.getRelative(x_abs), top_left[1], top_left[1] + mSize.y.getRelative(y_abs) };
    }

private:
    GUITransform();

    inline std::array<float, 2> getTopLeftCorner(float x_abs, float y_abs) const
    {
        float pos_x_rel = mPos.x.getRelative(x_abs);
        float pos_y_rel = mPos.y.getRelative(y_abs);
        float size_x_rel = mSize.x.getRelative(x_abs);
        float size_y_rel = mSize.y.getRelative(y_abs);

        std::array<float, 2> out;
        out[0] = (mPos.x.anchor == HorzAnchor::Left)      ? pos_x_rel                      :
                 (mPos.x.anchor == HorzAnchor::Middle)    ? pos_x_rel - 0.5*size_x_rel  :
                                                            pos_x_rel -     size_x_rel;

        out[1] = (mPos.y.anchor == VertAnchor::Top)       ? pos_y_rel                      :
                 (mPos.y.anchor == VertAnchor::Middle)    ? pos_y_rel - 0.5*size_y_rel  :
                                                            pos_y_rel -     size_y_rel;
        return out;
    }

    /*inline std::array<float, 2> getTopLeftCorner() const
    {
        std::array<float, 2> out;
        out[0] = (mPos.x.anchor == HorzAnchor::Left)      ? mPos.x.value                      :
                 (mPos.x.anchor == HorzAnchor::Middle)    ? mPos.x.value - 0.5*mSize.x.value  :
                                                            mPos.x.value -     mSize.x.value;

        out[1] = (mPos.y.anchor == VertAnchor::Top)       ? mPos.y.value                      :
                 (mPos.y.anchor == VertAnchor::Middle)    ? mPos.y.value - 0.5*mSize.y.value  :
                                                            mPos.y.value -     mSize.y.value;
        return out;
    }*/


    Position mPos;
    Size mSize;

};

} // namespace gui

} // namespace gfx

#endif // GUITRANSFORM_H
