#ifndef EVENTS_H
#define EVENTS_H

#include <queue>

#include "../common/either.h"

namespace events {

namespace Deferred
{

struct QuitEvent {};
struct ToggleFullscreenEvent {};
struct IncrUIScaleFactor {};
struct DecrUIScaleFactor {};

// should specialize either for empty types...
using Event = stdext::either<
    QuitEvent,
    ToggleFullscreenEvent,
    IncrUIScaleFactor,
    DecrUIScaleFactor
>;

using EventQueue = std::queue<Event>;

EventQueue &getEventQueue();

void emitEvent(Event e);

}

}



#endif // EVENTS_H
