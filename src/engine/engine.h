#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>
#include "../gui/gui.h"
#include "../graphics/openglrenderer.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"
#include "../state/gamestate.h"
#include "../mechanics/cameracontroller.h"

#include "mousestate.h"

// organize window/input handling under system..?

// engine marshals input around and handles cross-system communication?

namespace engine {

class Engine
{
    // big members
    state::GameState    mGameState;
    gfx::OpenGLRenderer mRenderer;              // must be initialized before GUI!
    gui::GUI            mGUI;                   // needs valid opengl context and glew init

    // to be moved
    gfx::Camera             mCamera;              // init order important!
    //gfx::SceneNode          mGFXSceneRoot;       // main game scene
    mech::CameraController  mCameraController;   // must be init after camera

    bool mGUICapturedMouse;
    MouseState mMouseState; // should be in some sort of input handler...

    // immovable, non-copyable, non-default-constructible
    Engine() = delete;
    Engine(Engine &&e) = delete;
    Engine(const Engine &e) = delete;

public:
    Engine(int w, int h, float scale_factor);

    // mutators
    inline void prepareFrame();
    void update();
    inline void resize(int w, int h);
    inline void draw();

    void handleKeyboardState(const Uint8 * keyboard_state);
    void handleKeyPressEvents(SDL_Keycode k);
    void handleMouseEvent(const SDL_Event &event);

    void updateUIScaleFactor(float scale_factor);

    // getters
    inline gui::GUI &getGui() { return mGUI; }
    inline gfx::OpenGLRenderer &getRenderer() { return mRenderer; }

private:
    // helper methods...
    void registerEngineCallbacks();

};

// inline function definitions
void Engine::prepareFrame()
{
    mCameraController.clearSignals();
}

inline void Engine::resize(int w, int h)
{
    mRenderer.resize(w, h);
    mGUI.resize(w, h);
}

inline void Engine::draw()
{
    mRenderer.draw(mCamera, mGUI.getGUIRoot());
}


} // namespace Engine

#endif // ENGINE_H
