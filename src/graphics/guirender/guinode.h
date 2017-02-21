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
#include "guievent.h"
#include "../texture.h"
#include "../../common/flags.h"
#include "../../common/initptr.h"
#include "../../common/statecontainer.h"
#include "../../common/gfx_primitives.h"
#include "../../common/resmanager/refcounted.h"


namespace gfx {

class OpenGLRenderer;

namespace gui {

class GUINode;
using GUINodeHandle = std::list<GUINode>::iterator;
using GUIElementHandle = std::list<GUIElement>::iterator;
using GUINodePtr = stdext::InitPtr<gui::GUINode>;

using GUIStateContainer = State::StateContainer;
using GUIStateBase = State::StateBase;

template<typename T>
using GUIStateHandle    = State::StateHandle<T>;

template<typename T>
using GUIStateWriter    = State::StateWriter<T>;

template<typename T>
using GUIStateReader    = State::StateReader<T>;

enum GUIFlag
{
    Default       = 0b00000000,
    Hide          = 0b00000001,
    ClickPassThru = 0b00000010
};

class GUINode
{
    using GUIFlags = stdext::Flags<GUIFlag, GUIFlag::Default>;
public:

    using GUIEventHandler = std::function<void(const GUIEvent&)>;

    // Constructor
    explicit GUINode(const GUITransform &gui_transform);

    // Move constructor
    /*GUINode(GUINode &&gn) :
        mGUITransform(std::move(gn.mGUITransform)),
        mGUIFlags(gn.mGUIFlags),
        mGUIState(std::move(gn.mGUIState), this),
        mGUIEventHandler(std::move(gn.mGUIEventHandler)),
        mChildren(std::move(gn.mChildren)),
        mElements(std::move(gn.mElements)),
        stateUpdate(std::move(gn.stateUpdate)) {}*/

    // gui node cannot move... all state writer handles would have to be update to point to new...

    // methods
    GUINodePtr getDeepestClicked(float x, float y, float w_abs, float h_abs);
    GUINodePtr getDeepestHovered(float x, float y, float w_abs, float h_abs, std::vector<GUINodePtr> &hovered);
    void resize(float w_abs, float h_abs);

    template<typename StateT>
    GUIStateHandle<StateT> setState(const StateT &state) { return mGUIState.getStateHandle<StateT>(state); }

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

    inline void isClickPassThru()       { mGUIFlags.checkFlag(GUIFlag::ClickPassThru);  }
    inline void clickSolid()            { mGUIFlags.clearFlag(GUIFlag::ClickPassThru);  }
    inline void clickPassThru()         { mGUIFlags.setFlag(GUIFlag::ClickPassThru);    }
    inline void toggleClickPassThru()   { mGUIFlags.toggleFlag(GUIFlag::ClickPassThru); }


    template <typename F>
    inline void addStateUpdateCallback(F f) { mGUIState.addCallback(std::forward<F>(f)); }

public:

    void forceStateUpdate();
    void setGUIEventHandler(GUIEventHandler &&gui_event_handler) { mGUIEventHandler = std::move(gui_event_handler); }
    void handleEvent(const GUIEvent &evt) { mGUIEventHandler(evt); }

private:
    GUINode() = delete;
    GUINode(const GUINode &gn) = delete;
    GUINode(GUINode &&gn) = delete;

    // private some operators?
    GUITransform mGUITransform;
    GUIFlags mGUIFlags;
    GUIStateContainer mGUIState;

    GUIEventHandler mGUIEventHandler;

    std::list<GUINode> mChildren;
    std::list<GUIElement> mElements;

    // helper methods
    inline GUINodePtr getDeepestIntersectingNode(float x, float y, float w_abs, float h_abs, GUIFlag flag_mask = GUIFlag::Default);
    inline bool thisSizeChangesWithParent();

    inline void onStateUpdate();
};

} // gui

} // gfx

#endif // GUINODE_H
