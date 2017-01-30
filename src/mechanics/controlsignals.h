#ifndef CONTROLSIGNALS_H
#define CONTROLSIGNALS_H

#include "../common/flags.h"

namespace mech {


// move signals, loosely related to what happens to controlled legs/movement


enum MoveSignal
{
    Idle         = 0b00000000,
    Forward      = 0b00000001,
    Backward     = 0b00000010,
    Left         = 0b00000100,
    Right        = 0b00001000,

    Sprint       = 0b00010000,
    Walk         = 0b00100000,
    Crouch       = 0b01000000,
    Unused7      = 0b10000000,
};

using MoveSignalFlags = stdext::Flags<MoveSignal, Idle>;

// need some signals for rotation of mouse..


// discrete events
// jump
// toggle lie down
// toggle crouch
// toggle stand up


} // namespace mech

#endif // CONTROLSIGNALS_H
