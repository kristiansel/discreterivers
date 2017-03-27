#include "newgameinfo.h"

#include "../system/async.h"
#include "../createscene.h"

NewGameInfo::NewGameInfo() :
    mMacroStatePtr(nullptr)
{
    // register callbacks
    mGenerateWorldCallbackRef = events::Immediate::add_callback<events::GenerateWorldEvent>(
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

    // set defaults
}

NewGameInfo::~NewGameInfo()
{
    // unregister callbacks
    events::Immediate::remove_callback<events::GenerateWorldEvent>(mGenerateWorldCallbackRef);
}

Ptr::OwningPtr<state::MacroState>&& NewGameInfo::moveMacroState()
{
    return std::move(mMacroStatePtr);
}

Ptr::ReadPtr<state::MacroState>     NewGameInfo::readMacroState()
{
    return mMacroStatePtr.getReadPtr();
}
