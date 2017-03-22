#include "engine.h"

#include "../createscene.h"
#include "../common/mathext.h"
#include "../common/macro/debuglog.h"
#include "../state/microstatecreationinfo.h"

namespace engine {

Engine::Engine(int w, int h, float scale_factor) :
    mGameState(),
    mRenderer(w, h, scale_factor),
    mGUI(w, h, scale_factor),

    // to be moved
    mGFXSceneManager(gfx::Camera(gfx::PerspectiveProjection((float)(w)/(float)(h), DR_M_PI_4, 0.0f, 1000000.0f)),
                     gfx::SceneNode(),
                     Ptr::WritePtr<PhysTransformContainer>(&mActorTransforms)),
    mMechanicsManager(&mGFXSceneManager.mCamera),
    mPhysicsManager(Ptr::WritePtr<PhysTransformContainer>(&mActorTransforms)),
    mGUICapturedMouse(false)

{
    gui::createGUI(mGUI);

    // make sure to resize everything
    // mGUI.resize(w, h);

    registerEngineCallbacks();

    // to be moved
    mGFXSceneManager.mCamera.mTransform.position = vmath::Vector3(0.0, 0.0, 10.0);
}


void Engine::handleKeyboardState(const Uint8 *keyboard_state)
{
    // is there any point in trying to eliminate the branches here?
    // or is that taken care of by the optimizer anyway?
    if (keyboard_state[SDL_SCANCODE_LSHIFT])
    {
        mMechanicsManager.getActiveController()->sendSignal(mech::CameraController::SpeedUp);
    }
    if (keyboard_state[SDL_SCANCODE_W])
    {
        mMechanicsManager.getActiveController()->sendSignal(mech::CameraController::Forward);
    }
    if (keyboard_state[SDL_SCANCODE_S])
    {
        mMechanicsManager.getActiveController()->sendSignal(mech::CameraController::Backward);
    }
    if (keyboard_state[SDL_SCANCODE_A])
    {
        mMechanicsManager.getActiveController()->sendSignal(mech::CameraController::Left);
    }
    if (keyboard_state[SDL_SCANCODE_D])
    {
        mMechanicsManager.getActiveController()->sendSignal(mech::CameraController::Right);
    }
    if (keyboard_state[SDL_SCANCODE_X])
    {
        mMechanicsManager.getActiveController()->sendSignal(mech::CameraController::Down);
    }
    if (keyboard_state[SDL_SCANCODE_Z])
    {
        mMechanicsManager.getActiveController()->sendSignal(mech::CameraController::Up);
    }
}

void Engine::handleKeyPressEvents(SDL_Keycode k)
{
    // forward events to GUI
    mGUI.handleKeyPressEvent(k);

    // handle global key events
    switch(k)
    {
    case(SDLK_f):
        mRenderer.toggleWireframe();
        break;
    case(SDLK_h):
        //alt_planet_triangles_so->toggleVisible();
        break;
    case(SDLK_u): {
        // do an iteration of repulsion
//                                AltPlanet::pointsRepulse(alt_planet_points, planet_shape, 0.003f);

//                                // update the scene object geometry
//                                std::vector<vmath::Vector4> position_data;
//                                std::vector<gfx::Point> primitives_data;

//                                for (int i = 0; i < alt_planet_points.size(); i++)
//                                {
//                                    position_data.push_back((const vmath::Vector4&)(alt_planet_points[i]));
//                                    position_data.back().setW(1.0f);
//                                    primitives_data.push_back({i});
//                                }

//                                gfx::Vertices vertices = gfx::Vertices(position_data, position_data /*, texcoords*/);
//                                gfx::Primitives primitives = gfx::Primitives(primitives_data);

//                                alt_planet_points_so->mGeometry = gfx::Geometry(vertices, primitives);
        break;
    }
    case (SDLK_F11): {
        events::Deferred::emitEvent(events::Deferred::ToggleFullscreenEvent());
        break;
    }
    case(SDLK_ESCAPE):
        // done = true;
        events::Immediate::broadcast(events::ToggleMainMenuEvent());
        break;
    case(SDLK_PLUS):
    case(SDLK_KP_PLUS):
        events::Deferred::emitEvent(events::Deferred::IncrUIScaleFactor());
        break;
    case(SDLK_MINUS):
    case(SDLK_KP_MINUS):
        events::Deferred::emitEvent(events::Deferred::DecrUIScaleFactor());
        break;
    } // switch k
}

void Engine::handleMouseEvent(const SDL_Event &event)
{
    switch(event.type)
    {
        case SDL_MOUSEBUTTONDOWN: {
            mMouseState.lmb_down = event.button.button == SDL_BUTTON_LEFT;
            mMouseState.rmb_down = event.button.button == SDL_BUTTON_RIGHT;
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                mGUICapturedMouse = mGUI.handleMouseButtonDown(event.button.x, event.button.y, gfx::gui::MouseButton::Left);
                //std::cout << "mouse captured: " << mGUICapturedMouse << std::endl;
            }
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                mGUI.handleMouseButtonUp(event.button.x, event.button.y, gfx::gui::MouseButton::Left);
            }
            mMouseState.lmb_down = event.button.button == SDL_BUTTON_LEFT ? false : mMouseState.lmb_down;
            mMouseState.rmb_down = event.button.button == SDL_BUTTON_RIGHT ? false : mMouseState.rmb_down;
            mGUICapturedMouse = false;
            break;
        }
        case SDL_MOUSEMOTION: {
            int32_t mouse_delta_x = mMouseState.prev_mouse_x - event.motion.x;
            int32_t mouse_delta_y = mMouseState.prev_mouse_y - event.motion.y;
            if ((mMouseState.lmb_down || mMouseState.rmb_down) && !mGUICapturedMouse) {

                float mouse_angle_x = static_cast<float>(mouse_delta_x)*0.0062832f; // 2π/1000?
                float mouse_angle_y = static_cast<float>(mouse_delta_y)*0.0062832f;

                mMechanicsManager.getActiveController()->sendTurnSignals({mouse_angle_x, mouse_angle_y});
            }

            mGUI.handleMouseMoved(event.motion.x, event.motion.y, mouse_delta_x, mouse_delta_y);

            // update previous mouse position
            mMouseState.prev_mouse_x = event.motion.x;
            mMouseState.prev_mouse_y = event.motion.y;
            break;
        }
        case SDL_MOUSEWHEEL: {
            mGUI.handleMouseWheelScroll(event.wheel.y);
            break;
        }
    }
}

