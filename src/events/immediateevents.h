#ifndef IMMEDIATEEVENTS_H
#define IMMEDIATEEVENTS_H

#include "immediatedispatcher.h"
#include "../state/macrostate.h"
#include "../state/scenecreationinfo.h"
#include "../common/pointer.h"

namespace events {

// Event types

struct ToggleMainMenuEvent {};

struct NewGameEvent
{
    NewGameEvent(bool clear_state = true) : clear_state(clear_state) {}
    bool clear_state;
};

struct CancelNewGameEvent {};

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

struct SimStatusUpdateEvent
{
   bool sim_paused;
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
    Ptr::ReadPtr<state::SceneCreationInfo> scene_creation_info;
};

} // namespace events



#endif // IMMEDIATEEVENTS_H
