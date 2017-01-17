#include "events.h"

namespace events {

EventQueue &getEventQueue()
{
   static EventQueue sEventQueue;
   return sEventQueue;
}

void emitEvent(Event e)
{
    getEventQueue().push(e);
}

}
