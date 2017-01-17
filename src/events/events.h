#ifndef EVENTS_H
#define EVENTS_H

#include <queue>

#include "../common/either.h"

namespace events {

struct QuitEvent {};
struct OtherEvent {};

using Event = stdext::either<QuitEvent, OtherEvent>;
using EventQueue = std::queue<Event>;

EventQueue &getEventQueue();

void emitEvent(Event e);

}



#endif // EVENTS_H
