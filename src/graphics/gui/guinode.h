#ifndef GUINODE_H
#define GUINODE_H

#include <list>
#include <iostream>

#include "guielement.h"
#include "guitransform.h"
#include "guitextvertices.h"
#include "guifontrenderer.h"
#include "../texture.h"
#include "../../common/gfx_primitives.h"
#include "../../common/resmanager/refcounted.h"


namespace gfx {

class OpenGLRenderer;

namespace gui {

class GUINode;
using GUINodeHandle = std::list<GUINode>::iterator;
using GUIElementHandle = std::list<GUIElement>::iterator;

class GUINode : public Resource::RefCounted<GUINode>
{
public:
    inline GUINode(const GUITransform &gui_transform);

    // vector init needs copy constructor... strange? Ah, it is because of initializer list
    // elements of the list are always passed as const reference 18.9 in standard

    GUINode(GUINode &&gn) : Resource::RefCounted<GUINode>(std::move(gn)),
        mGUITransform(gn.mGUITransform), mChildren(std::move(gn.mChildren)), mElements(std::move(gn.mElements))
    {}

    GUINode(const GUINode &gn) = default;


    template <typename ...Args>
    inline GUINodeHandle addGUINode(Args... args)
    {
        mChildren.emplace_back( std::forward<Args>(args)... );
        return (--mChildren.end());
    }

    template <typename ...Args>
    inline GUIElementHandle addElement(Args... args)
    {
        mElements.emplace_back( std::forward<Args>(args)... );
        return (--mElements.end());
    }

    inline const std::list<GUINode> &getChildren() const { return mChildren; }
    inline const std::list<GUIElement> &getElements() const { return mElements; }
    inline const GUITransform &getTransform() const { return mGUITransform; }

    // Resource::RefCounted<GUINode>
    void resourceDestruct() { std::cout << "deleting gui node" << std::endl; }

private:
    GUINode();

    // private some operators?

    GUITransform mGUITransform;

    std::list<GUINode> mChildren;
    std::list<GUIElement> mElements;
};


inline GUINode::GUINode(const GUITransform &gui_transform) :
    mGUITransform(gui_transform)
{

}

} // gui

} // gfx

#endif // GUINODE_H
