#ifndef NEWGAMEINFO_H
#define NEWGAMEINFO_H

#include "../events/immediateevents.h"
#include "../state/macrostate.h"
#include "../common/pointer.h"

class NewGameInfo
{
    // pointer to macro world
    Ptr::OwningPtr<state::MacroState> mMacroStatePtr;

    // some info about where/as what to start


    // character customization

    // callback reference
    events::Immediate::CallbackRef<events::GenerateWorldEvent> mGenerateWorldCallbackRef;

public:
    NewGameInfo(); // register gui callbacks and set defaults..
    ~NewGameInfo(); // unregister gui callbacks

    Ptr::OwningPtr<state::MacroState>&& moveMacroState();
    Ptr::ReadPtr<state::MacroState>     readMacroState();
};

#endif // NEWGAMEINFO_H
