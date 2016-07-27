#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>

#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include "planet.h"
#include "gfx_primitives.h"
#include "openglrenderer.h"

// point3 is not what is needed here. vector4 is the only one for rendering
namespace vmath = Vectormath::Aos;
//namespace oglr = OpenGLRenderer;

int main(int argc, char *argv[])
{
    // Generate planet
    Planet planet(
        3.15f,   // radius
        6,       // subdivision_level, 6 for close-up, 7 for detail+speed, 8+ slow but complex
        236234   // seed, 832576 has beautiful ocean
    );

    // think a bit about the usage

    // world creation

    // need to get planet geometry at detail levels
    // get rivers geometry at max detail (render with z-offset closer to cam)
    // get lakes geometry at max detail (render with z-offset closer to cam)

    // get height (radius) and triangle/with info using various inputs
    // getPoint3Projected<WhateverYouWant>(Point3 p);
    // getLatLong<WhateverYouWant>(LatLong ll);

    // various traversals

    // various modifiers
    // cities/roads

    // SDL2 window code:
    Uint32 flags = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL;
    int width = 1920;
    int height = 1024;

    SDL_Window * mainWindow = SDL_CreateWindow("SDL2 OpenGL test", // window name
                                  SDL_WINDOWPOS_UNDEFINED, // windowpos x
                                  SDL_WINDOWPOS_UNDEFINED, // windowpos y
                                  width, height, flags);

    SDL_GLContext mainGLContext = SDL_GL_CreateContext(mainWindow);

    std::cout << "Found graphics card: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;

    // Opengl renderer
    gfx::OpenGLRenderer opengl_renderer(width, height);

    // create some geometry
    const std::vector<vmath::Vector4> &planet_position_data = *(planet.getPointsPtr());
    const std::vector<gfx::Triangle> &planet_primitives_data = *(planet.getTrianglesPtr());

    std::vector<vmath::Vector4> planet_normal_data;
    gfx::generateNormals(&planet_normal_data, planet_position_data, planet_primitives_data);

    gfx::Vertices planet_vertices = gfx::Vertices(planet_position_data, planet_normal_data /*, texcoords*/);

    // create a scene graph node
    gfx::SceneNodeHandle planet_scene_node = opengl_renderer.addSceneNode();

    // create some scene objects using/sharing geometry
    gfx::SceneObjectHandle planet_sceneobject;
    {

        gfx::Primitives primitives = gfx::Primitives(planet_primitives_data);
        gfx::Geometry geometry = gfx::Geometry(planet_vertices, primitives);

        vmath::Vector4 color(0.07f, 0.4f, 0.15f, 1.0f);
        //vmath::Vector4 color(0.4f, 0.3f, 0.2f, 1.0f);
        //vmath::Vector4 color(0.4f, 0.4f, 0.4f, 1.0f);
        gfx::Material material = gfx::Material(color);
        //material.setWireframe(true);

        planet_sceneobject = planet_scene_node->addSceneObject(geometry, material);
    }

//    gfx::SceneObjectHandle planet_sceneobject_lod;
//    {
//        const std::vector<gfx::Triangle> &planet_primitives_data_lod = *(planet.getSubdTrianglesPtr(3));
//        gfx::Primitives primitives = gfx::Primitives(planet_primitives_data_lod);
//        gfx::Geometry geometry = gfx::Geometry(planet_vertices, primitives);

//        //vmath::Vector4 color(0.07f, 0.4f, 0.15f, 1.0f);
//        vmath::Vector4 color(0.4f, 0.3f, 0.2f, 1.0f);
//        //vmath::Vector4 color(0.4f, 0.4f, 0.4f, 1.0f);
//        gfx::Material material = gfx::Material(color);
//        material.setWireframe(true);

//        planet_sceneobject_lod = planet_scene_node->addSceneObject(geometry, material);
//    }

    gfx::SceneObjectHandle lambda_flowdown_sceneobject;
    {
        auto is_flowdown = []() { return true; };

        std::vector<gfx::Line> primitives_data;
        planet.getLinePrimitives(&primitives_data, planet.getFlowDownAdjacency(), is_flowdown);

        gfx::Primitives primitives = gfx::Primitives(primitives_data);
        gfx::Geometry geometry = gfx::Geometry(planet_vertices, primitives);

        vmath::Vector4 color(0.0f, 0.3f, 0.8f, 1.0f);
        gfx::Material material = gfx::Material(color);

        gfx::Transform transform;
        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
        transform.scale = vmath::Vector3(1.0008f, 1.0008f, 1.0008f);

        lambda_flowdown_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
    }



    gfx::SceneObjectHandle lambda_ridges_sceneobject;
    {
        auto is_ridge = [](const std::vector<int> &adj_list) { return adj_list.size()>=2; };

        std::vector<gfx::Line> primitives_data;
        planet.getLinePrimitives(&primitives_data, planet.getAdjacency(), is_ridge, planet.getFlowDownAdjacency());

        gfx::Primitives primitives = gfx::Primitives(primitives_data);
        gfx::Geometry geometry = gfx::Geometry(planet_vertices, primitives);

        vmath::Vector4 color(1.0f, 0.0f, 0.0f, 1.0f);
        gfx::Material material = gfx::Material(color);

        gfx::Transform transform;
        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
        transform.scale = vmath::Vector3(1.0005f, 1.0005f, 1.0005f);

        lambda_ridges_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
    }

    gfx::SceneObjectHandle lambda_rivers_sceneobject;
    {
        const std::vector<gfx::Line> &rivers_primitives_data = *(planet.getRiverLinesPtr());

        gfx::Primitives primitives = gfx::Primitives(rivers_primitives_data);
        gfx::Geometry geometry = gfx::Geometry(planet_vertices, primitives);

        vmath::Vector4 color(0.3f, 0.65f, 1.0f, 1.0f);
        gfx::Material material = gfx::Material(color);

        gfx::Transform transform;
        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
        transform.scale = vmath::Vector3(1.001f, 1.001f, 1.001f);

        lambda_rivers_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
    }

//    gfx::SceneObjectHandle lambda_valleys_sceneobject;
//    {
//        auto is_ridge = [](const std::vector<int> &adj_list) { return adj_list.size()>=2; };

//        std::vector<gfx::Line> primitives_data;
//        planet.getLinePrimitives(&primitives_data, planet.getAdjacency(), is_ridge, planet.getFlowUpAdjacency());

//        gfx::Primitives primitives = gfx::Primitives(primitives_data);
//        gfx::Geometry geometry = gfx::Geometry(planet_vertices, primitives);

//        vmath::Vector4 color(0.0f, 0.0f, 1.0f, 1.0f);
//        gfx::Material material = gfx::Material(color);

//        gfx::Transform transform;
//        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
//        transform.scale = vmath::Vector3(1.002f, 1.002f, 1.002f);

//        lambda_valleys_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
//    }

    gfx::SceneObjectHandle lambda_minima_sceneobject;
    {
        auto is_minimum = [](std::vector<int> &adj_list) { return adj_list.size()==0; };

        std::vector<gfx::Point> primitives_data;
        planet.getPointPrimitives(&primitives_data, planet.getAdjacency(), is_minimum, planet.getFlowDownAdjacency());

        gfx::Primitives primitives = gfx::Primitives(primitives_data);
        gfx::Geometry geometry = gfx::Geometry(planet_vertices, primitives);

        vmath::Vector4 color(0.0f, 0.0f, 1.0f, 1.0f);
        gfx::Material material = gfx::Material(color);

        gfx::Transform transform;
        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
        transform.scale = vmath::Vector3(1.003f, 1.003f, 1.003f);

        lambda_minima_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
    }

    gfx::SceneObjectHandle split_flow_sceneobject;
    {
        auto split_flow = [](const std::vector<int> &down_list, const std::vector<int> &up_list)
        {
            return down_list.size()>1 && up_list.size()>0;
        };

        std::vector<gfx::Point> primitives_data;
        planet.getPointPrimitives(&primitives_data, planet.getAdjacency(),
                         split_flow,
                         planet.getFlowDownAdjacency(),
                         planet.getFlowUpAdjacency());

        gfx::Primitives primitives = gfx::Primitives(primitives_data);
        gfx::Geometry geometry = gfx::Geometry(planet_vertices, primitives);

        vmath::Vector4 color(1.0f, 1.0f, 1.0f, 1.0f);
        gfx::Material material = gfx::Material(color);

        gfx::Transform transform;
        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
        transform.scale = vmath::Vector3(1.005f, 1.005f, 1.005f);

        split_flow_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
    }

    gfx::SceneObjectHandle ocean_sceneobject;
    {
        const std::vector<vmath::Vector4> &ocean_position_data = *(planet.getOceanVerticesPtr());
        const std::vector<gfx::Triangle> &ocean_primitives_data = *(planet.getOceanTrianglesPtr());

        std::vector<vmath::Vector4> ocean_normal_data;
        gfx::generateNormals(&ocean_normal_data, ocean_position_data, ocean_primitives_data);

        gfx::Vertices ocean_vertices = gfx::Vertices(ocean_position_data, ocean_normal_data /*, texcoords*/);
        gfx::Primitives primitives = gfx::Primitives( ocean_primitives_data );

        gfx::Geometry geometry = gfx::Geometry( ocean_vertices, primitives );

        vmath::Vector4 color(0.0f, 0.0f, 1.0f, 1.0f);
        gfx::Material material = gfx::Material(color);

        gfx::Transform transform;
        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
        transform.scale = vmath::Vector3(1.001f, 1.001f, 1.001f);

        ocean_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
    }

    // SDL event loop
    SDL_Event event;
    bool done = false;

    const auto dt_fixed = std::chrono::milliseconds(16);
    float planet_rotation = 0.0f;
    float planet_rotation_speed = 1.0f*M_PI/64.0f; // radians/sec
    //float planet_rotation_speed = 0.0f; // radians/sec

    while(!done)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    // Quit when user presses a key.
                    done = true;
                    break;

                case SDL_QUIT:
                    done = true;
                    break;

                default:
                    break;
            }   // End switch
        } // while(SDL_PollEvent(&event)))

        // animate rotation
        planet_rotation += std::chrono::duration<float>(dt_fixed).count()*planet_rotation_speed;
        vmath::Vector3 rotation_axis = vmath::normalize(vmath::Vector3(1.0f, -1.0f, 0.0f));

        planet_scene_node->transform.rotation =
            vmath::Quat::rotation(planet_rotation, rotation_axis);

//        // animate color
//        planet_sceneobject->mMaterial.color =
//            vmath::Vector4(0.5f*sin(100.0f*planet_rotation)+0.5f, 0.0f, 0.5f, 1.0f);

        // draw
        opengl_renderer.draw();

        // goes in drawing code:
        SDL_GL_SwapWindow(mainWindow);

        // sleep as to not consume 100% CPU
        std::this_thread::sleep_for (dt_fixed);

    }   // End while

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
