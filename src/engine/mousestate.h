#ifndef MOUSESTATE_H
#define MOUSESTATE_H

namespace engine {

struct MouseState
{
    bool lmb_down;
    bool rmb_down = false;

    int32_t prev_mouse_x;
    int32_t prev_mouse_y;

    MouseState() : lmb_down(false), rmb_down(false), prev_mouse_x(0), prev_mouse_y(0) {}

};

} // namespace Engine

#endif // MOUSESTATE_H
