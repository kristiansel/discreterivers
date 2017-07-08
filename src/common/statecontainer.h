#ifndef STATE_CONTAINER_H
#define STATE_CONTAINER_H

#include <type_traits>
#include "macro/macrodebugassert.h"

namespace State {

class StateContainer;

template<typename T>
class StateHandle;

inline void onStateContainerUpdate(StateContainer * _state);

template<typename StateT>
class StateWriter
{
    friend class StateHandle<StateT>;

    StateT *mStatePtr;
    StateContainer *mStateContainerPtr;
    bool mUpdateOnWrite;

    StateWriter()                            = delete;


    StateWriter(StateT *state_ptr, StateContainer *_state_ptr, bool update_on_write = true) :
        mStatePtr(state_ptr), mStateContainerPtr(_state_ptr), mUpdateOnWrite(update_on_write) {}

public:
    // trivial move and copy (weak ref semantics)
    StateWriter(StateWriter &&s)             = default;
    StateWriter(const StateWriter &s)        = default;

    StateT *const operator->() {
        return mStatePtr;
    }

    ~StateWriter()
    {
        if (mUpdateOnWrite) onStateContainerUpdate(mStateContainerPtr);
    }
};

template<typename StateT>
class StateReader
{
    friend class StateHandle<StateT>;

    StateT *mStatePtr;

    StateReader()                            = delete;

    StateReader(StateT *state_ptr) :
        mStatePtr(state_ptr) {}

public:
    // trivial move and copy (weak ref semantics)
    StateReader(StateReader &&s)             = default;
    StateReader(const StateReader &s)        = default;

    const StateT *const operator->() {
        return mStatePtr;
    }
};

class StateBase
{
public:
    virtual ~StateBase() {}
};

template<typename StateT>
class StateHandle
{
    friend class StateContainer;

    StateT *mStatePtr;
    StateContainer *mStateContainerPtr;

    StateHandle()                            = delete;

    StateHandle(StateT *state_ptr, StateContainer *state_container_ptr) :
        mStatePtr(state_ptr), mStateContainerPtr(state_container_ptr)
    {
        DEBUG_ASSERT(state_container_ptr!=nullptr&&state_ptr!=nullptr);
    }

public:
    // trivial move and copy (weak ref semantics)
    StateHandle(StateHandle &&s)             = default;
    StateHandle(const StateHandle &s)        = default;

    StateWriter<StateT> getStateWriter() const { return StateWriter<StateT>(mStatePtr, mStateContainerPtr); }
    StateReader<StateT> getStateReader() const { return StateReader<StateT>(mStatePtr); }
    StateWriter<StateT> getStateWriterNoUpdate() const { return StateWriter<StateT>(mStatePtr, mStateContainerPtr, false); }
};

class StateContainer
{
    StateBase * mStatePtr;

    // event handler stuff
    using CallbackT = std::function<void(void)>;
    std::vector<CallbackT> callbacks;

    // no copy/move possible...
    StateContainer(StateContainer &&s) = delete;
    StateContainer(const StateContainer &s) = delete;

public:
    StateContainer() :
        mStatePtr(nullptr) {}

    template<typename StateT>
    StateHandle<StateT> getStateHandle(const StateT &state)
    {
        static_assert(std::is_base_of<StateBase, StateT>::value, "StateT must inherit StateBase");
        delete mStatePtr;
        mStatePtr = new StateT(state);
        return StateHandle<StateT>(dynamic_cast<StateT*>(mStatePtr), this/*, mNodePtr*/);
    }

    inline void addCallback(CallbackT &&cb)      { callbacks.emplace_back(std::move(cb)); }
    inline void addCallback(const CallbackT &cb) { callbacks.push_back(cb); }
    inline void invokeCallbacks()       { for (const auto &c : callbacks) { c(); } }
    inline void onStateUpdate() { invokeCallbacks(); }

    ~StateContainer() { delete mStatePtr; }

};

inline void onStateContainerUpdate(StateContainer * _state)
{
    _state->onStateUpdate();
}

} // namespace State

#endif // STATE_CONTAINER_H
