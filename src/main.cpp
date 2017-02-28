#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>

#define _VECTORMATH_DEBUG

#include "common/macro/macroprofile.h"
#include "system/async.h"
#include "common/serialize.h"
#include "graphics/openglrenderer.h"
#include "gui/gui.h"
#include "events/queuedevents.h"
#include "events/immediateevents.h"
#include "engine/engine.h"
#include "engine/fpscounter.h"
#include "createscene.h"
#include "appconstraints.h"

// to be removed
#include "common/flags.h"
#include "common/threads/threadpool.h"
/*#include "common/expr/expr.h"

struct tag
{
    struct pos_attr   {};
    struct texco_attr {};
    struct mv_uni  {};
    struct tex_uni {};
    struct co_uni  {};
    struct ad_uni  {};
};*/

#include "common/shaderexpressions/shxexpr.h"
#include "common/shaderexpressions/shxdebugoutput.h"
#include "common/shaderexpressions/shxopengl.h"

/*"layout(location = 0) in vec4 vertex_position;"
"layout(location = 1) in vec2 vertex_tex_coords;"

"out vec2 tex_coords;"
"uniform mat4 mv;"

"void main() {"
"  tex_coords = vertex_tex_coords;"
"  gl_Position = mv * vertex_position;"
"}";

const char * fragment_shader_src =
"#version 410\n"

"in vec2 tex_coords;"

"out vec4 frag_color;"

"uniform sampler2D tex;"
"uniform vec4 color;"

"void main() {"
"  frag_color = vec4(1, 1, 1, texture(tex, tex_coords).r) * color;"
"}";*/


namespace vmath = Vectormath::Aos;

