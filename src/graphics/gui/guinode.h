#ifndef GUINODE_H
#define GUINODE_H

#include <list>
#include <iostream>

#include "guitransform.h"
#include "guielement.h"
#include "../texture.h"
#include "../../common/gfx_primitives.h"
#include "../../common/resmanager/refcounted.h"


namespace gfx {

class OpenGLRenderer;

namespace gui {

class GUINode : public Resource::RefCounted<GUINode>
{
public: // practically immutable?
    GUINode(vmath::Vector4 &&color, GUITransform &&gui_transform, std::string &&text = "",
            std::initializer_list<GUINode> &&children = {}, const Texture &texture = Texture()) :
        mColor(std::move(color)), mGUITransform(std::move(gui_transform)), mText(std::move(text)),
        mChildren(std::move(children)), mTexture(texture) {}
    // vector init needs copy constructor... strange? Ah, it is because of initializer list
    // elements of the list are always passed as const reference 18.9 in standard

    GUINode(GUINode &&gn) : Resource::RefCounted<GUINode>(std::move(gn)),
        mColor(gn.mColor), mGUITransform(gn.mGUITransform), mText(std::move(gn.mText)),
        mChildren(std::move(gn.mChildren)), mTexture(std::move(gn.mTexture)) {}

    GUINode(const GUINode &gn) = default;
    //GUINode(const GUINode &gn) : Resource::RefCounted<GUINode>(gn),
    //    mColor(gn.mColor), mGUITransform(gn.mGUITransform), mChildren(gn.mChildren),
    //    mText(gn.mText) { /*std::cout << "GUINode COPY constructor called" << std::endl;*/ }

    // TODO: move assign operator etc, needed?

    inline const GUITransform &getTransform() const { return mGUITransform; }
    inline const vmath::Vector4 &getColor() const { return mColor; }
    inline const std::vector<GUINode> &getChildren() const { return mChildren; }
    inline const GLuint getTextureID() const { return mTexture.getTextureID(); }

    // Resource::RefCounted<GUINode>
    void resourceDestruct() { std::cout << "deleting gui node" << std::endl; }

private:
    GUINode();

    // private some operators?

    std::vector<GUINode> mChildren;

    std::string mText;

    vmath::Vector4 mColor;

    GUITransform mGUITransform;

    Texture mTexture;
};

} // gui

} // gfx

#endif // GUINODE_H
