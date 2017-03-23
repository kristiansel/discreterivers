#ifndef IMMEDIATEEVENTS_H
#define IMMEDIATEEVENTS_H

#include <list>
#include <functional>
#include "../state/macrostate.h"
#include "../state/microstatecreationinfo.h"
#include "../common/pointer.h"

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
        std::cout << "adding callback" << std::endl;
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


// Event types

struct ToggleMainMenuEvent {};

struct NewGameEvent {};

struct SaveGameEvent {};

struct LoadGameEvent {};

struct OptionsEvent {};

struct ChooseOriginEvent {};

struct CharCreateEvent {};

// event fired after character creation menu!
struct StartGameEvent {};

// event fired when game is quit (to main menu)
struct EndGameEvent {};

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

    int planet_seed;
};

struct FinishGenerateWorldEvent
{
    Ptr::ReadPtr<state::MacroState> scene_data;
};

struct UIScaleFactorUpdate {
    enum class ScaleFactor { Small, Medium, Large };

    ScaleFactor size;
};

struct CreateSceneEvent
{
    Ptr::ReadPtr<state::MicroStateCreationInfo> scene_creation_info;
};

} // namespace events



#endif // IMMEDIATEEVENTS_H
