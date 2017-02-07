#include "altplanet.h"
#include "triangulate.hpp"
#include "incrtriangulate.hpp"
#include "../common/macro/macrodebugassert.h"
#include "../common/macro/macroprofile.h"
#include "../common/procedural/noise3d.h"
#include "../common/collision/projection.h"
#include "../common/mathext.h"
#include "../common/stdext.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <vector>


using namespace MathExt;

namespace AltPlanet
{
	std::vector<gfx::Triangle> triangulateAndOrient(const std::vector<vmath::Vector3> &points,
													const SpaceHash3D &spacehash,
													const Shape::BaseShape &planet_shape);

    void removePointsTooClose(std::vector<vmath::Vector3> &points, SpaceHash3D &spacehash, float min_distance);

    float scaleByPointDensity(const SpaceHash3D &spacehash) {return cbrt(1.0f/spacehash.getPointDensity());}

    PlanetGeometry generate(unsigned int n_points, const Shape::BaseShape &planet_shape)
	{
        PROFILE_BEGIN(generate_timer);
        std::cout << "generating planet... " << std::endl;

        PlanetGeometry geometry;
		std::vector<vmath::Vector3> &points = geometry.points;
		std::vector<gfx::Triangle> &triangles = geometry.triangles;

		// generate random points
		Shape::AABB aabb = planet_shape.getAABB();


		points.reserve(n_points);

		for (unsigned int i = 0; i < n_points; i++)
		{
			vmath::Vector3 point = {frand(-aabb.width *0.75, aabb.width *0.75),
									frand(-aabb.height*0.75, aabb.height*0.75),
									frand(-aabb.width *0.75, aabb.width *0.75)};

			// project onto shape
			point = planet_shape.projectPoint(point);

			points.push_back(point);
		}

		// hash the points onto a 3d grid
		SpaceHash3D spacehash(points);

		// Distribute them (more) evenly
		std::vector<vmath::Vector3> pt_force;
		pt_force.resize(points.size());

		//int k_nn = 5; // nearest neighbors
		//float repulse_factor = 0.006f; // repulsive force factor

        //std::cout << "distributing points evenly... ";
		int n_redistribute_iterations = 25;
		for (int i_red = 0; i_red < n_redistribute_iterations; i_red++)
		{
			float it_repulsion_factor = i_red > 2 ? 0.003f : 0.008f;
            //std::cout << i_red << "/" << n_redistribute_iterations << std::endl;
			//float it_repulse_factor = repulse_factor/(sqrt(float(i_red)));
			pointsRepulse(points, spacehash, planet_shape, it_repulsion_factor);
		}
        //std::cout << "done!" << std::endl;

		// finished distributing them evenly...

        float degenerate_point_sensitivity = 0.2480/2.0f;
        float min_distance = degenerate_point_sensitivity*scaleByPointDensity(spacehash); // size dependent
        removePointsTooClose(points, spacehash, min_distance);


		// triangulate... :)
        //std::cout << "triangulating points" << std::endl;
		triangles = triangulateAndOrient(points, spacehash, planet_shape);
        //std::cout << "done!" << std::endl;

        //std::cout << "found " << triangles.size() << " triangles" << std::endl;

        //perturbHeightNoise3D(points, planet_shape);

        std::cout << "done!" << std::endl;

        PROFILE_END(generate_timer);

		return geometry;
	}

	void pointsRepulse(std::vector<vmath::Vector3> &points, SpaceHash3D &spacehash, const Shape::BaseShape &planet_shape, float repulse_factor)
	{
		float largest_force = 0.0f;

		std::vector<vmath::Vector3> pt_force;
		pt_force.resize(points.size());

        float forceRadius = 1.8f*scaleByPointDensity(spacehash);
        float repulse_factor_scaled = 2.0f*forceRadius*repulse_factor;

		for (int i_p = 0; i_p<points.size(); i_p++)
		{
			pt_force[i_p] = {0.0f, 0.0f, 0.0f};
            spacehash.forEachPointInSphere(points[i_p], forceRadius,
										   [&](const int &i_n) -> bool
										   {
											   const auto diff_vector = points[i_p] - points[i_n];
											   float diff_length = vmath::length(diff_vector);
											   if (diff_length > 0.0f)
											   {
                                                   pt_force[i_p] += repulse_factor_scaled * vmath::normalize(diff_vector)/(diff_length);
											   }
											   return false;
										   });

			//for (int i = 0; i < 3; i++) if(isnan_lame(point[i])) std::cerr << "nan detected" << std::endl;

			// reduce the force size
			float force_size = vmath::length(pt_force[i_p]);
			float use_length = std::min(0.2f, force_size);
			pt_force[i_p] = use_length*vmath::normalize(pt_force[i_p]);

			// fix NaN
			for (int i = 0; i < 3; i++) if(isnan_lame(pt_force[i_p][i])) pt_force[i_p] = {0.f, 0.f, 0.f};

			// apply the force
			points[i_p] += pt_force[i_p];

			if (force_size > largest_force) largest_force = force_size;

			//std::cout << "n_neighbors: " << neighbors.size() << std::endl;
			//std::cout << "length(force):" << force_size << std::endl;
		}

		// check if nan
		for (const auto point : points)
		{
			for (int i = 0; i < 3; i++) if(isnan_lame(point[i])) std::cerr << "nan detected" << std::endl;
		}

		// reproject
		for (unsigned int i = 0; i < points.size(); i++)
		{
			// project onto shape
			points[i] = planet_shape.projectPoint(points[i]);
		}

		// rehash the points
		spacehash.rehash(points);
	}

