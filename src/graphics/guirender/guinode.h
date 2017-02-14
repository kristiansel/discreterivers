#ifndef GUINODE_H
#define GUINODE_H

#include <list>
#include <iostream>
#include <functional>
#include <type_traits>

#include "guielement.h"
#include "guitransform.h"
#include "guitextvertices.h"
#include "guifont.h"
#include "guistate.h"
#include "guievent.h"
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

class GUINode
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
    template<typename F, typename... Args>
    struct EventHandler
    {
        EventHandler() = default;
        EventHandler(EventHandler &&other) : callbacks(std::move(other.callbacks)) {}
        using CallbackT = std::function<F(Args...)>;
        friend class GUINode;
        inline void addCallback(CallbackT &&cb)      { callbacks.emplace_back(std::move(cb)); }
        inline void addCallback(const CallbackT &cb) { callbacks.push_back(cb); }
        inline void invokeCallbacks(Args... a)       { for (const auto &c : callbacks) { c(a...); } }
    private:
        std::vector<CallbackT> callbacks;
    };
    using GUIEventHandler = std::function<void(const GUIEvent&)>;

    // Constructor
    explicit inline GUINode(const GUITransform &gui_transform) :
        mGUITransform(gui_transform), mGUIFlags(GUIFlag::Default), mGUIState(this), mGUIEventHandler([](const GUIEvent&){}) {}

    // vector init needs copy constructor... strange? Ah, it is because of initializer list
    // elements of the list are always passed as const reference 18.9 in standard

    /*inline GUINode(GUINode &&gn) : //Resource::RefCounted<GUINode>(std::move(gn)),
        mGUITransform(gn.mGUITransform), mChildren(std::move(gn.mChildren)), mElements(std::move(gn.mElements)),
        mGUIFlags(GUIFlag::Default), mouseEnter(std::move(gn.mouseEnter)), mouseLeave(std::move(gn.mouseLeave)),
        mouseClick(std::move(gn.mouseClick))
    {
        mGUIStatePtr = gn.mGUIStatePtr;
        gn.mGUIStatePtr = nullptr;
    } // = delete */

    //inline GUINode(const GUINode &gn) = default;/

    // methods
    GUINodePtr handleMouseButtonDown(float x, float y, int32_t pix_x, int32_t pix_y);
    void handleMouseButtonUp(float x, float y, int32_t pix_x, int32_t pix_y);

    GUINodePtr getDeepestHovered(float x, float y, bool debug = false);

    template<typename StateT>
    GUIStateHandle<StateT> setState(const StateT &state) { return mGUIState.getGUIStateHandle<StateT>(state); }

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
    inline const std::list<GUINode> &getChildren()    const { return mChildren; }
    inline const std::list<GUIElement> &getElements() const { return mElements; }
    inline const GUITransform &getTransform()         const { return mGUITransform; }
    inline const bool isVisible()                     const { return !mGUIFlags.checkFlag(GUIFlag::Hide); }

    // mutate methods
    inline void hide()                  { mGUIFlags.setFlag(GUIFlag::Hide);             }
    inline void show()                  { mGUIFlags.clearFlag(GUIFlag::Hide);           }
    inline void toggleShow()            { mGUIFlags.toggleFlag(GUIFlag::Hide);          }

    inline void clickSolid()            { mGUIFlags.clearFlag(GUIFlag::ClickPassThru);  }
    inline void clickPassThru()         { mGUIFlags.setFlag(GUIFlag::ClickPassThru);    }
    inline void toggleClickPassThru()   { mGUIFlags.toggleFlag(GUIFlag::ClickPassThru); }



public:
    /*GUIEventHandler<void> mouseEnter;
    GUIEventHandler<void> mouseLeave;
    GUIEventHandler<void> mouseClick;
    GUIEventHandler<void> mouseRelease;*/
    EventHandler<void> stateUpdate;

    friend void onGUINodeStateUpdate(GUINode &gui_node);
    inline void onStateUpdate() { onGUINodeStateUpdate(*this); }

    void setGUIEventHandler(GUIEventHandler &&gui_event_handler) { mGUIEventHandler = std::move(gui_event_handler); }
    void handleEvent(const GUIEvent &evt) { mGUIEventHandler(evt); }

private:
    GUINode() = delete;
    GUINode(const GUINode &gn) = delete;
    GUINode(GUINode &&gn) = delete;

    // private some operators?
    GUITransform mGUITransform;
    GUIFlags mGUIFlags;
    detail::GUIState mGUIState;

    GUIEventHandler mGUIEventHandler;

    std::list<GUINode> mChildren;
    std::list<GUIElement> mElements;
};

} // gui

} // gfx

#endif // GUINODE_H
