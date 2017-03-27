#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>
#include "../gui/gui.h"
#include "../graphics/openglrenderer.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"
#include "mousestate.h"
#include "newgameinfo.h"
#include "clientstate.h"

// organize window/input handling under system..?

// engine marshals input around and handles cross-system communication?

namespace engine {

class Engine
{
    // big members
    //state::GameState    mGameState;
    gfx::OpenGLRenderer mRenderer;              // must be initialized before GUI!
    gui::GUI            mGUI;                   // needs valid opengl context and glew init

    Ptr::OwningPtr<ClientState> mClientStatePtr;

    // new game info ptr?
    Ptr::OwningPtr<NewGameInfo> mNewGameInfoPtr;

    // what is this used for, why does it not reside in GUI?
    bool mGUICapturedMouse;
    MouseState mMouseState; // should be in some sort of input handler...

    // wtf is this here for...
    int mWidth;
    int mHeight;

public:
    Engine(int w, int h, float scale_factor);
    ~Engine();

    // mutators
    //inline void prepareFrame();
    void update(float delta_time_sec);
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

    // immovable, non-copyable, non-default-constructible
    Engine() = delete;
    Engine(Engine &&e) = delete;
    Engine(const Engine &e) = delete;


};

inline void Engine::resize(int w, int h)
{
    mRenderer.resize(w, h);
    mGUI.resize(w, h);
    mWidth = w;
    mHeight = h;
}

inline void Engine::draw()
{
    if (mClientStatePtr)
    {
        /*mRenderer.draw(mClientStatePtr->mGFXSceneManager.mCamera,
                       mGUI.getGUIRoot(),
                       mClientStatePtr->mGFXSceneManager.mGFXSceneRoot);*/

        mRenderer.draw(mClientStatePtr->getActiveCamera(),
                       mGUI.getGUIRoot(),
                       mClientStatePtr->getGFXSceneRoot());
    }
    else
    {
        mRenderer.drawGUIOnly(mGUI.getGUIRoot());
    }
}


} // namespace Engine

#endif // ENGINE_H