	void pointsRepulse(std::vector<vmath::Vector3> &points, const Shape::BaseShape &planet_shape, float repulse_factor)
	{
		SpaceHash3D spacehash(points);
		pointsRepulse(points, spacehash, planet_shape, repulse_factor);
	}

	//bool custom_tri_equal(gfx::Triangle t1, gfx::Triangle t2)
	//{
	//    std::array<int,3> inds = {t1[0], t1[1], t1[2]};
	//    std::sort(inds.begin(), inds.end());
	//}

	inline vmath::Vector3 triangleCross(const gfx::Triangle &triangle, const std::vector<vmath::Vector3> &points)
	{
		const auto v1 = points[triangle[1]] - points[triangle[0]];
		const auto v2 = points[triangle[2]] - points[triangle[0]];
		return vmath::cross(v1,v2);
	}

	inline vmath::Vector3 triangleNormal(const gfx::Triangle &triangle, const std::vector<vmath::Vector3> &points)
	{
		return vmath::normalize(triangleCross(triangle, points));
	}

	void orientTriangles(const std::vector<vmath::Vector3> &points,
						 std::vector<gfx::Triangle> &triangles,
						 const Shape::BaseShape &planet_shape)
	{
		for (int i_t = 0; i_t<triangles.size(); i_t++)
		{
			gfx::Triangle &tri = triangles[i_t];
			const auto v1 = points[tri[1]] - points[tri[0]];
			const auto v2 = points[tri[2]] - points[tri[0]];

			if (vmath::dot(vmath::cross(v1,v2), planet_shape.getGradDir(points[tri[0]])) < 0.0f)
			{
				// reverse the order of the triangle indices
				std::swap(tri[0], tri[2]);
			}
		}
	}

	inline void surfaceGradFilterTriangles( const std::vector<vmath::Vector3> &points,
											std::vector<gfx::Triangle> &triangles,
											const Shape::BaseShape &planet_shape)
	{
		float n_dot_thresh = 0.9f;
		auto remove_predicate = [&](const gfx::Triangle &tri) -> bool
		{
			vmath::Vector3 tri_n = triangleNormal(tri, points);
			vmath::Vector3 av_pt = 0.33f*(points[tri[0]]+points[tri[1]]+points[tri[2]]);
			vmath::Vector3 grad_planet_n = vmath::normalize(planet_shape.getGradDir(av_pt));
			return vmath::dot(tri_n, grad_planet_n)<n_dot_thresh;
		};
		triangles.erase(std::remove_if(triangles.begin(), triangles.end(), remove_predicate), triangles.end());
	}

    /*
	inline bool checkIfTrianglesIntersect(const std::vector<vmath::Vector3> &points,
										  const gfx::Triangle &tri1, const gfx::Triangle &tri2)
	{
		std::array<vmath::Vector3, 3> triangle1 = {points[tri1[0]], points[tri1[1]], points[tri1[2]]};	
		std::array<vmath::Vector3, 3> triangle2 = {points[tri2[0]], points[tri2[1]], points[tri2[2]]};	
		return Projection::trianglesOverlap(triangle1, triangle2);
	}


    inline void overlapFilterTriangles( const std::vector<vmath::Vector3> &points,
                                        std::vector<gfx::Triangle> &triangles,
                                        const Shape::BaseShape &planet_shape)

    {
        // First create a point to tri association
        std::vector<std::vector<int>> p2triAdj(points.size());
        for (int i_tri = 0; i_tri < triangles.size(); i_tri++)
        {
            for (int i_ind = 0; i_ind < 3; i_ind++)
            {
                int i_p = triangles[i_tri][i_ind];
                auto f_it = std::find(p2triAdj[i_p].begin(), p2triAdj[i_p].end(), i_tri);
                if (f_it != p2triAdj[i_p].end())
                {
                    p2triAdj[i_p].push_back(i_tri);
                }
            }
        }

        // for each points, check if any adjacent (slightly shrunk) triangles intersect
        // if they do, then remove one...
        int found_intersecting_tris = 0;
        for (int i_p = 0; i_p < points.size(); i_p++)
        {
            // for all triangles connected to that point
            // check if they intersect each other
            for (const auto &i_t1 : p2triAdj[i_p]) {
                for (const auto &i_t2 : p2triAdj[i_p]) {
                    bool intersected = checkIfTrianglesIntersect(points, triangles[i_t1], triangles[i_t2]);
                    if (intersected)
                    {
                        found_intersecting_tris++;
                    }
                }
            }
        }
        std::cout << "found_intersecting_tris: " << found_intersecting_tris << std::endl;
    }*/
	
