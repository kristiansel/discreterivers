#include "queuedevents.h"

namespace events {

namespace Queued {


EventQueue &getEventQueue()
{
   static EventQueue sEventQueue;
   return sEventQueue;
}

void emitEvent(Event e)
{
    getEventQueue().push(e);
}

} // namespace Queued

} // namespace event
