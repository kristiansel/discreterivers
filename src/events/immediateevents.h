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
    template<typename BroadCastEvtType>
    friend void broadcast(const BroadCastEvtType &evt);

    template<typename CallbackEvtType>
    friend void add_callback(std::function<void(CallbackEvtType)> &&callback);

    Dispatcher() {} // singleton type
    std::list<std::function<void(EventType)>> mCallbacks;

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

template<typename EventType>
void broadcast(const EventType &evt)
{
    Dispatcher<EventType>::get().broadcast(evt);
}

template<typename EventType>
void add_callback(std::function<void(EventType)> &&callback)
{
    Dispatcher<EventType>::get().addCallback(std::move(callback));
}

} // namespace Immediate


// Event types

struct ToggleMainMenuEvent {};

struct NewGameEvent {};

struct SaveGameEvent {};

struct LoadGameEvent {};

struct OptionsEvent {};

struct FPSUpdateEvent
{
    float fps;
};

struct GenerateWorldEvent
{
    enum class PlanetShape {Sphere, Disk, Torus};
    enum class PlanetSize  {Small, Medium, Large};

    PlanetShape planet_shape;
    PlanetSize planet_size;
};


} // namespace events



#endif // IMMEDIATEEVENTS_H
