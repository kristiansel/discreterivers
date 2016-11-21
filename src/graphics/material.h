#ifndef MATERIAL_H
#define MATERIAL_H

#include "../common/gfx_primitives.h"

namespace gfx {

struct Material
{
    Material() : color(1.0f), wireframe(false), visible(true) {}
    Material(const Material &m) : color(m.color), wireframe(m.wireframe), visible(m.visible) {}
    explicit Material(const vmath::Vector4 &color_in) : color(color_in), wireframe(false), visible(true) {}

    // get
    inline const vmath::Vector4 &getColor() const {return color;}
    inline const bool &getWireframe() const {return wireframe;}
    inline const bool &getVisible() const {return visible;}

    // set
    inline void setColor(const vmath::Vector4 &color_in) {color=color_in;}
    inline void setWireframe(bool w) {wireframe=w;}
    inline void setVisible(bool v) {visible=v;}

private:
    vmath::Vector4 color;
    bool wireframe;
    bool visible;
};

}

#endif // MATERIAL_H
