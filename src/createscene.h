#ifndef CREATESCENE_H
#define CREATESCENE_H

#include "graphics/openglrenderer.h"
#include "altplanet/altplanet.h"
#include "altplanet/subivide.h"
#include "altplanet/watersystem.h"
#include "altplanet/climate/irradiance.h"
#include "altplanet/climate/humidity.h"
#include "altplanet/climate/climate.h"
#include "events/immediateevents.h"
#include "state/macrostate.h"

#include "common/pointer.h"

using PlanetShape = events::GenerateWorldEvent::PlanetShape;
using PlanetSize = events::GenerateWorldEvent::PlanetSize;

inline Ptr::OwningPtr<MacroState> createPlanetData(PlanetShape planet_shape_selector, PlanetSize planet_size_selector, int planet_seed)
{
#ifdef PROFILE
    for (int num_pts = 500; num_pts < 4000; num_pts += 500)
    {
        std::cout << "generating with " << num_pts << " points" << std::endl;

        PROFILE_BEGIN(altplanet_generate);
        auto dummy = AltPlanet::generate(num_pts, planet_shape);
        PROFILE_END(altplanet_generate);
    }
#endif
    srand(planet_seed);

    // parse input arguments
    AltPlanet::PlanetShape alt_planet_shape = planet_shape_selector == PlanetShape::Sphere ?
        AltPlanet::PlanetShape::Sphere      : planet_shape_selector == PlanetShape::Disk ?
        AltPlanet::PlanetShape::Sphere      : /*planet_shape_selector == PlanetShape::Torus ?*/
        AltPlanet::PlanetShape::Torus;

    unsigned int num_subdivisions = planet_size_selector == PlanetSize::Small  ? 0 :
                                    planet_size_selector == PlanetSize::Medium ? 1 :
                                /*planet_size_selector == PlanetSize::Large  ?*/ 2 ;

    // Alt planet
    AltPlanet::PlanetGeometry alt_planet_geometry;
    AltPlanet::Shape::BaseShape * planet_shape_ptr = nullptr;
    AltPlanet::createOrLoadPlanetGeom(alt_planet_geometry, planet_shape_ptr, alt_planet_shape);

    AltPlanet::Shape::BaseShape &planet_shape = *planet_shape_ptr;

    // TODO: Refactor out sketchy pointers...

    // generate planet
    // AltPlanet::PlanetGeometry alt_planet_geometry = AltPlanet::generate(3000, planet_shape);

    std::vector<std::vector<gfx::Triangle>> subd_triangles;
    AltPlanet::subdivideGeometry(alt_planet_geometry.points, alt_planet_geometry.triangles, subd_triangles, num_subdivisions);
    // jitter the points around a bit...
    // AltPlanet::jitterPoints(alt_planet_geometry.points);

    AltPlanet::perturbHeightNoise3D(alt_planet_geometry.points, planet_shape);

    std::vector<vmath::Vector3> &alt_planet_points = alt_planet_geometry.points;
    std::vector<gfx::Triangle> &alt_planet_triangles = alt_planet_geometry.triangles;

    // Generate the planet water system
    float planet_ocean_fraction = 0.55f;
    int num_river_springs = 100;
    auto water_geometry = AltPlanet::WaterSystem::generateWaterSystem(alt_planet_geometry, planet_shape,
                                                                      planet_ocean_fraction,
                                                                      num_river_springs);

    // Deconstruct the ocean geometry
    /*std::vector<vmath::Vector3> &alt_ocean_points = water_geometry.ocean.points;
    std::vector<gfx::Triangle> &alt_ocean_triangles = water_geometry.ocean.triangles;

    // Deconstruct freshwater geometry
    std::vector<vmath::Vector3> &alt_lake_points = water_geometry.freshwater.lakes.points;
    std::vector<gfx::Triangle> &alt_lake_triangles = water_geometry.freshwater.lakes.triangles;
    std::vector<gfx::Line> &alt_river_lines = water_geometry.freshwater.rivers.lines;*/

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

    // texcos
    std::vector<gfx::TexCoords> alt_planet_texcoords = planet_shape.getUV(alt_planet_points);
    std::vector<gfx::TexCoords> clim_mat_texco = AltPlanet::Climate::getClimateCoords(alt_planet_irradiance, alt_planet_humidity);

    return Ptr::OwningPtr<MacroState>(
        new MacroState{
            alt_planet_geometry.points,
            alt_planet_geometry.triangles,

            water_geometry.ocean.points,
            water_geometry.ocean.triangles,

            water_geometry.freshwater.lakes.points,
            water_geometry.freshwater.lakes.triangles,
            water_geometry.freshwater.rivers.lines,

            alt_planet_texcoords,
            clim_mat_texco,

            planet_shape_ptr
        }
    );
}