int main(int argc, char *argv[])
{
    //= o o o =========================//
    //         SDL2 Window code        //
    //=================================//
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // get main screen resolution and dpi
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

    //float scale_factor = (float)(maindisp_height)/(float)(512);
    //float scale_factor = (float)(maindisp_height)/(float)(768);
    float scale_factor = (float)(maindisp_height)/(float)(1024);
    //float scale_factor = (float)(maindisp_height)/(float)(2024);
    int width = scale_factor * 1400;
    int height = scale_factor * 800;

    // initialize and create window
    Uint32 flags = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;

    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // hint
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1); // hint
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    //int width = 2800;    int height = 1600;
    SDL_Window * mainWindow = SDL_CreateWindow("Discrete rivers", // window name
                                               SDL_WINDOWPOS_UNDEFINED, // windowpos x
                                               SDL_WINDOWPOS_UNDEFINED, // windowpos y
                                               width, height, flags);

    SDL_SetWindowMinimumSize(mainWindow, scale_factor * appconstraints::min_window_width_abs,
                                         scale_factor * appconstraints::min_window_height_abs);

    SDL_GLContext mainGLContext = SDL_GL_CreateContext(mainWindow);

    std::cout << "Checking SDL error: " << SDL_GetError() << std::endl;
    std::cout << "Found graphics card: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;

    // stencil debugging
    int stencil_size;
    int sdl_get_ret = SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil_size);
    std::cout << "SDL2 stencil buffer bit depth: " << stencil_size << std::endl;
    std::cout << "sdl_get_ret: " << sdl_get_ret << std::endl;

    //=================================//
    //     Initialize game engine      //
    //=================================//
    engine::Engine engine(width, height, scale_factor);

    // test stuff====================================================================================================================
    // vertex shader
    shx::attribute<shx::vec4_t > pos;
    shx::attribute<shx::vec2_t > texco;
    shx::uniform  <shx::mat4_t > mv;
    //shx::uniform  <shx::mat3_t > m2;
    //shx::uniform  <shx::mat3_t > m3;
    //shx::uniform  <shx::tex2d_t> t;

    shx::expr<shx::vec4_t > pos_out = mv * pos;
    shx::expr<shx::vec2_t > texco_out = texco;
    //shx::expr<shx::float_t> test2_out = shx::expr<shx::vec4_t>(pos).x() + shx::expr<shx::vec4_t>(pos).g() + texture(t, texco).q();

    auto vert_shdr = shx::opengl::make_vertex_shader( pos_out, texco_out/*, test2_out*/);

    // fragment shader
    shx::uniform<shx::tex2d_t> tex;
    shx::uniform<shx::vec4_t > color;

    shx::expr<shx::vec4_t >  tex_sample   = shx::texture(tex, texco);
    shx::expr<shx::float_t>  tex_sample_r = tex_sample.r();
    shx::expr<shx::vec4_t >  col_out      = shx::vec4(1.0f, 1.0f, 1.0f, tex_sample_r) * color;

    //auto vert_shdr = shdr::make_fragment_shader( col_out, texco ); // first arg is frag color, rest are "in" parameters

    /*uniform<stex2d, tag::tex_uni> tex_uni;
    uniform<svec4,  tag::co_uni > col_uni;

    auto tex   = make_expr(tex_uni);
    auto col   = make_expr(col_uni);

    auto lit1  = lit(1);

    auto tex_sample   = texture(tex, texco);
    auto tex_sample_r = tex_sample.r();
    auto alpha_white  = make_vec4(lit1, lit1, lit1, tex_sample_r);
    //auto col_out      = alpha_white * col;

    std::string expr_str = get_expr_string(alpha_white);
    std::cout << "expression reads: " << std::endl;
    std::cout << expr_str << std::endl;*/

    // test expr
    /*attrib <svec4, tag::pos_attr  > pos_attr;
    attrib <svec2, tag::texco_attr> texco_attr;
    uniform<smat4, tag::mv_uni    > mv_uni;
    uniform<svec4, tag::ad_uni    > ad_uni;

    auto pos   = make_expr(pos_attr);
    auto texco = make_expr(texco_attr);
    auto mv    = make_expr(mv_uni);
    auto ad    = make_expr(ad_uni);

    expr<svec4,
         uniforms<
            uniform<svec4, tag::ad_uni>,
            uniform<smat4, tag::mv_uni> >,
         attribs <
            attrib <svec4, tag::pos_attr> > >
    pos_out = mv * mv * pos + ad;

    std::string expr_str1 = get_expr_string(pos_out);
    std::cout << "expression1 reads: " << std::endl;
    std::cout << expr_str1 << std::endl;

    //auto vert_shdr = shdr::make_vertex_shader( pos_out, texco );

    uniform<stex2d, tag::tex_uni> tex_uni;
    uniform<svec4,  tag::co_uni > col_uni;

    auto tex   = make_expr(tex_uni);
    auto col   = make_expr(col_uni);

    auto lit1  = lit(1);

    auto tex_sample   = texture(tex, texco);
    auto tex_sample_r = tex_sample.r();
    auto alpha_white  = make_vec4(lit1, lit1, lit1, tex_sample_r);
    //auto col_out      = alpha_white * col;

    std::string expr_str = get_expr_string(alpha_white);
    std::cout << "expression reads: " << std::endl;
    std::cout << expr_str << std::endl;

    //auto frag_shdr  = shdr::make_fragment_shader( col_out );*/

    // finish test ====================================================================================================================

    bool valgrind_test;
    if (valgrind_test) std::cout << "valgrind better catch this...." << std::endl; // and it does, well done!

    // SDL event loop
    SDL_Event event;
    bool done = false;
    bool fullscreen = false;

    float filter_weight = 0.03f;
    engine::FPSCounter fps_counter(filter_weight);

    //=================================//
    //         Main loop               //
    //=================================//
    while(!done)
    {
        fps_counter.startFrame();

        bool resizing_this_frame = false;

        // pre-event work
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
                //if (event.key.repeat == 0)
                engine.handleKeyPressEvents(event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:   // nice
            case SDL_MOUSEBUTTONUP:     // case
            case SDL_MOUSEMOTION:       // fall
            case SDL_MOUSEWHEEL:        // thru
                engine.handleMouseEvent(event);
                break;
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED: // This is the most general resize event
                    {
                        // WOW, this event even triggers on startup :)
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

        //std::cout << "got past input handling" << std::endl;

        // update based on events
        engine.update();


        //std::cout << "got past engine update" << std::endl;

        // Interaction with gui and other parts of the system might have caused events
        // related to SDL and the window system. These events are processed here...
        events::Deferred::EventQueue &evt_queue = events::Deferred::getEventQueue();
        while (!evt_queue.empty())
        {
            events::Deferred::Event &evt = evt_queue.front();
            // do something with evt
            switch(evt.get_type())
            {
            case (events::Deferred::Event::is_a<events::Deferred::QuitEvent>::value):
                {
                    done = true;
                }
                break;
            case (events::Deferred::Event::is_a<events::Deferred::ToggleFullscreenEvent>::value):
                {
                    SDL_SetWindowFullscreen(mainWindow, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP );
                    fullscreen = !fullscreen;
                }
                break;
            case (events::Deferred::Event::is_a<events::Deferred::IncrUIScaleFactor>::value):
                {
                    scale_factor = std::min(scale_factor+0.1f, appconstraints::max_ui_scale_factor);
                    std::cout << "scale_factor = " << scale_factor << std::endl;
                    engine.updateUIScaleFactor(scale_factor);
                }
                break;
            case (events::Deferred::Event::is_a<events::Deferred::DecrUIScaleFactor>::value):
                {
                    scale_factor = std::max(scale_factor-0.1f, appconstraints::min_ui_scale_factor);
                    std::cout << "scale_factor = " << scale_factor << std::endl;
                    engine.updateUIScaleFactor(scale_factor);
                }
                break;
            default:
                break;

            }

            evt_queue.pop();
        }

        //std::cout << "got past late events" << std::endl;

        // User interaction might have caused asynchronous jobs to spawn in separate threads.
        // When these jobs are complete, final processing is done here in the main thread
        sys::Async::processReturnedJobs();

        if (!resizing_this_frame)
        {
            //=================================//
            //              DRAW               //
            //=================================//
            engine.draw();

            //std::cout << "got past draw" << std::endl;

            // end of meaningfull work, measure the FPS
            float fps_filtered_val = fps_counter.getFrameFPSFiltered();
            events::Immediate::broadcast(events::FPSUpdateEvent{fps_filtered_val});

            // OOPS! This function call can sleep the main thread
            SDL_GL_SwapWindow(mainWindow);
        }

        //std::cout << "got past swap window" << std::endl;

    } // End while(!done) // main loop

    //=================================//
    //              QUIT               //
    //=================================//

    gfx::checkOpenGLErrors("program end");

    SDL_GL_DeleteContext(mainGLContext);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();

    return 0;
}
