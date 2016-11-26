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
#include "altplanet/irradiance.h"
#include "common/macro/macroprofile.h"
#include "common/gfx_primitives.h"
#include "common/serialize.h"
#include "graphics/openglrenderer.h"

// point3 is not what is needed here. vector4 is the only one for rendering
namespace vmath = Vectormath::Aos;
//namespace oglr = OpenGLRenderer;

int main(int argc, char *argv[])
{
    //AltPlanet::Shape::Disk disk(3.0f);
    AltPlanet::Shape::Sphere sphere(3.0f);
    AltPlanet::Shape::Torus torus(3.0f, 1.0f);
    AltPlanet::Shape::BaseShape &planet_shape = torus;

    // profile
    /*for (int num_pts = 500; num_pts<4000; num_pts+=500)
    {
        std::cout << "generating with " << num_pts << " points" << std::endl;
        PROFILE_BEGIN()

        auto dummy = AltPlanet::generate(num_pts, planet_shape);
        
        PROFILE_END(altplanet_generate)
    }*/

    // Alt planet
    std::string planet_filename = "torus_planet.dat";

    AltPlanet::PlanetGeometry alt_planet_geometry;
/*
    // try to open planet file
    std::ifstream file(planet_filename, std::ios::binary);
    bool loading_went_bad = false;
    if (file.is_open()) {
        // load planet from file
        try {
            std::cout << "loading planet file: " << planet_filename << std::endl;
            Serial::StreamType resin = Serial::read_from_file(planet_filename);
            alt_planet_geometry = Serial::deserialize<AltPlanet::PlanetGeometry>(resin);
        } catch (...) {
            loading_went_bad = true;
            std::cout << "something went wrong while trying to load " << planet_filename << std::endl;
        }
    }
    else
    {
        std::cout << "could not open file " << planet_filename << std::endl;
        loading_went_bad = true;
    }

    if (loading_went_bad)
    {
        std::cout << "generating planet geometry" << std::endl;
        // create the planet
*/
        // Generate geometry
        alt_planet_geometry = AltPlanet::generate(3000, planet_shape);
/*
        // Serialize it
        try {
            Serial::serialize_to_file(alt_planet_geometry, planet_filename);
        } catch (...) {
            std::cout << "something went wrong while trying to serialize to " << planet_filename << std::endl;
        }
    }*/

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


    // SDL2 window code:
    Uint32 flags = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL;
    int width = 1800;
    int height = 900;

    //int width = 2800;
    //int height = 1600;

    SDL_Window * mainWindow = SDL_CreateWindow("SDL2 OpenGL test", // window name
                                  SDL_WINDOWPOS_UNDEFINED, // windowpos x
                                  SDL_WINDOWPOS_UNDEFINED, // windowpos y
                                  width, height, flags);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GLContext mainGLContext = SDL_GL_CreateContext(mainWindow);

    std::cout << "Found graphics card: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;


    // Camera
    gfx::Camera camera(width, height);

    // Opengl renderer
    gfx::OpenGLRenderer opengl_renderer;

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

    gfx::Vertices alt_planet_vertices = gfx::Vertices(alt_planet_position_data, alt_planet_normal_data);

    // Planet point data scene object
    gfx::SceneObjectHandle alt_planet_points_so = ([&]()
    {
       gfx::Primitives primitives = gfx::Primitives(alt_planet_point_primitives_data);
       gfx::Geometry geometry = gfx::Geometry(alt_planet_vertices, primitives);

       vmath::Vector4 color(1.0f, 0.0f, 0.0f, 1.0f);
       gfx::Material material = gfx::Material(color);
       //material.setWireframe(true);

       gfx::Transform transform;
       transform.scale = vmath::Vector3(1.0008f, 1.0008f, 1.0008f);

       return planet_scene_node->addSceneObject(geometry, material, transform);
    })(); // immediately invoked lambda!

    // Add planet triangle scene object
    gfx::SceneObjectHandle alt_planet_triangles_so = ([&]()
    {

        gfx::Primitives primitives = gfx::Primitives(alt_planet_triangles);
        gfx::Geometry geometry = gfx::Geometry(alt_planet_vertices, primitives);

        vmath::Vector4 color(1.f, 1.f, 1.f, 1.0f);
        gfx::Material material = gfx::Material(color);

        /*gfx::Material::ColorScale colorScale = {
            {0.0f, {0.0f, 0.0f, 0.0f, 1.0f}},
            {1.0f, {1.0f, 0.5f, 0.0f, 1.0f}}
        }*/

        //gfx::Material material = gfx::VertexColorMaterial(alt_planet_irradiance /*, colorScale*/)
        material.setWireframe(false);

        return planet_scene_node->addSceneObject(geometry, material);
    })(); // immediately invoked lambda!

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
                                                             vmath::Vector4(0.5f, 0.5f, 0.6f, 1.0f), planet_scene_node);

    // Add planet lakes scene object
    gfx::SceneObjectHandle alt_lakes_so = add_trivial_object(alt_lake_points, alt_lake_triangles,
                                                             vmath::Vector4(0.7f, 0.7f, 0.8f, 1.0f), planet_scene_node);

    // Add planet rivers scene object
    gfx::SceneObjectHandle rivers_sceneobject = ([&]()
    {
        const std::vector<gfx::Line> &rivers_primitives_data = alt_river_lines;

        gfx::Primitives primitives = gfx::Primitives(rivers_primitives_data);
        gfx::Geometry geometry = gfx::Geometry(alt_planet_vertices, primitives);

        vmath::Vector4 color(0.7f, 0.7f, 0.8f, 1.0f);
        gfx::Material material = gfx::Material(color);

        gfx::Transform transform;
        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
        transform.scale = vmath::Vector3(1.00f, 1.00f, 1.00f);

        return planet_scene_node->addSceneObject(geometry, material, transform);
    })();

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

    int frame_counter = 0;
    while(!done)
    {
        ++frame_counter;
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
                    case(SDLK_u):
                    {
                        // do an iteration of repulsion
                        AltPlanet::pointsRepulse(alt_planet_points, planet_shape, 0.003f);

                        // update the scene object geometry
                        std::vector<vmath::Vector4> position_data;
                        std::vector<gfx::Point> primitives_data;

                        for (int i = 0; i<alt_planet_points.size(); i++)
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
                    case(SDLK_t):
                    {

                    }
                    case(SDLK_ESCAPE):
                        done = true;
                        break;
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
            {
                lmb_down = event.button.button == SDL_BUTTON_LEFT;
                rmb_down = event.button.button == SDL_BUTTON_RIGHT;
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                lmb_down = event.button.button == SDL_BUTTON_LEFT ? false : lmb_down;
                rmb_down = event.button.button == SDL_BUTTON_RIGHT ? false : rmb_down;
                break;
            }
            case SDL_MOUSEMOTION:
            {
                if (lmb_down || rmb_down)
                    std::cout << "Mouse = (" << event.motion.x << ", " << event.motion.y << ")" << std::endl;
                break;
            }
            case SDL_QUIT:
                done = true;
                break;
            default:
                break;
            }   // End switch
        } // while(SDL_PollEvent(&event)))

        // animate rotation
        planet_rotation += std::chrono::duration<float>(dt_fixed).count()*planet_rotation_speed;
        vmath::Vector3 rotation_axis1 = vmath::normalize(vmath::Vector3(1.0f, -1.0f, 0.0f));
        vmath::Vector3 rotation_axis2 = vmath::normalize(vmath::Vector3(-1.0f, -1.0f, 0.0f));


        planet_scene_node->transform.rotation = vmath::Quat::rotation(planet_rotation, rotation_axis1)
                                               *vmath::Quat::rotation(planet_rotation/3.0f, rotation_axis2);

//        // animate color
//        planet_sceneobject->mMaterial.color =
//            vmath::Vector4(0.5f*sin(100.0f*planet_rotation)+0.5f, 0.0f, 0.5f, 1.0f);

        // draw
        opengl_renderer.draw(camera);

        // goes in drawing code:
        SDL_GL_SwapWindow(mainWindow);

        // sleep as to not consume 100% CPU
        // could perhaps subtract the frame time here...
        std::this_thread::sleep_for (dt_fixed);

    }   // End while(!done)

    // after drawing finished
    GLenum gl_err = GL_NO_ERROR;
    while((gl_err = glGetError()) != GL_NO_ERROR)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "OpenGL Error: " << gl_err << std::endl;
    }


    SDL_GL_DeleteContext(mainGLContext);
    SDL_DestroyWindow(mainWindow);

    return 0;
}
