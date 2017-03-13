#include "createscene.h"

#include "altplanet/altplanet.h"
#include "altplanet/subivide.h"
#include "altplanet/watersystem.h"
#include "altplanet/climate/irradiance.h"
#include "altplanet/climate/humidity.h"
#include "altplanet/climate/climate.h"
#include "common/macro/macrodebuglog.h"

Ptr::OwningPtr<MacroState> createPlanetData(PlanetShape planet_shape_selector, PlanetSize planet_size_selector, int planet_seed)
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

    float planet_scale_factor  = planet_size_selector == PlanetSize::Small  ?     400.0f : // gives a side length of approx 50 m
                                 planet_size_selector == PlanetSize::Medium ?     800.0f :
                                /*planet_size_selector == PlanetSize::Large  ?*/ 1600.0f ;

//    float planet_scale_factor  = planet_size_selector == PlanetSize::Small  ?     1.0f :
//                                 planet_size_selector == PlanetSize::Medium ?     1.0f :
//                                /*planet_size_selector == PlanetSize::Large  ?*/  1.0f ;


    // Alt planet
    AltPlanet::PlanetGeometry alt_planet_geometry;
    AltPlanet::Shape::BaseShape * planet_shape_ptr = nullptr;
    AltPlanet::createOrLoadPlanetGeom(alt_planet_geometry, planet_shape_ptr, alt_planet_shape, planet_scale_factor);

    AltPlanet::Shape::BaseShape &planet_shape = *planet_shape_ptr;


    // TODO: Refactor out sketchy pointers...

    // generate planet
    // AltPlanet::PlanetGeometry alt_planet_geometry = AltPlanet::generate(3000, planet_shape);

    for (int i = 0; i<num_subdivisions; i++)
    {
        // subdivide
        AltPlanet::subdivideOnce(alt_planet_geometry.points, alt_planet_geometry.triangles);

        // reproject points
        AltPlanet::reproject(alt_planet_geometry.points, planet_shape);

        // jitter the points around a bit...
        // AltPlanet::jitterPoints(alt_planet_geometry.points);
    }

    AltPlanet::perturbHeightNoise3D(alt_planet_geometry.points, planet_shape);

    std::vector<vmath::Vector3> &alt_planet_points = alt_planet_geometry.points;
    std::vector<gfx::Triangle> &alt_planet_triangles = alt_planet_geometry.triangles;

    /*std::vector<float> side_lengths;
    for (const auto &tri : alt_planet_triangles)
    {
        float s1 = vmath::length(alt_planet_points[tri[0]] - alt_planet_points[tri[1]]);
        float s2 = vmath::length(alt_planet_points[tri[0]] - alt_planet_points[tri[2]]);
        float s3 = vmath::length(alt_planet_points[tri[1]] - alt_planet_points[tri[2]]);
        float av = 0.33f*(s1 + s2 + s3);
        side_lengths.push_back(av);
    }

    float av_av = 0.0f;
    for (const auto &sl : side_lengths) { av_av += sl; }
    av_av /= side_lengths.size();

    DEBUG_LOG("average triangle side length = " << av_av);*/


    // Generate the planet water system
    float planet_ocean_fraction = 0.55f;
    int num_river_springs = 100;
    auto water_geometry = AltPlanet::WaterSystem::generateWaterSystem(alt_planet_geometry, planet_shape,
                                                                      planet_ocean_fraction,
                                                                      num_river_springs);

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


inline gfx::SceneObjectHandle add_trivial_object(const std::vector<vmath::Vector3> &points,
                                                const std::vector<gfx::Triangle> &triangles,
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
}

void createScene(gfx::SceneNodeHandle scene_root_hdl, Ptr::ReadPtr<MacroState> scene_data, float &cam_view_distance)
{
    gfx::SceneNode &scene_root = *scene_root_hdl;
    gfx::SceneNodeHandle planet_scene_node = scene_root.addSceneNode();

    // Create some alt planet vertex data to share
    std::vector<vmath::Vector4> alt_planet_position_data;
    std::vector<gfx::Point> alt_planet_point_primitives_data;

    float largest_dist_sqr = 0;
    for (int i = 0; i<scene_data->alt_planet_points.size(); i++)
    {
        alt_planet_position_data.push_back((const vmath::Vector4&)(scene_data->alt_planet_points[i]));
        alt_planet_position_data.back().setW(1.0f);
        alt_planet_point_primitives_data.push_back({i});
        float p_len_sqr = vmath::lengthSqr(scene_data->alt_planet_points[i]);
        largest_dist_sqr = p_len_sqr > largest_dist_sqr ? p_len_sqr : largest_dist_sqr;
    }

    cam_view_distance = 2.0f * std::sqrt(largest_dist_sqr);

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
        float z_offset = +0.005;
        gfx::Material material = gfx::Material(color, z_offset);

        return planet_scene_node->addSceneObject(geometry, material);
    })();

    std::cout << "rivers_sceneobject" << std::endl;
}


