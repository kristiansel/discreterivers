#ifndef MICROSTATECREATIONINFO_H
#define MICROSTATECREATIONINFO_H

#include <vector>
#include "macrostate.h"
#include "../common/pointer.h"
#include "../common/gfx_primitives.h"

namespace state {

struct ActorCreationInfo
{
    unsigned int id;

    vmath::Vector3 pos;
    vmath::Quat rot;

    // more info. All become boxes for now...
};

struct MicroStateCreationInfo
{
    Ptr::ReadPtr<MacroState> macro_state_ptr;
    vmath::Vector3 anchor_pos;
    std::vector<ActorCreationInfo> actors;
};

}

#endif // MICROSTATECREATIONINFO_H
