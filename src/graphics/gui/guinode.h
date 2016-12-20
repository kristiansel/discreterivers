#ifndef GUINODE_H
#define GUINODE_H

#include <list>
#include <iostream>

#include "guitransform.h"
#include "guielement.h"
#include "../../common/gfx_primitives.h"
#include "../../common/resmanager/refcounted.h"


namespace gfx {

class OpenGLRenderer;

namespace gui {

class GUINode : public Resource::RefCounted<GUINode>
{
public:
    GUINode(vmath::Vector4 && color, GUITransform && gui_transform, const char * text = "") :
        mColor(color), mGUITransform(gui_transform), mText(text) {}

    /*GUINode(GUINode &&gn) : Resource::RefCounted<GUINode>(gn),
        mColor(gn.mColor), mGUITransform(gn.mGUITransform), mChildren(std::move(gn.mChildren)) {}*/

    const GUITransform &getTransform() const { return mGUITransform; }
    const vmath::Vector4 &getColor() const { return mColor; }

    // Resource::RefCounted<GUINode>
    void resourceDestruct() { std::cout << "deleting gui node" << std::endl; }

private:
    GUINode();

    /*std::vector<GUINode> mChildren;*/

    const char * mText;

    vmath::Vector4 mColor;

    GUITransform mGUITransform;
};

} // gui

} // gfx

#endif // GUINODE_H