    inline void tripleEdgeFilterTriangles(std::vector<gfx::Triangle> &triangles)
	{
		// build edge-triangle adjacency and triangle-edge adjacency
		std::unordered_map<gfx::Line, std::vector<int>> edge_tri_adj;

		for (int i_tri = 0; i_tri < triangles.size(); ++i_tri) 
		{
			const gfx::Triangle &tri = triangles[i_tri];
			for (int j = 0; j < 3; j++)
			{
				gfx::Line line = { tri[j%3], tri[(j+1)%3] };
				if (line.indices[1] > line.indices[0]) std::swap(line.indices[0], line.indices[1]);

				auto it = edge_tri_adj.find(line);
				if (it == edge_tri_adj.end())
				{
					edge_tri_adj.insert(std::make_pair(line, std::vector<int>()));
				}
				edge_tri_adj[line].push_back(i_tri);
			}	
		}

		// for each triangle
		//   count the number of edges with more than 2 triangles adjacent
		//     if more than two edges has 2 triangles adjacent, then mark this triangle for removal
		auto rogue_tri = [&](const gfx::Triangle &tri) -> bool
		{
			int triple_edge_count = 0; // number of edges with three or more triangles
			for (int j = 0; j < 3; j++)
			{
				gfx::Line line = { tri[j%3], tri[(j+1)%3] };
				if (line.indices[1] > line.indices[0]) std::swap(line.indices[0], line.indices[1]);
				if (edge_tri_adj[line].size() > 2) ++triple_edge_count;
			}
			return triple_edge_count > 1;
		};

        // remove triangles
		int w = 0;
		for (int r = 0; r < triangles.size(); r++)
	   	{
			const gfx::Triangle &tri = triangles[r];

			// read from r, write to index w	
			if (rogue_tri(tri))
			{
				// remove it from the edge vector
				for (int j = 0; j < 3; j++)
				{
					gfx::Line line = { tri[j%3], tri[(j+1)%3] };
					if (line.indices[1] > line.indices[0]) std::swap(line.indices[0], line.indices[1]);

					auto &v = edge_tri_adj[line];
					v.erase(std::remove(v.begin(), v.end(), r), v.end());
				}
			}
			else 
			{
				// copy the read triangle to the writing position and increment the write index
				triangles[w] = triangles[r];
				++w;
			}
		}

		// resize the vector to its final size (w)
		triangles.resize(w);
	}

	std::vector<gfx::Triangle> triangulateAndOrient(const std::vector<vmath::Vector3> &points,
													const SpaceHash3D &spacehash, const Shape::BaseShape &planet_shape)
	{
        float triangulationRadius = 0.78f*scaleByPointDensity(spacehash);
        //std::cout << "n_points = " << points.size() << ", rad = " << triangulationRadius << std::endl;

		// create a triangulation
        Triangulate::ReturnType trisandhash = Triangulate::trianglesWithHash(points, triangulationRadius, spacehash);
//        Triangulate::ReturnType trisandhash = Triangulate::incrTriangulate(points, triangulationRadius, spacehash); // need to look more into this
		std::vector<gfx::Triangle> &triangles = trisandhash.triangles;
		//std::unordered_set<gfx::Triangle> &existing_tris = trisandhash.trianglesHash;

		// orient triangles
		orientTriangles(points, triangles, planet_shape);

		// filter away triangles with normals different from planet shape normal
		surfaceGradFilterTriangles(points, triangles, planet_shape);

        // Filter away triangles with more than two edges that border three triangles
        // an edge should strictly be between two triangles for "manifoldness"
        tripleEdgeFilterTriangles(triangles);


        // analyse edges
        int num_edges = 0;
        std::unordered_set<gfx::Line> existing_edges;
        std::vector<std::vector<int>> point_point_adj(points.size());
        for (const auto &tri : triangles)
        {
            for (int i=0; i<3; i++)
            {
                int i_next =(i+1)%3;
                std::array<int,2> inds = {tri[i], tri[i_next]};
                std::sort(inds.begin(), inds.end());

                gfx::Line line = {inds[0], inds[1]};

                if (existing_edges.find(line) == existing_edges.end())
                {
                    existing_edges.insert(line);
                    num_edges++;
                    point_point_adj[inds[0]].push_back(inds[1]);
                    point_point_adj[inds[1]].push_back(inds[0]);
                }
            }
        }

        // compute the euler characteristic
        int euler_characteristic = points.size()-num_edges+triangles.size();

        /*std::cout << "num points = " << points.size() << std::endl;
		std::cout << "num triangles = " << triangles.size() << std::endl;
        std::cout << "num edges = " << num_edges << std::endl;*/
		std::cout << "euler characteristic = " << euler_characteristic << std::endl;

		return triangles;
	}

