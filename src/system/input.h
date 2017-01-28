#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

class Input
{
public:
    void handleKeyboardEvent(const SDL_Event &event);

private:
    Input();
};

#endif // INPUT_H
