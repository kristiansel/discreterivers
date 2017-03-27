#include "engine.h"

#include "../createscene.h"
#include "../common/mathext.h"
#include "../common/macro/debuglog.h"
#include "../state/scenecreationinfo.h"

namespace engine {

Engine::Engine(int w, int h, float scale_factor) :
    mRenderer(w, h, scale_factor),
    mGUI(w, h, scale_factor),
    mWidth(w), mHeight(h),

    mClientStatePtr(nullptr),
    mNewGameInfoPtr(nullptr),

    mGUICapturedMouse(false)

{
    gui::createGUI(mGUI);

    registerEngineCallbacks();
}

Engine::~Engine()
{
    //delete mClientStatePtr; // not necessary with owning pointer...
}

void Engine::handleKeyboardState(const Uint8 *keyboard_state)
{
    if (mClientStatePtr)
    {
        // is there any point in trying to eliminate the branches here?
        // or is that taken care of by the optimizer anyway?
        if (keyboard_state[SDL_SCANCODE_LSHIFT])
        {
            mClientStatePtr->sendControlSignal(mech::CameraController::SpeedUp);
        }
        if (keyboard_state[SDL_SCANCODE_W])
        {
            mClientStatePtr->sendControlSignal(mech::CameraController::Forward);
        }
        if (keyboard_state[SDL_SCANCODE_S])
        {
            mClientStatePtr->sendControlSignal(mech::CameraController::Backward);
        }
        if (keyboard_state[SDL_SCANCODE_A])
        {
            mClientStatePtr->sendControlSignal(mech::CameraController::Left);
        }
        if (keyboard_state[SDL_SCANCODE_D])
        {
            mClientStatePtr->sendControlSignal(mech::CameraController::Right);
        }
        if (keyboard_state[SDL_SCANCODE_X])
        {
            mClientStatePtr->sendControlSignal(mech::CameraController::Down);
        }
        if (keyboard_state[SDL_SCANCODE_Z])
        {
            mClientStatePtr->sendControlSignal(mech::CameraController::Up);
        }
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
            if ((mMouseState.lmb_down || mMouseState.rmb_down) &&
                !mGUICapturedMouse && bool(mClientStatePtr))
            {

                float mouse_angle_x = static_cast<float>(mouse_delta_x)*0.0062832f; // 2π/1000?
                float mouse_angle_y = static_cast<float>(mouse_delta_y)*0.0062832f;

                mClientStatePtr->sendTurnSignals({mouse_angle_x, mouse_angle_y});
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
    if (mClientStatePtr)
    {
        mClientStatePtr->update(delta_time_sec);
    }
}

void Engine::updateUIScaleFactor(float scale_factor)
{
    mRenderer.updateUIScaleFactor(scale_factor);
    mGUI.updateUIScaleFactor(scale_factor);
}

void Engine::registerEngineCallbacks()
{
    DEBUG_LOG( "registering engine callbacks" );

    events::Immediate::add_callback<events::EndGameEvent>(
        [this] (const events::EndGameEvent &evt) {
            // nuke client state
            /*delete mClientStatePtr;
            mClientStatePtr = nullptr;*/
            mClientStatePtr = Ptr::OwningPtr<ClientState>(nullptr);
    });

//    // clear all on start new game...
    events::Immediate::add_callback<events::NewGameEvent>(
        [this] (const events::NewGameEvent &evt) {
            // first quit the old game
            events::Immediate::broadcast(events::EndGameEvent());

            // initialize the new game info object
            mNewGameInfoPtr = Ptr::OwningPtr<NewGameInfo>(new NewGameInfo());
    });

    events::Immediate::add_callback<events::CancelNewGameEvent>(
        [this] (const events::CancelNewGameEvent &evt) {
            // initialize the new game info object
            mNewGameInfoPtr = Ptr::OwningPtr<NewGameInfo>(nullptr); // is this even necessary? It gets recreated anyway...
    });


    // TODO: Refactor this from using events so extensively, to just be a bunch of stuff that happens in ClientState constructor...

    // load the world into the graphical scene when starting a new game
    events::Immediate::add_callback<events::StartGameEvent>(
        [this] (const events::StartGameEvent &evt) {
            // client state should be already added by callback above..... hope...
            // or not..
            // here shold move from new game into the client thigly

            // consume new game info ptr...

            Ptr::OwningPtr<state::MacroState> scene_data = mNewGameInfoPtr->moveMacroState();

            // find a point on land...
            int i_point = rand()%scene_data->alt_planet_points.size();
            while (scene_data->land_water_types[i_point] != AltPlanet::LandWaterType::Land)
            {
                i_point = rand()%scene_data->alt_planet_points.size();
            }
            vmath::Vector3 land_point = scene_data->alt_planet_points[i_point];

            // go a bit above that point
            vmath::Vector3 local_up = vmath::normalize(scene_data->planet_base_shape->getGradDir(land_point));
            vmath::Vector3 point_above = land_point + 4.0f * local_up;

            state::SceneCreationInfo scene_creation_info(std::move(scene_data), land_point, point_above);
            scene_creation_info.aspect_ratio = (float)(mWidth)/(float)(mHeight);
            scene_creation_info.actors.push_back({ point_above,
                                                   vmath::Quat(),
                                                   { state::Actor::Spec::Type::TestIco },
                                                   state::Actor::Control::Player } );

            // test actors
            vmath::Vector3 side_unit = vmath::cross(local_up, vmath::Vector3(0.0, 1.0, 0.0));
            scene_creation_info.actors.push_back({ point_above +  8.0f*side_unit,
                                                   vmath::Quat(),
                                                   { state::Actor::Spec::Type::TestBox },
                                                   state::Actor::Control::AI } );
            scene_creation_info.actors.push_back({ point_above + 16.0f*side_unit,
                                                   vmath::Quat(),
                                                   { state::Actor::Spec::Type::TestIco },
                                                   state::Actor::Control::AI} );

            // consume scene_creation_info
            mClientStatePtr = Ptr::OwningPtr<ClientState>(new ClientState(scene_creation_info));

            // end func
    });

}


} // namespace Engine
