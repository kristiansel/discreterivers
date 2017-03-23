#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>
#include "../gui/gui.h"
#include "../graphics/openglrenderer.h"
#include "../events/immediateevents.h"
#include "../events/queuedevents.h"
#include "../state/gamestate.h"
#include "../mechanics/mechanicsmanager.h"
#include "../physics/physicsmanager.h"
#include "mousestate.h"
#include "gfxscenemanager.h"
#include "phystransformcontainer.h"

// organize window/input handling under system..?

// engine marshals input around and handles cross-system communication?

namespace engine {

class Engine
{
    // big members
    state::GameState    mGameState;
    gfx::OpenGLRenderer mRenderer;              // must be initialized before GUI!
    gui::GUI            mGUI;                   // needs valid opengl context and glew init

    struct ClientState
    {
        ClientState(int w, int h);
        // to be moved into graphics state manager
        GFXSceneManager mGFXSceneManager;

        // intermediary between physics, control and graphics
        PhysTransformContainer mActorTransforms;
        PhysTransformContainer mKinematicTransforms; // camera etc, not physics controlled

        // to be moved into mechanics state manager
        MechanicsManager mMechanicsManager;

        // physics
        PhysicsManager mPhysicsManager;
    };

    ClientState * mClientStatePtr;

    // what is this used for, why does it not reside in GUI?
    bool mGUICapturedMouse;
    MouseState mMouseState; // should be in some sort of input handler...

    // wtf is this here for...
    int mWidth;
    int mHeight;

    // immovable, non-copyable, non-default-constructible
    Engine() = delete;
    Engine(Engine &&e) = delete;
    Engine(const Engine &e) = delete;

public:
    Engine(int w, int h, float scale_factor);

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
        mRenderer.draw(mClientStatePtr->mGFXSceneManager.mCamera,
                       mGUI.getGUIRoot(),
                       mClientStatePtr->mGFXSceneManager.mGFXSceneRoot);
    }
    else
    {
        mRenderer.drawGUIOnly(mGUI.getGUIRoot());
    }
}


} // namespace Engine

#endif // ENGINE_H
