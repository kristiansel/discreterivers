#include "queuedevents.h"

namespace events {

namespace Deferred {


EventQueue &getEventQueue()
{
   static EventQueue sEventQueue;
   return sEventQueue;
}

void emitEvent(Event e)
{
    getEventQueue().push(e);
}

} // namespace Deferred

} // namespace event
