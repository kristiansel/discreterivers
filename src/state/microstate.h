#ifndef MICROSTATE_H
#define MICROSTATE_H

#include "actor.h"
#include "../common/freelistset.h"
#include "microstatecreationinfo.h"

namespace state {

class MicroState
{
    using ActorCollection = stdext::freelist_set<Actor, 1000>; // 1000 actors max...
    using ActorHandle     = ActorCollection::node*;

    // character controller...

    // Actors are characterized by control signals, physics and skeletal animation
    // as well as special mechanics
    ActorCollection mActors;

    // other stuff
public:
    MicroState();
};

}

#endif // MICROSTATE_H
