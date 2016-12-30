#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>

#define _VECTORMATH_DEBUG

#include "altplanet/altplanet.h"
#include "altplanet/watersystem.h"
#include "altplanet/climate/irradiance.h"
#include "altplanet/climate/humidity.h"
#include "altplanet/climate/climate.h"
#include "common/macro/macroprofile.h"
#include "common/gfx_primitives.h"
#include "common/serialize.h"
#include "graphics/openglrenderer.h"
#include "graphics/gui/guifontrenderer.h"


// point3 is not what is needed here. vector4 is the only one for rendering
namespace vmath = Vectormath::Aos;

int main(int argc, char *argv[])
{
    //AltPlanet::Shape::Disk disk(3.0f);
    AltPlanet::Shape::Sphere sphere(3.0f);
    AltPlanet::Shape::Torus torus(3.0f, 1.0f);
    AltPlanet::Shape::BaseShape &planet_shape = torus;

#ifdef PROFILE
    for (int num_pts = 500; num_pts < 4000; num_pts += 500)
    {
        std::cout << "generating with " << num_pts << " points" << std::endl;

        PROFILE_BEGIN(altplanet_generate);
        auto dummy = AltPlanet::generate(num_pts, planet_shape);
        PROFILE_END(altplanet_generate);
    }
#endif
    // Alt planet
    //std::string planet_filename = "torus_planet.dat";

    // // try to open planet file
    // std::ifstream file(planet_filename, std::ios::binary);
    // bool loading_went_bad = false;
    // if (file.is_open()) {
    //     // load planet from file
    //     try {
    //         std::cout << "loading planet file: " << planet_filename << std::endl;
    //         Serial::StreamType resin = Serial::read_from_file(planet_filename);
    //         alt_planet_geometry = Serial::deserialize<AltPlanet::PlanetGeometry>(resin);
    //     } catch (...) {
    //         loading_went_bad = true;
    //         std::cout << "something went wrong while trying to load " << planet_filename << std::endl;
    //     }
    // }
    // else
    // {
    //     std::cout << "could not open file " << planet_filename << std::endl;
    //     loading_went_bad = true;
    // }

    // if (loading_went_bad)
    // {
    //     std::cout << "generating planet geometry" << std::endl;
    //     // create the planet

    //     // Generate geometry
    //     alt_planet_geometry = AltPlanet::generate(3000, planet_shape);

    //     // Serialize it
    //     try {
    //         Serial::serialize_to_file(alt_planet_geometry, planet_filename);
    //     } catch (...) {
    //         std::cout << "something went wrong while trying to serialize to " << planet_filename << std::endl;
    //     }
    // }

    // generate planet
    AltPlanet::PlanetGeometry alt_planet_geometry = AltPlanet::generate(3000, planet_shape);

    std::vector<vmath::Vector3> &alt_planet_points = alt_planet_geometry.points;
    std::vector<gfx::Triangle> &alt_planet_triangles = alt_planet_geometry.triangles;

    // Generate the planet water system
    float planet_ocean_fraction = 0.55f;
    int num_river_springs = 150;
    auto water_geometry = AltPlanet::WaterSystem::generateWaterSystem(alt_planet_geometry, planet_shape,
                                                                      planet_ocean_fraction,
                                                                      num_river_springs);

    // Deconstruct the ocean geometry
    std::vector<vmath::Vector3> &alt_ocean_points = water_geometry.ocean.points;
    std::vector<gfx::Triangle> &alt_ocean_triangles = water_geometry.ocean.triangles;

    // Deconstruct freshwater geometry
    std::vector<vmath::Vector3> &alt_lake_points = water_geometry.freshwater.lakes.points;
    std::vector<gfx::Triangle> &alt_lake_triangles = water_geometry.freshwater.lakes.triangles;
    std::vector<gfx::Line> &alt_river_lines = water_geometry.freshwater.rivers.lines;

    // Generate planet irradiance map
    std::vector<vmath::Vector3> alt_planet_normals;
    gfx::generateNormals(&alt_planet_normals, alt_planet_points, alt_planet_triangles);

    float planet_tilt = 0.408407f; // radians, same as earth
    std::vector<float> alt_planet_irradiance = AltPlanet::Irradiance::irradianceYearMean(
        alt_planet_points,
        alt_planet_normals,
        alt_planet_triangles,
        planet_tilt);

    // check the irradiance
    float max = std::numeric_limits<float>::min();
    float min = std::numeric_limits<float>::max();
    float mean = 0.f;
    for (const auto &el : alt_planet_irradiance)
    {
        if (el < min) min=el;
        if (el > max) max=el;
        mean += el/alt_planet_irradiance.size();
    }

    std::cout << "irradiance info" << min << ", " << mean << ", " << max << std::endl;

    // planet humidity
    std::vector<float> alt_planet_humidity = AltPlanet::Humidity::humidityYearMean(alt_planet_points, planet_shape);


    // SDL2 window code:
    Uint32 flags = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;
    //int width = 1000;
    //int height = 800;

    int width = 2800;    int height = 1600;

    SDL_Window * mainWindow = SDL_CreateWindow("Discrete rivers", // window name
                                               SDL_WINDOWPOS_UNDEFINED, // windowpos x
                                               SDL_WINDOWPOS_UNDEFINED, // windowpos y
                                               width, height, flags);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // hint
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1); // hint
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GLContext mainGLContext = SDL_GL_CreateContext(mainWindow);

    std::cout << "Found graphics card: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;


    // Camera
    gfx::Camera camera(width, height);
    camera.mTransform.position = vmath::Vector3(0.0, 0.0, 10.0);

    // Opengl renderer
    gfx::OpenGLRenderer opengl_renderer(width, height);

    // add some gui
    vmath::Vector4 color_gui_base = vmath::Vector4{0.06, 0.09, 0.12, 1.0};


    gfx::gui::GUIFontRenderer guiFontRenderer("res/fonts/IMFePIrm28P.ttf");

    gfx::Texture font_atlas_tex = guiFontRenderer.getTextureAtlas();

    gfx::Texture earth_tex = gfx::Texture("res/textures/earthlike.png");

    opengl_renderer.addGUINode(
        vmath::Vector4(color_gui_base),
        gfx::gui::GUITransform({0.33f, 0.33f}, {0.5f, 0.5f}),
        "",
        {
            gfx::gui::GUINode(
                vmath::Vector4{1.0, 1.0, 1.0, 1.0},
                gfx::gui::GUITransform({0.1f, 0.1f}, {0.8f, 0.8f}),
                "The Quick Brown Fox Jumps Over The Lazy Dog", {}, earth_tex
            ),
        }
    );

    opengl_renderer.addGUINode(
                vmath::Vector4(2.0f*color_gui_base),
                gfx::gui::GUITransform({0.15f, 0.15f}, {0.25f, 0.25f}),
                "", {}, font_atlas_tex);




    // create a scene graph node for a light
    gfx::SceneNodeHandle light_scene_node = opengl_renderer.addSceneNode();
    gfx::LightHandle light = ([](const gfx::SceneNodeHandle &scene_node)
                              {
                                  vmath::Vector4 color(1.0f, 1.0f, 1.0f, 1.0f);
                                  gfx::Transform transform;
                                  transform.position = vmath::Vector3(10.0f, 10.0f, 10.0f);

                                  return scene_node->addLight(color, transform);
                              })(light_scene_node);

    gfx::SceneNodeHandle planet_scene_node = opengl_renderer.addSceneNode();

    // Create some alt planet vertex data to share
    std::vector<vmath::Vector4> alt_planet_position_data;
    std::vector<gfx::Point> alt_planet_point_primitives_data;

    for (int i = 0; i<alt_planet_points.size(); i++)
    {
        alt_planet_position_data.push_back((const vmath::Vector4&)(alt_planet_points[i]));
        alt_planet_position_data.back().setW(1.0f);
        alt_planet_point_primitives_data.push_back({i});
    }

    std::vector<vmath::Vector4> alt_planet_normal_data;
    gfx::generateNormals(&alt_planet_normal_data, alt_planet_position_data, alt_planet_triangles);

    std::vector<gfx::TexCoords> alt_planet_texcoords = planet_shape.getUV(alt_planet_points);
    gfx::Vertices alt_planet_vertices = gfx::Vertices(alt_planet_position_data, alt_planet_normal_data, alt_planet_texcoords);

    // Planet point data scene object
    gfx::SceneObjectHandle alt_planet_points_so = ([&]()
                                                   {
                                                       gfx::Primitives primitives = gfx::Primitives(alt_planet_point_primitives_data);
                                                       gfx::Geometry geometry = gfx::Geometry(alt_planet_vertices, primitives);

                                                       vmath::Vector4 color(1.0f, 0.0f, 0.0f, 1.0f);
                                                       gfx::Material material = gfx::Material(color);

                                                       // material.setWireframe(true);

                                                       gfx::Transform transform;
                                                       transform.scale = vmath::Vector3(1.0008f, 1.0008f, 1.0008f);
                                                       return planet_scene_node->addSceneObject(geometry, material, transform);
                                                   })(); // immediately invoked lambda!

    alt_planet_points_so->toggleVisible();

    // Add planet triangle scene object
    gfx::SceneObjectHandle alt_planet_triangles_so = ([&]()
    {
        /*std::vector<gfx::TexCoords> irr_mat_texco;
        gfx::Material material = gfx::Material::VertexColors(alt_planet_humidity, irr_mat_texco);


        gfx::Vertices alt_planet_irr_verts = gfx::Vertices(alt_planet_position_data, alt_planet_normal_data, irr_mat_texco);

        gfx::Primitives primitives = gfx::Primitives(alt_planet_triangles);
        gfx::Geometry geometry = gfx::Geometry(alt_planet_irr_verts, primitives);*/

        auto climate_tex = AltPlanet::Climate::createClimatePixels();

        gfx::Material material = gfx::Material(static_cast<void*>(&climate_tex.pixels[0]),
                                               climate_tex.w, climate_tex.h, gfx::gl_type(GL_FLOAT), gfx::Texture::filter::nearest);

        std::vector<gfx::TexCoords> clim_mat_texco = AltPlanet::Climate::getClimateCoords(alt_planet_irradiance, alt_planet_humidity);

        gfx::Vertices alt_planet_clim_verts = gfx::Vertices(alt_planet_position_data, alt_planet_normal_data, clim_mat_texco);

        gfx::Primitives primitives = gfx::Primitives(alt_planet_triangles);
        gfx::Geometry geometry = gfx::Geometry(alt_planet_clim_verts, primitives);

        return planet_scene_node->addSceneObject(geometry, material);
    })(); // immediately invoked lambda!

    //alt_planet_triangles_so->setWireframe(true);

    auto add_trivial_object = [](   const std::vector<vmath::Vector3> points,
                                    const std::vector<gfx::Triangle> triangles,
                                    const vmath::Vector4 &color,
                                    gfx::SceneNodeHandle &scene_node)
            {
                std::vector<vmath::Vector4> position_data;

                for (int i = 0; i<points.size(); i++)
                {
                    position_data.push_back((const vmath::Vector4&)(points[i]));
                    position_data.back().setW(1.0f);
                }

                std::vector<vmath::Vector4> normal_data;
                gfx::generateNormals(&normal_data, position_data, triangles);

                gfx::Vertices vertices = gfx::Vertices(position_data, normal_data /*, texcoords*/);

                gfx::Primitives primitives = gfx::Primitives(triangles);
                gfx::Geometry geometry = gfx::Geometry(vertices, primitives);

                gfx::Material material = gfx::Material(color);

                gfx::Transform transform;
                transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
                transform.scale = vmath::Vector3(1.00f, 1.00f, 1.00f);

                return scene_node->addSceneObject(geometry, material, transform);
            };

    // Add planet ocean scene object
    gfx::SceneObjectHandle alt_ocean_so = add_trivial_object(alt_ocean_points, alt_ocean_triangles,
                                                             vmath::Vector4(0.2f, 0.2f, 0.8f, 1.0f), planet_scene_node);

    std::cout << "alt_ocean_so" << std::endl;

    // Add planet lakes scene object
    gfx::SceneObjectHandle alt_lakes_so = add_trivial_object(alt_lake_points, alt_lake_triangles,
                                                             vmath::Vector4(0.6f, 0.6f, 0.9f, 1.0f), planet_scene_node);

    std::cout << "alt_lakes_so" << std::endl;

    // Add planet rivers scene object
    gfx::SceneObjectHandle rivers_sceneobject = ([&]()
    {
        const std::vector<gfx::Line> &rivers_primitives_data = alt_river_lines;

        gfx::Primitives primitives = gfx::Primitives(rivers_primitives_data);
        gfx::Geometry geometry = gfx::Geometry(alt_planet_vertices, primitives);

        vmath::Vector4 color(0.6f, 0.6f, 0.9f, 1.0f);
        gfx::Material material = gfx::Material(color);


        gfx::Transform transform;
        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
        transform.scale = vmath::Vector3(1.00f, 1.00f, 1.00f);

        return planet_scene_node->addSceneObject(geometry, material, transform);
    })();

    std::cout << "rivers_sceneobject" << std::endl;


    // SDL event loop
    SDL_Event event;
    bool done = false;

    const auto dt_fixed = std::chrono::milliseconds(16);
    float planet_rotation = 0.0f;
    float planet_rotation_speed = -2.0f*M_PI/64.0f; // radians/sec
    //float planet_rotation_speed = 0.0f; // radians/sec

    // test mouse
    bool lmb_down = false;
    bool rmb_down = false;

    int32_t prev_mouse_x = 0;
    int32_t prev_mouse_y = 0;

    bool fullscreen = false;

    int frame_counter = 0;
    while(!done)
    {
        ++frame_counter;

        bool resizing_this_frame = false;

        while( SDL_PollEvent(&event) )
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    if (event.key.repeat == 0)
                    {
                        switch(event.key.keysym.sym)
                        {
                            case(SDLK_f):
                                opengl_renderer.toggleWireframe();
                                break;
                            case(SDLK_q):
                                done = true;
                                break;
                            case(SDLK_h):
                                alt_planet_triangles_so->toggleVisible();
                                break;
                            case(SDLK_u): {
                                // do an iteration of repulsion
                                AltPlanet::pointsRepulse(alt_planet_points, planet_shape, 0.003f);

                                // update the scene object geometry
                                std::vector<vmath::Vector4> position_data;
                                std::vector<gfx::Point> primitives_data;

                                for (int i = 0; i < alt_planet_points.size(); i++)
                                {
                                    position_data.push_back((const vmath::Vector4&)(alt_planet_points[i]));
                                    position_data.back().setW(1.0f);
                                    primitives_data.push_back({i});
                                }

                                gfx::Vertices vertices = gfx::Vertices(position_data, position_data /*, texcoords*/);
                                gfx::Primitives primitives = gfx::Primitives(primitives_data);

                                alt_planet_points_so->mGeometry = gfx::Geometry(vertices, primitives);
                                break;
                            }
                            case(SDLK_r): {
                                SDL_SetWindowSize(mainWindow, 1500, 800);
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
                                SDL_SetWindowFullscreen(mainWindow, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP );
                                fullscreen = !fullscreen;
                                break;
                            }
                            case(SDLK_ESCAPE):
                                done = true;
                                break;
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN: {
                    lmb_down = event.button.button == SDL_BUTTON_LEFT;
                    rmb_down = event.button.button == SDL_BUTTON_RIGHT;
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    lmb_down = event.button.button == SDL_BUTTON_LEFT ? false : lmb_down;
                    rmb_down = event.button.button == SDL_BUTTON_RIGHT ? false : rmb_down;
                    break;
                }
                case SDL_MOUSEMOTION: {
                    if (lmb_down || rmb_down) {
                        int32_t mouse_delta_x = prev_mouse_x - event.motion.x;
                        int32_t mouse_delta_y = prev_mouse_y - event.motion.y;
                        float mouse_angle_x = -static_cast<float>(mouse_delta_x)*0.0062832f; // 2π/1000?
                        float mouse_angle_y = -static_cast<float>(mouse_delta_y)*0.0062832f;

                        planet_scene_node->transform.rotation =
                                vmath::Quat::rotation(mouse_angle_x, vmath::Vector3(0.0, 1.0, 0.0))*
                                vmath::Quat::rotation(mouse_angle_y, vmath::Vector3(1.0, 0.0, 0.0))*
                                planet_scene_node->transform.rotation;
                    }

                    // update previous mouse position
                    prev_mouse_x = event.motion.x;
                    prev_mouse_y = event.motion.y;
                    break;
                }
                case SDL_MOUSEWHEEL: {
                    camera.mTransform.scale -= vmath::Vector3(0.05f*event.wheel.y);
                    break;
                }
                case SDL_WINDOWEVENT: {
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_SIZE_CHANGED: { // This is the most general resize event
                            /*SDL_Log("Window %d size changed to %dx%d",
                                    event.window.windowID, event.window.data1,
                                    event.window.data2);*/
                            resizing_this_frame = true;
                            opengl_renderer.resize(event.window.data1, event.window.data2);
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

        if (!resizing_this_frame)
        {
            // draw
            opengl_renderer.draw(camera);

            // goes in drawing code:
            SDL_GL_SwapWindow(mainWindow);


            // sleep as to not consume 100% CPU
            // could perhaps subtract the frame time here...
            std::this_thread::sleep_for(dt_fixed);

        }

    }   // End while(!done)

    // after drawing finished
    GLenum gl_err = GL_NO_ERROR;
    while((gl_err = glGetError()) != GL_NO_ERROR)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "OpenGL Error (exit): " << gl_err << std::endl;
    }


    SDL_GL_DeleteContext(mainGLContext);
    SDL_DestroyWindow(mainWindow);

    return 0;
}
