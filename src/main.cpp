#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>

#define _VECTORMATH_DEBUG

#include "common/macro/macroprofile.h"
#include "common/serialize.h"
#include "graphics/openglrenderer.h"
#include "gui/gui.h"
#include "events/queuedevents.h"
#include "events/immediateevents.h"
#include "engine/engine.h"
#include "engine/fpscounter.h"
#include "createscene.h"

// to be removed
#include "common/flags.h"

namespace vmath = Vectormath::Aos;

int main(int argc, char *argv[])
{

    SceneData scene_data = createPlanetData();

    // SDL2 window code
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // get main window resolution and dpi
    int maindisp_width, maindisp_height, maindisp_dpi;
    if (SDL_GetNumVideoDisplays() > 0)
    {
        int disp_index = 0;
        SDL_DisplayMode disp_mode;
        if (SDL_GetCurrentDisplayMode(disp_index, &disp_mode) != 0)
        {
            std::cerr << "Could not get display mode for video display " << disp_index << ": " << SDL_GetError() << std::endl;
            return 1;
        }
        else
        {
            float ddpi, hdpi, vdpi;
            if(SDL_GetDisplayDPI(disp_index, &ddpi, &hdpi, &vdpi) != 0)
            {
                std::cerr << "Could not get display dpi for video display " << disp_index << ": " << SDL_GetError() << std::endl;
                return 1;
            }
            else
            {
                maindisp_width = disp_mode.w;
                maindisp_height = disp_mode.h;
                maindisp_dpi = ddpi; // implicitly rounded
            }
        }
    }
    else
    {
        std::cerr << "no displays detected, aborting." << std::endl;
        return 1;
    }

    int width = std::min(maindisp_width, maindisp_dpi * 14);
    int height = std::min(maindisp_height, maindisp_dpi * 10);
    int dpi = maindisp_dpi;

    Uint32 flags = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;

    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // hint
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2); // hint*/
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    //int width = 2800;    int height = 1600;
    SDL_Window * mainWindow = SDL_CreateWindow("Discrete rivers", // window name
                                               SDL_WINDOWPOS_UNDEFINED, // windowpos x
                                               SDL_WINDOWPOS_UNDEFINED, // windowpos y
                                               width, height, flags);

    SDL_GLContext mainGLContext = SDL_GL_CreateContext(mainWindow);

    // DPI settings
    //int current_display = SDL_GetWindowDisplayIndex(mainWindow);

    std::cout << "Checking SDL error: " << SDL_GetError() << std::endl;

    std::cout << "Found graphics card: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;

    // stencil debugging
    int stencil_size;
    int sdl_get_ret = SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil_size);
    std::cout << "SDL2 stencil buffer bit depth: " << stencil_size << std::endl;
    std::cout << "sdl_get_ret: " << sdl_get_ret << std::endl;

    //=================================//
    //           Initialize            //
    //=================================//
    engine::Engine engine(width, height, dpi);

    // create a scene
    createScene(engine.getRenderer(), scene_data);

    // SDL event loop
    SDL_Event event;
    bool done = false;

    // test mouse
    bool fullscreen = false;

    // fps counter
    float filter_weight = 0.03f;
    engine::FPSCounter fps_counter(filter_weight);

    while(!done)
    {
        // start timer for fps counting
        fps_counter.startFrame();

        bool resizing_this_frame = false;

        //=================================//
        //             PREPARE             //
        //=================================//
        engine.prepareFrame();

        // handle continuous events
        SDL_PumpEvents();
        const Uint8 * keyboard_state = SDL_GetKeyboardState(nullptr);
        engine.handleKeyboardState(keyboard_state);

        // handle discrete events
        while( SDL_PollEvent(&event) )
        {
            switch(event.type)
            {
            case SDL_KEYDOWN:
                if (event.key.repeat == 0) engine.handleKeyPressEvents(event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:   // nice
            case SDL_MOUSEBUTTONUP:     // case
            case SDL_MOUSEMOTION:       // fall
            case SDL_MOUSEWHEEL:        // thru
                engine.handleMouseEvent(event);
                break;
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED: { // This is the most general resize event
                        resizing_this_frame = true;
                        int resize_width = event.window.data1;
                        int resize_height = event.window.data2;

                        engine.resize(resize_width, resize_height);
                        break;
                    }
                } // switch (event.window.event)
                break;
            } // case SDL_WINDOWEVENT:
            case SDL_QUIT:
                done = true;
                break;
            default:
                break;
            }   // End switch
        } // while(SDL_PollEvent(&event)))

        //=================================//
        //             UPDATE              //
        //=================================//
        engine.update();

        // interaction with gui and other parts of the system might have caused events
        // related to SDL and the window system. These events are processed here...
        events::Queued::EventQueue &evt_queue = events::Queued::getEventQueue();
        while (!evt_queue.empty())
        {
            events::Queued::Event &evt = evt_queue.front();
            // do something with evt
            switch(evt.get_type())
            {
            case (events::Queued::Event::is_a<events::Queued::QuitEvent>::value):
                {
                    done = true;
                    break;
                }
            case (events::Queued::Event::is_a<events::Queued::ToggleFullscreenEvent>::value):
                {
                    SDL_SetWindowFullscreen(mainWindow, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP );
                    fullscreen = !fullscreen;
                    break;
                }
            default:
                {
                    break;
                }
            }

            evt_queue.pop();
        }


        if (!resizing_this_frame)
        {
            //=================================//
            //              DRAW               //
            //=================================//
            engine.draw();

            // end of work
            float fps_filtered_val = fps_counter.getFrameFPSFiltered();

            // TODO: Implement a monospace text element that sends updated text as texture coordinate updates
            // either that, or some dynamic text element, that just updates the buffer data in the gui, text element
            // or both...

            events::Immediate::broadcast(events::FPSUpdateEvent{fps_filtered_val});

            SDL_GL_SwapWindow(mainWindow); // woops: This sleeps!
        }

    }   // End while(!done)


    //=================================//
    //              QUIT               //
    //=================================//
    gfx::checkOpenGLErrors("program end");

    // safe to clean up these before destructors are called?
    SDL_GL_DeleteContext(mainGLContext);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();

    return 0;
}
