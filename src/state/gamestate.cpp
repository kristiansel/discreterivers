#include "gamestate.h"

#include "../events/immediateevents.h"
#include "../system/async.h"
#include "../createscene.h"

namespace state {

GameState::GameState() :
    mMacroStatePtr(nullptr),
    mMicroStatePtr(nullptr)
{
    // register event callbacks
    events::Immediate::add_callback<events::GenerateWorldEvent>(
        [this] (const events::GenerateWorldEvent &evt) {
            sys::Async::addJob(
                // The asynchronous operation
                        [evt]()->Ptr::OwningPtr<MacroState> {
                            return createPlanetData(evt.planet_shape, evt.planet_size, evt.planet_seed);
                        },
                // Process the result on return
                        [this](Ptr::OwningPtr<MacroState> &scene_data)->void{
                            // move the pointer into world object
                            std::cout << "STATE moving IN world ptr" << std::endl;
                            this->mMacroStatePtr = std::move(scene_data);

                            // world object fires something
                            events::Immediate::broadcast(events::FinishGenerateWorldEvent{this->mMacroStatePtr.getReadPtr()});
                        });
        }
    );
}

}