    void perturbHeightNoise3D(std::vector<vmath::Vector3> &points, const Shape::BaseShape &planet_shape)
    {
        Shape::AABB aabb = planet_shape.getAABB();
        float smallest_noise_scale = 0.2f; // TODO: meters....
        Noise3D noise3d(aabb.width, aabb.height, smallest_noise_scale, 198327);

        for (auto &point : points)
        {
            float noise_sample = 0.1f*noise3d.sample(point);
            planet_shape.scalePointHeight(point, std::max(1.0f+noise_sample, 0.5f));
        }
    }

    void removePointsTooClose(std::vector<vmath::Vector3> &points, SpaceHash3D &spacehash, float min_distance)
    {
        int total_points_filtered = 0;
        bool found_none_tooclose = false;
        while (!found_none_tooclose)
        {
            found_none_tooclose = true; // assume everything is ok until proven otherwise

            std::vector<std::pair<int, int>> pairs_too_close;

            // search in a sphere around each point
            for (int i_p = 0; i_p<points.size(); i_p++)
            {
                std::vector<int> neighbors;
                spacehash.forEachPointInSphere(points[i_p], min_distance,
                                               [&](const int &i_n) -> bool // stop sphere search after first found
                                               {
                                                    if (i_n != i_p) neighbors.push_back(i_n);
                                                    return false;
                                               });

                if (neighbors.size() > 0)
                {
                    found_none_tooclose = false;
                    // pick only the closest one
                    std::sort(neighbors.begin(), neighbors.end(), [&](int i0, int i1)
                    {
                        return vmath::length(points[i0]-points[i_p]) < vmath::length(points[i1]-points[i_p]);
                    });
                    int i_n = neighbors[0];

                    auto pn_pair = (i_n > i_p) ? std::make_pair(i_p, i_n) : std::make_pair(i_n, i_p);
                    pairs_too_close.push_back(pn_pair);
                }
            }
            // at this point two copies might exist of a closest point pair, need to remove duplicates
            std::sort( pairs_too_close.begin(), pairs_too_close.end() );
            pairs_too_close.erase( std::unique( pairs_too_close.begin(), pairs_too_close.end() ), pairs_too_close.end() );

            // Only one of a "too close pair" needs to be deleted, arbitrarily pick the first one
            auto filter = [](const std::pair<int, int> &pair) -> int {return pair.first;};
            std::vector<int> to_del_inds = StdExt::vector_map<std::pair<int,int>, int>(pairs_too_close, filter);
            // why cannot the template arguments be deduced?

            StdExt::remove_index(points, to_del_inds);

            total_points_filtered += to_del_inds.size();

            spacehash.rehash(points);
        } // while found_none_tooclose

        std::cout << "filtered out " << total_points_filtered << " degenerate points" << std::endl;
    }

    void createOrLoadPlanetGeom(PlanetGeometry &alt_planet_geometry, AltPlanet::Shape::BaseShape *&planet_shape_ptr, PlanetShape shape)
    {
        std::string planet_filename;
        switch(shape)
        {
        case(PlanetShape::Sphere):
        {
            planet_shape_ptr = new AltPlanet::Shape::Sphere(3.0f);
            planet_filename = "sphere_planet.dat";
        }
        break;
        case(PlanetShape::Torus):
        {
            planet_shape_ptr = new AltPlanet::Shape::Torus(3.0f, 1.0f);
            planet_filename = "torus_planet.dat";
        }
        break;
        default:
            DEBUG_ASSERT(false&&"invalid planet shape enum");
        }
        AltPlanet::Shape::BaseShape &planet_shape = *planet_shape_ptr;

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

            // Generate geometry
            alt_planet_geometry = AltPlanet::generate(10000, planet_shape);

            // Serialize it
            try {
                Serial::serialize_to_file(alt_planet_geometry, planet_filename);
            } catch (...) {
                std::cout << "something went wrong while trying to serialize to " << planet_filename << std::endl;
            }
        }
    }

}
