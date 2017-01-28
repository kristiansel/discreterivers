#include "engine.h"

Engine::Engine(int w, int h, int dpi) :
    mRenderer(w, h),
    mGUI(w, h, dpi),

    // to be moved
    camera(w, h)

{
    gui::createGUI(mGUI);

    // to be moved
    camera.mTransform.position = vmath::Vector3(0.0, 0.0, 10.0);
}

void Engine::handleKeyDownEvent(SDL_Keycode k)
{
    switch(k)
    {
        case(SDLK_f):
            mRenderer.toggleWireframe();
            break;
        case(SDLK_q):
            events::Queued::emitEvent(events::Queued::QuitEvent());
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
        case(SDLK_r): {
            //SDL_SetWindowSize(mainWindow, 1500, 800);
            break;
        }
        case(SDLK_UP): {
            camera.mTransform.position -= vmath::Vector3(0.0f, 0.1f, 0.0f);
            break;
        }
        case(SDLK_DOWN): {
            camera.mTransform.position += vmath::Vector3(0.0f, 0.1f, 0.0f);
            break;
        }
        case(SDLK_RIGHT): {
            camera.mTransform.position -= vmath::Vector3(0.1f, 0.0f, 0.0f);
            break;
        }
        case(SDLK_LEFT): {
            camera.mTransform.position += vmath::Vector3(0.1f, 0.0f, 0.0f);
            break;
        }
        case (SDLK_F11): {
            //SDL_SetWindowFullscreen(mainWindow, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP );
            //fullscreen = !fullscreen;
            break;
        }
        case(SDLK_ESCAPE):
            // done = true;
            events::Immediate::broadcast(events::ToggleMainMenuEvent());
            break;
    }
}
