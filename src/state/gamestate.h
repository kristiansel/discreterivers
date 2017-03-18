#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <list>
#include "../common/pointer.h"
#include "macrostate.h"
#include "microstate.h"
#include "graphicsstatemanager.h"

namespace state {

class GameState
{
    // zero or one macro state
    Ptr::OwningPtr<MacroState> mMacroStatePtr;

    // zero or several serving micro-states
    //std::list<MicroState> mServingMicroStatePtrs;

    // zero or one mirror micro-state
    // Ptr::OwningPtr<MicroState> mMicroStatePtr;

    // subsystems here directly
    GraphicsStateManager mGraphicsStateManager;

public:
    GameState();

    Ptr::ReadPtr<MacroState> readMacroState() { return mMacroStatePtr.getReadPtr(); }

    void createMicroState(const MicroStateCreationInfo &micro_state_creation_info);
};

}

#endif // GAMESTATE_H
