#ifndef GUISTATE_H
#define GUISTATE_H

#include <type_traits>

namespace gfx {

namespace gui {

class GUINode;
void onGUINodeStateUpdate(GUINode &gui_node);

template<typename StateT>
class GUIStateWriter
{
    StateT *mStatePtr;
    GUINode *mGUINodePtr;
    bool mUpdateOnWrite;

    GUIStateWriter()                            = delete;
public:
    GUIStateWriter(StateT *state_ptr, GUINode *gui_node_ptr, bool update_on_write = true) :
        mStatePtr(state_ptr), mGUINodePtr(gui_node_ptr), mUpdateOnWrite(update_on_write) {}

    StateT *const operator->() {
        return mStatePtr;
    }

    ~GUIStateWriter()
    {
        if (mUpdateOnWrite) onGUINodeStateUpdate(*mGUINodePtr);
    }
};

template<typename StateT>
class GUIStateReader
{
    StateT *mStatePtr;

    GUIStateReader()                            = delete;
public:
    GUIStateReader(StateT *state_ptr) :
        mStatePtr(state_ptr) {}

    const StateT *const operator->() {
        return mStatePtr;
    }
};

class GUIStateBase
{
public:
    virtual ~GUIStateBase() {}
};

template<typename StateT>
class GUIStateHandle
{
    StateT *mStatePtr;
    GUINode *mGUINodePtr;

    GUIStateHandle()                            = delete;
public:
    GUIStateHandle(StateT *state_ptr, GUINode *gui_node_ptr) :
        mStatePtr(state_ptr), mGUINodePtr(gui_node_ptr) {}

    GUIStateWriter<StateT> getStateWriter() const { return GUIStateWriter<StateT>(mStatePtr, mGUINodePtr); }
    GUIStateReader<StateT> getStateReader() const { return GUIStateReader<StateT>(mStatePtr);              }
    GUIStateWriter<StateT> getStateWriterNoUpdate() const { return GUIStateWriter<StateT>(mStatePtr, mGUINodePtr, false); }
};


namespace detail
{

class GUIState
{
    GUIStateBase * mStatePtr;
    GUINode * mGUINodePtr;

public:
    GUIState(GUINode * gui_node_ptr) :
        mStatePtr(nullptr), mGUINodePtr(gui_node_ptr) {}

    template<typename StateT>
    GUIStateHandle<StateT> getGUIStateHandle(const StateT &state)
    {
        static_assert(std::is_base_of<GUIStateBase, StateT>::value, "StateT must inherit GUIStateBase");
        delete mStatePtr;
        mStatePtr = new StateT(state);
        return GUIStateHandle<StateT>(dynamic_cast<StateT*>(mStatePtr), mGUINodePtr);
    }

    ~GUIState() { delete mStatePtr; }

};

} // namespace detail

} // namespace gui

} // namespace gfx

#endif // GUISTATE_H