inline std::vector<vmath::Vector4> proj2d(const std::vector<vmath::Vector3> &points,
                                          const AltPlanet::Shape::BaseShape *planet_shape,
                                          float u_cycle, float v_cycle)
{
    std::vector<gfx::TexCoords> map_uv_coords = planet_shape->getUV(points);

    for (auto &uv : map_uv_coords)
    {
        float intpart;
        uv[0] = modf(uv[0]+u_cycle, &intpart);
        uv[0] = uv[0]<0.0f ? 1.0f+uv[0] : uv[0];
        uv[1] = modf(uv[1]+v_cycle, &intpart);
        uv[1] = uv[1]<0.0f ? 1.0f+uv[1] : uv[1];
    }

    float u_bias = -0.5f+u_cycle;
    float v_bias = -0.5f+v_cycle;
    float aspect_uv = planet_shape->aspectUV();
    float u_scale = 2.0f * aspect_uv;
    float v_scale = 2.0f;

    std::vector<vmath::Vector4> position_data;
    for (int i = 0; i<points.size(); i++)
    {
        const gfx::TexCoords &uv = map_uv_coords[i];
        float h = planet_shape->getHeight(points[i]);
        position_data.push_back(vmath::Vector4{u_scale*(uv[0]+u_bias), v_scale*(uv[1]+v_bias), h, 1.0f});
    }

    return position_data;
}


std::vector<gfx::Line> filterLines(const std::vector<gfx::Line> &unfiltered,
                                   const std::vector<vmath::Vector4> &pos_data)
{
    std::vector<gfx::Line> filtered(unfiltered.size());

    auto it = std::copy_if (unfiltered.begin(), unfiltered.end(), filtered.begin(),
        [&](const gfx::Line &l){
            return vmath::length(pos_data[l[0]]-pos_data[l[1]])<0.97f;
        }
    );
    filtered.resize(std::distance(filtered.begin(), it));  // shrink container to new size

    return filtered;
}

std::vector<gfx::Triangle> filterTriangles(const std::vector<gfx::Triangle> &unfiltered,
                                           const std::vector<vmath::Vector4> &pos_data)
{
    std::vector<gfx::Triangle> filtered(unfiltered.size());

    auto it = std::copy_if (unfiltered.begin(), unfiltered.end(), filtered.begin(),
        [&](const gfx::Triangle &t) -> bool {
            return (vmath::length(pos_data[t[0]]-pos_data[t[1]])<0.97f &&
                    vmath::length(pos_data[t[0]]-pos_data[t[2]])<0.97f &&
                    vmath::length(pos_data[t[1]]-pos_data[t[2]])<0.97f);
        }
    );
    filtered.resize(std::distance(filtered.begin(), it));  // shrink container to new size

    return filtered;
}

inline gfx::SceneObjectHandle add_trivial_map_object(const std::vector<vmath::Vector3> &points,
                                                     const std::vector<gfx::Triangle> &triangles,
                                                     const vmath::Vector4 &color,
                                                     gfx::SceneNodeHandle &scene_node,
                                                     const AltPlanet::Shape::BaseShape *planet_shape,
                                                     float u_cycle, float v_cycle, float h_min, float h_max)
{
    std::vector<vmath::Vector4> position_data = proj2d(points, planet_shape, u_cycle, v_cycle);

    // scale the z coord
    for (int i = 0; i<position_data.size(); i++)
    {
        position_data[i][2] = (position_data[i][2]-h_min)/(h_max-h_min);
    }

    std::vector<vmath::Vector4> normal_data(points.size(), vmath::Vector4(0.0f, 0.0f, 1.0f, 0.0f));

    gfx::Vertices vertices = gfx::Vertices(position_data, normal_data /*, texcoords*/);

    std::vector<gfx::Triangle> primitives_data = filterTriangles(triangles, position_data);

    gfx::Primitives primitives = gfx::Primitives(primitives_data);
    gfx::Geometry geometry = gfx::Geometry(vertices, primitives);

    gfx::Material material = gfx::Material(color);

    return scene_node->addSceneObject(geometry, material);
}


