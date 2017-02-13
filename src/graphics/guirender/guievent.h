#ifndef GUIEVENT_H
#define GUIEVENT_H


#include <SDL2/SDL.h>
#include "../../common/either.h"

namespace gfx {

namespace gui {

// Mouse events
enum class MouseButton { Left, Middle, Right };

struct MouseMoveEvent
{
    uint16_t x;
    uint16_t y;
};

struct MouseButtonDownEvent
{
    MouseButton button;
    uint16_t x;
    uint16_t y;
};

struct MouseButtonUpEvent
{
    MouseButton button;
    uint16_t x;
    uint16_t y;
};

// Key events
using KeyCode = SDL_Keycode;

struct KeyDownEvent
{
    KeyCode key_code;
};

struct KeyUpEvent
{
    KeyCode key_code;
};

using GUIEvent = stdext::either<
    MouseMoveEvent,
    MouseButtonDownEvent,
    MouseButtonUpEvent,
    KeyDownEvent,
    KeyUpEvent
>;

}

}

#endif // GUIEVENT_H
