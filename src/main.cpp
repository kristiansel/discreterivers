#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#define _VECTORMATH_DEBUG

#include "altplanet/altplanet.h"
#include "common/gfx_primitives.h"
#include "common/serialize.h"
#include "graphics/openglrenderer.h"
#include "planet/planet.h"
#include "planet/topology.h" // for testing

// point3 is not what is needed here. vector4 is the only one for rendering
namespace vmath = Vectormath::Aos;
//namespace oglr = OpenGLRenderer;

int main(int argc, char *argv[])
{

    // Generate planet
    Planet planet(
        3.15f,  // radius
        6,      // subdivision_level, 6 for close-up, 7 for detail+speed, 8+ slow but complex
        1.0f,   // terrain roughness
        0.70f,  // fraction of planet covered by ocean
        150,     // number of freshwater springs
        5782    // seed, 832576, 236234 ocean, 234435 nice water, 6 nice ocean and lake, 5723 nice continents and islands
    );


    Topology::Test::barycentricCoords();
    Topology::Test::multinomialCoefficient();


    // Alt planet
    std::string filename = "data.dat";


    //AltPlanet::Shape::Disk disk(3.0f);
    AltPlanet::Shape::Sphere sphere(3.0f);
    AltPlanet::Shape::Torus torus(3.0f, 1.0f);
    AltPlanet::Shape::BaseShape &planet_shape = torus;

    /*
    // uncomment to make a new planet (takes some time)
    AltPlanet::Geometry temp_geom = AltPlanet::generate(15000, planet_shape);

    // Serialize it
    Serial::serialize_to_file(temp_geom, filename);
    */

    Serial::StreamType resin = Serial::read_from_file(filename);

    auto alt_planet_geometry = Serial::deserialize<AltPlanet::Geometry>(resin);

    //AltPlanet::Geometry alt_planet_geometry = AltPlanet::generate(5000, planet_shape);
    std::vector<vmath::Vector3> &alt_planet_points = alt_planet_geometry.points;
    std::vector<gfx::Triangle> &alt_planet_triangles = alt_planet_geometry.triangles;



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



    // Opengl renderer
    gfx::OpenGLRenderer opengl_renderer(width, height);

    // create a scene graph node for a light
    gfx::SceneNodeHandle light_scene_node = opengl_renderer.addSceneNode();
    gfx::LightHandle light;
    {
        vmath::Vector4 color(1.0f, 1.0f, 1.0f, 1.0f);
        gfx::Transform transform;
        transform.position = vmath::Vector3(10.0f, 10.0f, 10.0f);

        light = light_scene_node->addLight(color, transform);
    }

    // create some geometry
    const std::vector<vmath::Vector4> &planet_position_data = *(planet.getPointsPtr());
    const std::vector<gfx::Triangle> &planet_primitives_data = *(planet.getTrianglesPtr());

    std::vector<vmath::Vector4> planet_normal_data;
    gfx::generateNormals(&planet_normal_data, planet_position_data, planet_primitives_data);

    gfx::Vertices planet_vertices = gfx::Vertices(planet_position_data, planet_normal_data /*, texcoords*/);

    // create a scene graph node for the planet
    gfx::SceneNodeHandle planet_scene_node = opengl_renderer.addSceneNode();

    vmath::Vector4 fresh_water_color(0.3f, 0.6f, 1.0f, 1.0f);
    vmath::Vector4 salt_water_color(0.00f, 0.0f, 0.55f, 1.0f);


    // create some scene objects using/sharing geometry
    gfx::SceneObjectHandle planet_sceneobject = ([&]()
    {

        gfx::Primitives primitives = gfx::Primitives(planet_primitives_data);
        gfx::Geometry geometry = gfx::Geometry(planet_vertices, primitives);

        vmath::Vector4 color(0.07f, 0.4f, 0.15f, 1.0f);
        //vmath::Vector4 color(0.4f, 0.3f, 0.2f, 1.0f);
        //vmath::Vector4 color(0.4f, 0.4f, 0.4f, 1.0f);
        gfx::Material material = gfx::Material(color);
        // material.setWireframe(false);

        return planet_scene_node->addSceneObject(geometry, material);
    })(); // immediately invoked lambda!
    planet_sceneobject->toggleVisible(); // hide by default

//    // create some scene objects using/sharing geometry
//    gfx::SceneObjectHandle subd_planet_sceneobject;
//    {
//        std::vector<vmath::Vector4> subd_planet_position_data;
//        std::vector<gfx::Triangle> subd_planet_primitive_data;

//        planet.getCubicBezierGeometry(&subd_planet_position_data, &subd_planet_primitive_data, 4);

//        std::vector<vmath::Vector4> subd_planet_normal_data;
//        gfx::generateNormals(&subd_planet_normal_data, subd_planet_position_data, subd_planet_primitive_data);

//        gfx::Vertices subd_planet_vertices = gfx::Vertices(subd_planet_position_data, subd_planet_normal_data /*, texcoords*/);

//        gfx::Primitives primitives = gfx::Primitives(subd_planet_primitive_data);
//        gfx::Geometry geometry = gfx::Geometry(subd_planet_vertices, primitives);

//        vmath::Vector4 color(0.07f, 0.4f, 0.15f, 1.0f);
//        //vmath::Vector4 color(0.4f, 0.3f, 0.2f, 1.0f);
//        //vmath::Vector4 color(0.4f, 0.4f, 0.4f, 1.0f);
//        gfx::Material material = gfx::Material(color);
//        //material.setWireframe(true);

//        subd_planet_sceneobject = planet_scene_node->addSceneObject(geometry, material);
//    }

//    gfx::SceneObjectHandle subd_control_points_sceneobject;
//    {
//        std::vector<vmath::Vector4> controlpt_position_data;
//        std::vector<gfx::Point> controlpt_primitive_data;

//        planet.getBezierControlPtPrimitives(&controlpt_position_data, &controlpt_primitive_data);

//        gfx::Vertices vertices = gfx::Vertices(controlpt_position_data, controlpt_position_data /*, texcoords*/);

//        gfx::Primitives primitives = gfx::Primitives(controlpt_primitive_data);
//        gfx::Geometry geometry = gfx::Geometry(vertices, primitives);

//        vmath::Vector4 color(1.f, 1.f, 1.f, 1.0f);
//        //vmath::Vector4 color(0.4f, 0.3f, 0.2f, 1.0f);
//        //vmath::Vector4 color(0.4f, 0.4f, 0.4f, 1.0f);
//        gfx::Material material = gfx::Material(color);
//        //material.setWireframe(true);

//        //subd_control_points_sceneobject = planet_scene_node->addSceneObject(geometry, material);
//    }



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

    gfx::SceneObjectHandle alt_planet_points_so = ([&]()
    {


        std::vector<vmath::Vector4> position_data;
        std::vector<gfx::Point> primitives_data;

        for (int i = 0; i<alt_planet_points.size(); i++)
        {
           position_data.push_back((const vmath::Vector4&)(alt_planet_points[i]));
           position_data.back().setW(1.0f);
           primitives_data.push_back({i});
        }

        std::vector<vmath::Vector4> normal_data;
        gfx::generateNormals(&normal_data, position_data, alt_planet_triangles);

        gfx::Vertices vertices = gfx::Vertices(position_data, normal_data /*, texcoords*/);

        gfx::Primitives primitives = gfx::Primitives(primitives_data);
        gfx::Geometry geometry = gfx::Geometry(vertices, primitives);

        vmath::Vector4 color(1.0f, 0.0f, 0.0f, 1.0f);
        gfx::Material material = gfx::Material(color);
        //material.setWireframe(true);

        gfx::Transform transform;
        transform.scale = vmath::Vector3(1.0008f, 1.0008f, 1.0008f);

        return planet_scene_node->addSceneObject(geometry, material, transform);
    })(); // immediately invoked lambda!

    gfx::SceneObjectHandle alt_planet_triangles_so = ([&]()
    {
        std::vector<vmath::Vector4> position_data;

        for (int i = 0; i<alt_planet_points.size(); i++)
        {
           position_data.push_back((const vmath::Vector4&)(alt_planet_points[i]));
           position_data.back().setW(1.0f);
        }

        std::vector<vmath::Vector4> normal_data;
        gfx::generateNormals(&normal_data, position_data, alt_planet_triangles);

        gfx::Vertices vertices = gfx::Vertices(position_data, normal_data /*, texcoords*/);

        gfx::Primitives primitives = gfx::Primitives(alt_planet_triangles);
        gfx::Geometry geometry = gfx::Geometry(vertices, primitives);

        vmath::Vector4 color(1.f, 1.f, 1.f, 1.0f);
        gfx::Material material = gfx::Material(color);
        material.setWireframe(false);

        return planet_scene_node->addSceneObject(geometry, material);
    })(); // immediately invoked lambda!

    /*
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

        //lambda_flowdown_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
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

        //lambda_ridges_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
    }

    gfx::SceneObjectHandle rivers_sceneobject;
    {
        const std::vector<gfx::Line> &rivers_primitives_data = *(planet.getRiverLinesPtr());

        gfx::Primitives primitives = gfx::Primitives(rivers_primitives_data);
        gfx::Geometry geometry = gfx::Geometry(planet_vertices, primitives);

        vmath::Vector4 color = fresh_water_color;
        gfx::Material material = gfx::Material(color);

        gfx::Transform transform;
        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
        transform.scale = vmath::Vector3(1.001f, 1.001f, 1.001f);

        //rivers_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
    }

    gfx::SceneObjectHandle lambda_valleys_sceneobject;
    {
        auto is_valley = [](const std::vector<int> &adj_list) { return adj_list.size()>=2; };

        std::vector<gfx::Line> primitives_data;
        planet.getLinePrimitives(&primitives_data, planet.getAdjacency(), is_valley, planet.getFlowUpAdjacency());

        gfx::Primitives primitives = gfx::Primitives(primitives_data);
        gfx::Geometry geometry = gfx::Geometry(planet_vertices, primitives);

        vmath::Vector4 color(0.35f, 0.0f, 0.7f, 1.0f);
        gfx::Material material = gfx::Material(color);

        gfx::Transform transform;
        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
        transform.scale = vmath::Vector3(1.002f, 1.002f, 1.002f);

        //lambda_valleys_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
    }

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

        //lambda_minima_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
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

        //split_flow_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
    }*/


//    gfx::SceneObjectHandle ocean_sceneobject;
//    {
//        const std::vector<vmath::Vector4> &ocean_position_data = *(planet.getOceanVerticesPtr());
//        const std::vector<gfx::Triangle> &ocean_primitives_data = *(planet.getOceanTrianglesPtr());

//        std::vector<vmath::Vector4> ocean_normal_data;
//        gfx::generateNormals(&ocean_normal_data, ocean_position_data, ocean_primitives_data);

//        gfx::Vertices ocean_vertices = gfx::Vertices(ocean_position_data, ocean_normal_data /*, texcoords*/);
//        gfx::Primitives primitives = gfx::Primitives( ocean_primitives_data );

//        gfx::Geometry geometry = gfx::Geometry( ocean_vertices, primitives );

//        vmath::Vector4 color = salt_water_color;
//        gfx::Material material = gfx::Material(color);

//        gfx::Transform transform;
//        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
//        //transform.scale = vmath::Vector3(1.001f, 1.001f, 1.001f);

//        //ocean_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
//    }

//    gfx::SceneObjectHandle lakes_sceneobject;
//    {
//        const std::vector<vmath::Vector4> &lakes_position_data = *(planet.getLakeVerticesPtr());
//        const std::vector<gfx::Triangle> &lakes_primitives_data = *(planet.getLakeTrianglesPtr());

//        std::vector<vmath::Vector4> lakes_normal_data;
//        gfx::generateNormals(&lakes_normal_data, lakes_position_data, lakes_primitives_data);

//        gfx::Vertices lakes_vertices = gfx::Vertices(lakes_position_data, lakes_normal_data /*, texcoords*/);
//        gfx::Primitives primitives = gfx::Primitives( lakes_primitives_data );

//        gfx::Geometry geometry = gfx::Geometry( lakes_vertices, primitives );

//        vmath::Vector4 color = fresh_water_color;
//        gfx::Material material = gfx::Material(color);

//        gfx::Transform transform;
//        transform.position = vmath::Vector3(0.0f, 0.0f, 0.0f);
//        transform.scale = vmath::Vector3(1.001f, 1.001f, 1.001f);

//        //lakes_sceneobject = planet_scene_node->addSceneObject(geometry, material, transform);
//    }

    // SDL event loop
    SDL_Event event;
    bool done = false;

    const auto dt_fixed = std::chrono::milliseconds(16);
    float planet_rotation = 0.0f;
    float planet_rotation_speed = -2.0f*M_PI/64.0f; // radians/sec
    //float planet_rotation_speed = 0.0f; // radians/sec

    int x = 0;
    while(!done)
    {
        ++x;
        while( SDL_PollEvent(&event) )
        {
            switch(event.type)
            {
                case(SDL_KEYDOWN):
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
                                planet_sceneobject->toggleVisible();
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
                            case(SDLK_ESCAPE):
                                done = true;
                                break;

                        }
                    }
                    break; // without this, it quits...

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
        opengl_renderer.draw();

        // goes in drawing code:
        SDL_GL_SwapWindow(mainWindow);

        // sleep as to not consume 100% CPU
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
