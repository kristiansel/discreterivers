#ifndef IMMEDIATEEVENTS_H
#define IMMEDIATEEVENTS_H

#include <list>
#include <functional>

namespace events {

namespace Immediate {

template<typename EventType>
class Dispatcher
{
private:
    Dispatcher() {} // singleton type
    std::list<std::function<void(EventType)>> mCallbacks;
public:
    static Dispatcher &get()
    {
        static Dispatcher sDispatcher;
        return sDispatcher;
    }

    void addCallback(std::function<void(EventType)> &&callback)
    {
        std::cout << "adding callback" << std::endl;
        mCallbacks.emplace_back(std::move(callback));
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

} // namespace Immediate

// Event types
struct FPSUpdateEvent
{
    float fps;
};



} // namespace events



#endif // IMMEDIATEEVENTS_H
