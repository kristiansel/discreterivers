#ifndef IMMEDIATEDISPATCHER_H
#define IMMEDIATEDISPATCHER_H

#include <list>
#include <functional>
#include "../common/macro/debuglog.h"

namespace events {

namespace Immediate {

template<typename EventType>
using CallbackRef = typename std::list<std::function<void(EventType)>>::iterator;

template<typename EventType>
class Dispatcher
{
private:
    template<typename BroadCastEvtType>
    friend void broadcast(const BroadCastEvtType &evt);

    template<typename CallbackEvtType>
    friend void add_callback(std::function<void(CallbackEvtType)> &&callback);

    Dispatcher() {} // singleton type
    std::list<std::function<void(EventType)>> mCallbacks;

public:
    static Dispatcher &get()
    {
        static Dispatcher sDispatcher;
        return sDispatcher;
    }

    CallbackRef<EventType> addCallback(std::function<void(EventType)> &&callback)
    {
        DEBUG_LOG("adding callback");
        mCallbacks.emplace_back(std::move(callback));
        return (--(mCallbacks.end()));
    }

    void removeCallback(const CallbackRef<EventType> &callback_ref)
    {
        mCallbacks.erase(callback_ref);
    }

    // Should not need this.
    //void removeCallback;

    void broadcast(const EventType &evt)
    {
        for (const auto &callback : mCallbacks)
        {
            callback(evt);
        }
    }
};

template<typename EventType>
void broadcast(const EventType &evt)
{
    Dispatcher<EventType>::get().broadcast(evt);
}

template<typename EventType>
CallbackRef<EventType> add_callback(std::function<void(EventType)> &&callback)
{
    return Dispatcher<EventType>::get().addCallback(std::move(callback));
}

template<typename EventType>
void remove_callback(const CallbackRef<EventType> &callback_ref)
{
    Dispatcher<EventType>::get().removeCallback(callback_ref);
}

} // namespace Immediate

} // namespace events

#endif // IMMEDIATEDISPATCHER_H
