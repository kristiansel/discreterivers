#ifndef EVENTS_H
#define EVENTS_H

#include <queue>

#include "../common/either.h"

namespace events {

namespace Queued
{

struct QuitEvent {};
struct ToggleFullscreenEvent {};

// should specialize either for empty types...
using Event = stdext::either<
    QuitEvent,
    ToggleFullscreenEvent
>;

using EventQueue = std::queue<Event>;

EventQueue &getEventQueue();

void emitEvent(Event e);

}

}



#endif // EVENTS_H
