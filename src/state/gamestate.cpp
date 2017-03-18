#include "gamestate.h"

#include "../events/immediateevents.h"
#include "../system/async.h"
#include "../createscene.h"

namespace state {

GameState::GameState() :
    mMacroStatePtr(nullptr)
{
    // register event callbacks
    events::Immediate::add_callback<events::GenerateWorldEvent>(
        [this] (const events::GenerateWorldEvent &evt) {
            sys::Async::addJob(
                // The asynchronous operation
                        [evt]()->Ptr::OwningPtr<state::MacroState> {
                            return createPlanetData(evt.planet_shape, evt.planet_size, evt.planet_seed);
                        },
                // Process the result on return
                        [this](Ptr::OwningPtr<state::MacroState> &scene_data)->void{
                            // move the pointer into world object
                            std::cout << "STATE moving IN world ptr" << std::endl;
                            this->mMacroStatePtr = std::move(scene_data);

                            // world object fires something
                            events::Immediate::broadcast(events::FinishGenerateWorldEvent{this->mMacroStatePtr.getReadPtr()});
                        });
        }
    );

    // register event callbacks
    /*events::Immediate::add_callback<events::StartGameEvent>(
        [this] (const events::StartGameEvent &evt) {
            this->mMicroStatePtr = Ptr::OwningPtr<MicroState> ( new MicroState() );
        }
    );*/
}

void GameState::createMicroState(const MicroStateCreationInfo &micro_state_creation_info)
{
    //
}

}
