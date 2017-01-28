#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include "../gui/gui.h"
#include "../graphics/openglrenderer.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"

// organize window/input handling under system..?

// engine marshals input around and handles cross-system communication?


class Engine
{
    // members
    gfx::OpenGLRenderer mRenderer; // must be initialized before GUI!
    gui::GUI mGUI;  // needs valid opengl context and glew init

    // to be moved
    // Camera
    gfx::Camera camera;

    // deleted methods
    Engine() = delete;
    Engine(Engine &&e) = delete;
    Engine(const Engine &e) = delete;

public:
    Engine(int w, int h, int dpi);

    // mutators
    inline void draw();
    inline void resize(int w, int h);
    void handleKeyDownEvent(SDL_Keycode k);

    // getters
    inline gui::GUI &getGui() { return mGUI; }
    inline gfx::OpenGLRenderer &getRenderer() { return mRenderer; }

};

// inline function definitions
inline void Engine::draw()
{
    mRenderer.draw(camera, mGUI.getGUIRoot());
}

inline void Engine::resize(int w, int h)
{
    mRenderer.resize(w, h);
    mGUI.resize(w, h);
}

#endif // ENGINE_H