inline void createScene(gfx::SceneNode &scene_root, Ptr::ReadPtr<MacroState> scene_data)
{
    /*gfx::SceneNodeHandle light_scene_node = scene_root.addSceneNode();
    gfx::LightHandle light = ([](const gfx::SceneNodeHandle &scene_node)
    {
        vmath::Vector4 color(1.0f, 1.0f, 1.0f, 1.0f);
        gfx::Transform transform;
        transform.position = vmath::Vector3(10.0f, 10.0f, 10.0f);

        return scene_node->addLight(color, transform);
    })(light_scene_node);*/

    gfx::SceneNodeHandle planet_scene_node = scene_root.addSceneNode();

    // Create some alt planet vertex data to share
    std::vector<vmath::Vector4> alt_planet_position_data;
    std::vector<gfx::Point> alt_planet_point_primitives_data;

    for (int i = 0; i<scene_data->alt_planet_points.size(); i++)
    {
        alt_planet_position_data.push_back((const vmath::Vector4&)(scene_data->alt_planet_points[i]));
        alt_planet_position_data.back().setW(1.0f);
        alt_planet_point_primitives_data.push_back({i});
    }

    std::vector<vmath::Vector4> alt_planet_normal_data;
    gfx::generateNormals(&alt_planet_normal_data, alt_planet_position_data, scene_data->alt_planet_triangles);

    gfx::Vertices alt_planet_vertices = gfx::Vertices(alt_planet_position_data, alt_planet_normal_data, scene_data->alt_planet_texcoords);

    // Planet point data scene object
    gfx::SceneObjectHandle alt_planet_points_so = ([&]()
    {
        gfx::Primitives primitives = gfx::Primitives(alt_planet_point_primitives_data);
        gfx::Geometry geometry = gfx::Geometry(alt_planet_vertices, primitives);

        vmath::Vector4 color(1.0f, 0.0f, 0.0f, 1.0f);
        gfx::Material material = gfx::Material(color);
        // material.setWireframe(true);

        return planet_scene_node->addSceneObject(geometry, material);
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

        //auto climate_zone_tex = AltPlanet::Climate::createClimatePixels();
        auto climate_tex = AltPlanet::Climate::createClimateColorPixels();

        gfx::Material material = gfx::Material(static_cast<void*>(&climate_tex.pixels[0]),
                climate_tex.w, climate_tex.h, gfx::gl_type(GL_FLOAT), gfx::Texture::filter::linear);

        gfx::Vertices alt_planet_clim_verts = gfx::Vertices(alt_planet_position_data, alt_planet_normal_data, scene_data->clim_mat_texco);

        gfx::Primitives primitives = gfx::Primitives(scene_data->alt_planet_triangles);
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

        return scene_node->addSceneObject(geometry, material);
    };

    // Add planet ocean scene object
    gfx::SceneObjectHandle alt_ocean_so = add_trivial_object(scene_data->alt_ocean_points, scene_data->alt_ocean_triangles,
                                                             vmath::Vector4(0.2f, 0.2f, 0.8f, 1.0f), planet_scene_node);

    std::cout << "alt_ocean_so" << std::endl;

    // Add planet lakes scene object
    gfx::SceneObjectHandle alt_lakes_so = add_trivial_object(scene_data->alt_lake_points, scene_data->alt_lake_triangles,
                                                             vmath::Vector4(0.6f, 0.6f, 0.9f, 1.0f), planet_scene_node);

    std::cout << "alt_lakes_so" << std::endl;

    // Add planet rivers scene object
    gfx::SceneObjectHandle rivers_sceneobject = ([&]()
    {
        const std::vector<gfx::Line> &rivers_primitives_data = scene_data->alt_river_lines;

        gfx::Primitives primitives = gfx::Primitives(rivers_primitives_data);
        gfx::Geometry geometry = gfx::Geometry(alt_planet_vertices, primitives);

        vmath::Vector4 color(0.6f, 0.6f, 0.9f, 1.0f);
        gfx::Material material = gfx::Material(color);

        return planet_scene_node->addSceneObject(geometry, material);
    })();

    std::cout << "rivers_sceneobject" << std::endl;
}

inline void createMap(gfx::SceneNode &scene_root, Ptr::ReadPtr<MacroState> scene_data)
{
    //DEBUG_ASSERT(false&&"implement create map");
    gfx::SceneNodeHandle map_scene_node = scene_root.addSceneNode();

    // get the uv coordinates
    std::vector<gfx::TexCoords> map_uv_coords = scene_data->planet_base_shape->getUV(scene_data->alt_planet_points);

    // get the position data
    std::vector<vmath::Vector4> map_position_data;
    float max_x = std::numeric_limits<float>::min();
    float min_x = std::numeric_limits<float>::max();

    float max_y = std::numeric_limits<float>::min();
    float min_y = std::numeric_limits<float>::max();

    float max_z = std::numeric_limits<float>::min();
    float min_z = std::numeric_limits<float>::max();
    for (int i = 0; i<scene_data->alt_planet_points.size(); i++)
    {
        const gfx::TexCoords &uv = map_uv_coords[i];
        float h = scene_data->planet_base_shape->getHeight(scene_data->alt_planet_points[i]);
        map_position_data.push_back(vmath::Vector4{uv[0], uv[1], h, 1.0f});

        max_x = uv[0] > max_x ? uv[0] : max_x;
        min_x = uv[0] < min_x ? uv[0] : min_x;

        max_y = uv[1] > max_y ? uv[1] : max_y;
        min_y = uv[1] < min_y ? uv[1] : min_y;

        max_z = h > max_z ? h : max_z;
        min_z = h < min_z ? h : min_z;
    }

    std::cout << "range of map points:" << std::endl;
    std::cout << "x(" << min_x << ", " << max_x << ")" << std::endl;
    std::cout << "y(" << min_y << ", " << max_y << ")" << std::endl;
    std::cout << "z(" << min_z << ", " << max_z << ")" << std::endl;

    // get normals
    std::vector<vmath::Vector4> map_normal_data;
    gfx::generateNormals(&map_normal_data, map_position_data, scene_data->alt_planet_triangles);

    gfx::SceneObjectHandle map_triangles_so = ([&]()
    {
        //auto climate_zone_tex = AltPlanet::Climate::createClimatePixels();
        auto climate_tex = AltPlanet::Climate::createClimateColorPixels();

        gfx::Material material = gfx::Material(static_cast<void*>(&climate_tex.pixels[0]),
                climate_tex.w, climate_tex.h, gfx::gl_type(GL_FLOAT), gfx::Texture::filter::linear);

        gfx::Vertices map_climate_verts = gfx::Vertices(map_position_data, map_normal_data, scene_data->clim_mat_texco);

        gfx::Primitives primitives = gfx::Primitives(scene_data->alt_planet_triangles);
        gfx::Geometry geometry = gfx::Geometry(map_climate_verts, primitives);

        return map_scene_node->addSceneObject(geometry, material);
    })(); // immediately invoked lambda!

    std::cout << "created map scene object" << std::endl;

}

#endif // CREATESCENE_H
