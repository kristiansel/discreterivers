#ifndef GUINODE_H
#define GUINODE_H

#include <list>
#include <iostream>
#include <functional>

#include "guielement.h"
#include "guitransform.h"
#include "guitextvertices.h"
#include "guifont.h"
#include "../texture.h"
#include "../../common/flags.h"
#include "../../common/gfx_primitives.h"
#include "../../common/resmanager/refcounted.h"


namespace gfx {

class OpenGLRenderer;

namespace gui {

class GUINode;
using GUINodeHandle = std::list<GUINode>::iterator;
using GUIElementHandle = std::list<GUIElement>::iterator;
using GUINodePtr = gui::GUINode*;

class GUINode// : public Resource::RefCounted<GUINode> // why does this need to be refcounted. It has no pointer data that can be reused...
{
    // private class GUIFlags
    enum GUIFlag
    {
        Default       = 0b00000000,
        Hide          = 0b00000001,
        ClickPassThru = 0b00000010
    };

    using GUIFlags = stdext::Flags<GUIFlag, GUIFlag::Default>;
public:
    using MouseEnterHandler = std::function<void(void)>;
    using MouseLeaveHandler = std::function<void(void)>;
    using MouseClickHandler = std::function<void(void)>;

    template<typename F, typename... Args>
    struct GUIEventHandler
    {
        GUIEventHandler() = default;
        GUIEventHandler(GUIEventHandler &&other) : callbacks(std::move(other.callbacks)) {}
        using CallbackT = std::function<F(Args...)>;
        friend class GUINode;
        inline void addCallback(CallbackT &&cb) { callbacks.emplace_back(std::move(cb)); }
        inline void addCallback(const CallbackT &cb) { callbacks.push_back(cb); }
        inline void handle(Args... a) { }
        inline void invokeCallbacks(Args... a) { for (const auto &c : callbacks) { c(a...); } }
    private:
        std::vector<CallbackT> callbacks;
    };
    //friend class GUIEventHandler;

    bool handleMouseClick(float x, float y) {
        gui::AABB aabb = mGUITransform.getAABB();
        if ( (x < aabb.xMax && x > aabb.xMin) &&
             (y < aabb.yMax && y > aabb.yMin) &&
              !mGUIFlags.checkFlag(GUIFlag::Hide) )
        {
            //std::cout << "clicked inside child " << x << ", " << y << std::endl;
            mouseClick.invokeCallbacks();

            float x_rel = (x-aabb.xMin)/(aabb.xMax-aabb.xMin);
            float y_rel = (y-aabb.yMin)/(aabb.yMax-aabb.yMin);

            bool result = !mGUIFlags.checkFlag(GUIFlag::ClickPassThru);
            for (auto &child : mChildren)
            {
                bool child_solid = child.handleMouseClick(x_rel, y_rel);
                result = result || child_solid;
            }
            return result;
        }
        else
        {
            return false;
        }
    }

    GUINodePtr getDeepestHovered(float x, float y, bool debug = false)
    {
        gui::AABB aabb = mGUITransform.getAABB();
        if ( (x < aabb.xMax && x > aabb.xMin) &&
             (y < aabb.yMax && y > aabb.yMin) &&
              !mGUIFlags.checkFlag(GUIFlag::Hide) )
        {
            //if (debug) std::cout << " inside: " << this << std::endl;
            float x_rel = (x-aabb.xMin)/(aabb.xMax-aabb.xMin);
            float y_rel = (y-aabb.yMin)/(aabb.yMax-aabb.yMin);

            //if (debug) std::cout << " # children: " << mChildren.size() << std::endl;
            GUINodePtr child_hovered = nullptr;
            for (auto &child : mChildren)
            {
                GUINodePtr result = child.getDeepestHovered(x_rel, y_rel);
                child_hovered = result ? result : child_hovered;
            }
            //if (debug) std::cout << " child_hovered: " << (child_hovered ? true : false) << std::endl;
            return (child_hovered != nullptr) ? child_hovered : this;
        }
        else
        {
            //if (debug) std::cout << " not inside: " << this << std::endl;
            return nullptr;
        }
    }

    // to handle mouse leave, keep track of the deepest node that the mouse was in previously
    // check which is the deepest on the current tick. If it changed:
    // fire a.leave() and b.enter()
    // should perhaps reside in the gui class, or be a static? No, static is just wrong... keep in renderer for now

    explicit inline GUINode(const GUITransform &gui_transform);

    // vector init needs copy constructor... strange? Ah, it is because of initializer list
    // elements of the list are always passed as const reference 18.9 in standard

    inline GUINode(GUINode &&gn) : //Resource::RefCounted<GUINode>(std::move(gn)),
        mGUITransform(gn.mGUITransform), mChildren(std::move(gn.mChildren)), mElements(std::move(gn.mElements)),
        mGUIFlags(GUIFlag::Default), mouseEnter(std::move(gn.mouseEnter)), mouseLeave(std::move(gn.mouseLeave)),
        mouseClick(std::move(gn.mouseClick)) {}

    inline GUINode(const GUINode &gn) = default;

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

    // read methods
    inline const std::list<GUINode> &getChildren() const { return mChildren; }
    inline const std::list<GUIElement> &getElements() const { return mElements; }
    inline const GUITransform &getTransform() const { return mGUITransform; }
    inline const bool isVisible() const { return !mGUIFlags.checkFlag(GUIFlag::Hide); }

    // mutate methods
    inline void hide()                  { mGUIFlags.setFlag(GUIFlag::Hide);             }
    inline void show()                  { mGUIFlags.clearFlag(GUIFlag::Hide);           }
    inline void toggleShow()            { mGUIFlags.toggleFlag(GUIFlag::Hide);          }

    inline void clickSolid()            { mGUIFlags.clearFlag(GUIFlag::ClickPassThru);  }
    inline void clickPassThru()         { mGUIFlags.setFlag(GUIFlag::ClickPassThru);    }
    inline void toggleClickPassThru()   { mGUIFlags.toggleFlag(GUIFlag::ClickPassThru); }

public:
    GUIEventHandler<void> mouseEnter;
    GUIEventHandler<void> mouseLeave;
    GUIEventHandler<void> mouseClick;

    // Resource::RefCounted<GUINode>
    // inline void resourceDestruct() { std::cout << "deleting gui node" << std::endl; }

private:
    GUINode();

    // private some operators?
    GUITransform mGUITransform;
    GUIFlags mGUIFlags;

    std::list<GUINode> mChildren;
    std::list<GUIElement> mElements;


};


inline GUINode::GUINode(const GUITransform &gui_transform) :
    mGUITransform(gui_transform), mGUIFlags(GUIFlag::Default)
{
    // value ctor
}

} // gui

} // gfx

#endif // GUINODE_H