void Engine::update(float delta_time_sec)
{
    // update mechanics
    mMechanicsManager.update();

    // update physics
    mPhysicsManager.stepPhysicsSimulation(delta_time_sec);

    // update render jobs
    mGFXSceneManager.updateGraphicsTransforms();
}

void Engine::updateUIScaleFactor(float scale_factor)
{
    mRenderer.updateUIScaleFactor(scale_factor);
    mGUI.updateUIScaleFactor(scale_factor);
}

void Engine::registerEngineCallbacks()
{
    DEBUG_LOG( "registering engine callbacks" );

    // clear all on start new game...
    events::Immediate::add_callback<events::NewGameEvent>(
        [this] (const events::NewGameEvent &evt) {
            // should just have to nuke the game state...?
            mGFXSceneManager.mGFXSceneRoot.clearAll();
    });

    // load the world into the graphical scene when starting a new game
    events::Immediate::add_callback<events::StartGameEvent>(
        [this] (const events::StartGameEvent &evt) {
            Ptr::ReadPtr<state::MacroState> scene_data = mGameState.readMacroState();


            // find a point on land...
            int i_point = rand()%scene_data->alt_planet_points.size();
            while (scene_data->land_water_types[i_point] != AltPlanet::LandWaterType::Land)
            {
                i_point = rand()%scene_data->alt_planet_points.size();
            }
            vmath::Vector3 land_point = scene_data->alt_planet_points[i_point];

            // go a bit above that point
            vmath::Vector3 local_up = vmath::normalize(scene_data->planet_base_shape->getGradDir(land_point));
            vmath::Vector3 point_above = land_point + 15.0f * local_up;

            unsigned int player_actor_id = 1;
            state::MicroStateCreationInfo scene_creation_info = {
                scene_data, // macro state ptr
                land_point, // land position
                point_above, // anchor_pos
                {
                    { player_actor_id, point_above, vmath::Quat() },
                   /* other actors */
                }
            };

            events::Immediate::broadcast(events::CreateSceneEvent{&scene_creation_info});

            //mMechanicsManager.wireControlsToCamera(mGFXSceneManager.getCamera());

            // now everything is created...
            // wire up some connections?


            // end func
    });

}


} // namespace Engine
