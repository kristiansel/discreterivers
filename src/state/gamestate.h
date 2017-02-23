#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <list>
#include "../common/initptr.h"
#include "macrostate.h"

namespace state {

using MacroState = SceneData;

struct MicroState {};

class GameState
{
    // zero or one macro state
    stdext::OwningInitPtr<MacroState> mMacroStatePtr;

    // zero or several serving micro-states
    //std::list<MicroState> mServingMicroStatePtrs;

    // zero or one mirror micro-state
    stdext::OwningInitPtr<MicroState> mMicroStatePtr;

public:
    GameState();
};

}

#endif // GAMESTATE_H