void createMapNodeWithOffset(gfx::SceneNodeHandle scene_root_hdl, Ptr::ReadPtr<MacroState> scene_data,
                             float u_cycle, float v_cycle, float h_min, float h_max)
{
    gfx::SceneNode &scene_root = *scene_root_hdl;

    //DEBUG_ASSERT(false&&"implement create map");
    gfx::SceneNodeHandle map_scene_node = scene_root.addSceneNode();

    std::vector<vmath::Vector4> map_position_data = proj2d(scene_data->alt_planet_points, scene_data->planet_base_shape, u_cycle, v_cycle);

    // scale the z coord
    for (int i = 0; i<map_position_data.size(); i++)
    {
        map_position_data[i][2] = (map_position_data[i][2]-h_min)/(h_max-h_min);
    }

    // get normals
    std::vector<vmath::Vector4> map_normal_data;
    gfx::generateNormals(&map_normal_data, map_position_data, scene_data->alt_planet_triangles);

    gfx::Vertices map_climate_verts = gfx::Vertices(map_position_data, map_normal_data, scene_data->clim_mat_texco);

    gfx::SceneObjectHandle map_triangles_so = ([&]()
    {
        //auto climate_zone_tex = AltPlanet::Climate::createClimatePixels();
        auto climate_tex = AltPlanet::Climate::createClimateColorPixels();

        std::vector<gfx::Triangle> planet_primitives_data = filterTriangles(scene_data->alt_planet_triangles, map_position_data);

        gfx::Primitives primitives = gfx::Primitives(planet_primitives_data);
        gfx::Geometry geometry = gfx::Geometry(map_climate_verts, primitives);

        gfx::Material material = gfx::Material(static_cast<void*>(&climate_tex.pixels[0]),
                climate_tex.w, climate_tex.h, gfx::gl_type(GL_FLOAT), gfx::Texture::filter::linear);

        return map_scene_node->addSceneObject(geometry, material);
    })(); // immediately invoked lambda!

    // Add planet ocean scene object
    gfx::SceneObjectHandle alt_ocean_so = add_trivial_map_object(scene_data->alt_ocean_points, scene_data->alt_ocean_triangles,
                                                vmath::Vector4(0.2f, 0.2f, 0.8f, 1.0f), map_scene_node, scene_data->planet_base_shape,
                                                u_cycle, v_cycle, h_min, h_max);
    // Add planet lakes scene object
    gfx::SceneObjectHandle alt_lakes_so = add_trivial_map_object(scene_data->alt_lake_points, scene_data->alt_lake_triangles,
                                                vmath::Vector4(0.6f, 0.6f, 0.9f, 1.0f), map_scene_node, scene_data->planet_base_shape,
                                                u_cycle, v_cycle, h_min, h_max);

    // Add planet rivers scene object
    gfx::SceneObjectHandle rivers_sceneobject = ([&]()
    {
        std::vector<gfx::Line> rivers_primitives_data = filterLines(scene_data->alt_river_lines, map_position_data);

        gfx::Primitives primitives = gfx::Primitives(rivers_primitives_data);
        gfx::Geometry geometry = gfx::Geometry(map_climate_verts, primitives);

        vmath::Vector4 color(0.6f, 0.6f, 0.9f, 1.0f);
        float z_offset = +0.005;
        gfx::Material material = gfx::Material(color, z_offset);

        return map_scene_node->addSceneObject(geometry, material);
    })();

    std::cout << "rivers_sceneobject" << std::endl;
}


void createMap(gfx::SceneNodeHandle scene_root_hdl, Ptr::ReadPtr<MacroState> scene_data)
{
    const std::vector<vmath::Vector3> &points = scene_data->alt_planet_points;
    const AltPlanet::Shape::BaseShape *planet_shape = scene_data->planet_base_shape;

    // vertical scaling...
    float max_h = std::numeric_limits<float>::lowest();
    float min_h = std::numeric_limits<float>::max();
    for (int i = 0; i<points.size(); i++)
    {
        float h = planet_shape->getHeight(points[i]);
        max_h = h > max_h ? h : max_h;
        min_h = h < min_h ? h : min_h;
    }

    createMapNodeWithOffset(scene_root_hdl, scene_data, 0.0, 0.0, min_h, max_h);
    createMapNodeWithOffset(scene_root_hdl, scene_data, 0.5, 0.0, min_h, max_h);
    createMapNodeWithOffset(scene_root_hdl, scene_data, -0.5, 0.0, min_h, max_h);
    createMapNodeWithOffset(scene_root_hdl, scene_data, 0.0, 0.5, min_h, max_h);
    createMapNodeWithOffset(scene_root_hdl, scene_data, 0.0, -0.5, min_h, max_h);

    // do something funny with this after a while
}
