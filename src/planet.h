#ifndef PLANET_H
#define PLANET_H

#include <vector>
#include <array>
#include <iostream>
#include "gfx_primitives.h"
#include "typetag.h"


// don't mess with working code..
//struct point_tag{};
//typedef ID<point_tag, int, -1> point_index;

// should ideally type-safe this...
typedef int point_index;
typedef int watershed_index;
typedef int saddle_index;
typedef int minimum_index;
typedef int topological_index;

struct triangle_tag{};
typedef ID<triangle_tag, int, -1> tri_index;

typedef std::vector<point_index> ConnectionList;
typedef std::vector<saddle_index> SaddleList;
typedef std::vector<minimum_index> MinimaList;

enum class TopologicalType;
struct TopoTypeIndexPair
{
    TopologicalType topo_type;
    topological_index topo_index;
};

enum class LandWaterType
{
    Sea,
    River,
    Lake,
    Land
};

class Planet
{
public:
    Planet(const float radius,
           const int subdivision_level,
           const float ocean_fraction,
           const unsigned int n_springs,
           const int seed);



    const std::vector<Vectormath::Aos::Vector4> * const getPointsPtr(); // non-const because of lazy setting w=1 for all points
    const std::vector<gfx::Triangle> * const getTrianglesPtr() const {return &mTriangles;}

    const std::vector<gfx::Triangle> * const getSubdTrianglesPtr(int subd_lvl) const {assert((subd_lvl>=0 && subd_lvl<mSubdTriangles.size()));
                                                                                      return &mSubdTriangles[subd_lvl];}

    const std::vector<Vectormath::Aos::Vector4> * const getOceanVerticesPtr(); // non-const because of lazy setting w=1 for all points
    const std::vector<gfx::Triangle> * const getOceanTrianglesPtr() const {return &mOceanTriangles;}

    const std::vector<Vectormath::Aos::Vector4> * const getLakeVerticesPtr(); // non-const because of lazy setting w=1 for all points
    const std::vector<gfx::Triangle> * const getLakeTrianglesPtr() const {return &mLakeTriangles;}

    //const std::vector<Vectormath::Aos::Vector4> * const getRidgePointsPtr(); // non-const because of lazy setting w=1 for all points
    //const std::vector<gfx::Line> * const getRidgeLinesPtr() const {return &mRidgeLines;}

    //const std::vector<Vectormath::Aos::Vector4> * const getMinimaPointsPtr(); // non-const because of lazy evaluation
    const std::vector<gfx::Point> * const getMinimaPointsPtr() const {return (std::vector<gfx::Point> *)&mMinima;}

    const std::vector<gfx::Line> * const getFlowLinesPtr();

    const std::vector<gfx::Line> * const getRiverLinesPtr() {return &mRiverLines;}

    inline const std::vector<ConnectionList> &getAdjacency() {return mPointToPointAdjacencyList;}
    inline const std::vector<ConnectionList> &getFlowDownAdjacency() {return mFlowDownAdjacency;}
    inline const std::vector<ConnectionList> &getFlowUpAdjacency() {return mFlowUpAdjacency;}
    inline const std::vector<Vectormath::Aos::Vector3> &getPoints() {return mPoints;}


    template<class FilterFunc, class ... Args>
    void getLinePrimitives(std::vector<gfx::Line> * const primitives_data,
                          const std::vector<std::vector<int>> &full_adjacency,
                          FilterFunc filter_func,
                          Args ...args);

    template<class FilterFunc, class ... Args>
    void getPointPrimitives(std::vector<gfx::Point> * const primitives_data,
                           const std::vector<std::vector<int>> &full_adjacency,
                           FilterFunc filter_func,
                           Args ... args);
protected:
    // protected functions

    float getHeightAtPoint(Vectormath::Aos::Vector3 point, tri_index guess_triangle);

    void smoothSubdivideTriangle( std::vector<Vectormath::Aos::Vector3> * subd_points,
                                std::vector<gfx::Triangle> * subd_triangles,
                                tri_index tri_to_subd);


private:
    // graphics/shared data
    std::vector<Vectormath::Aos::Vector3> mPoints;
    std::vector<Vectormath::Aos::Vector3> mNormals;
    std::vector<gfx::Triangle> mTriangles;
    std::vector<std::vector<gfx::Triangle>> mSubdTriangles;

    // graphics/shared data
    std::vector<Vectormath::Aos::Vector3> mOceanPoints;
    std::vector<gfx::Triangle> mOceanTriangles;

    // graphics/shared data
    std::vector<Vectormath::Aos::Vector3> mLakePoints;
    std::vector<gfx::Triangle> mLakeTriangles;

    // graphics/shared data
    // std::vector<Vectormath::Aos::Vector3> mRiverPoints; // not needed yet since rivers are at terrain position
    std::vector<gfx::Line> mRiverLines;

    // graphics ridges
    // std::vector<Vectormath::Aos::Vector3> mRidgePoints;
    // std::vector<gfx::Line> mRidgeLines;

    std::vector<LandWaterType> mPointsLandWaterType;

//    bool mPointsDirty;
//    std::vector<Vectormath::Aos::Vector4> mVertices;

    // graph/topology related
    // bidirected graph of all connected points
    std::vector<ConnectionList> mPointToPointAdjacencyList;           // maps to mPoints

    std::vector<std::vector<tri_index>> mPointToTriAdjacencyList;     // maps to mPoints
    std::vector<std::vector<tri_index>> mTriToTriAdjacencyList;       // maps to mTriangles

    // unidirectional graph of points connected by preferred flow (down) direction
    std::vector<ConnectionList> mFlowDownAdjacency;

    // reverse of preferred flow direction graph
    std::vector<ConnectionList> mFlowUpAdjacency;

    // graphics maxima
    std::vector<Vectormath::Aos::Vector3> mMinimaPoints;

    // graphics flow graph
    std::vector<gfx::Line> mFlowLines;

    // two-way lookup of topological type:
    // point->type
    std::vector<TopoTypeIndexPair> mPointsTopologicalType;    // maps to mPoints

    // type->points
    std::vector<point_index> mMaxima;
    std::vector<point_index> mMinima;
    std::vector<point_index> mSaddles;

    // two-way lookup of watershed
    // point->watershed
//    std::vector<watershed_index> mPointWatersheds;          // maps to mPoints

    // watershed->sub-watershed->...->points
    std::vector<SaddleList> mMinimumToSaddleAdjecency;    // maps to mMinima
    std::vector<MinimaList> mSaddleToMinimumAdjecency;   // maps to mSaddles

};

// Template methods

template<class FilterFunc, class ... Args>
void Planet::getLinePrimitives(std::vector<gfx::Line> * const primitives_data,
                      const std::vector<std::vector<int>> &full_adjacency,
                      FilterFunc filter_func,
                      Args ...args)
{
    // assert both same size etc...

    auto filter_func_index = [&](int i) {return filter_func( args[i]... );};

    for (int i = 0; i<full_adjacency.size(); i++)
    {
        for (const auto i_adj : full_adjacency[i])
        {
            if (filter_func_index(i) && filter_func_index(i_adj)) primitives_data->push_back({i, i_adj});
        }
    }
}

template<class FilterFunc, class ... Args>
void Planet::getPointPrimitives(std::vector<gfx::Point> * const primitives_data,
                       const std::vector<std::vector<int>> &full_adjacency,
                       FilterFunc filter_func,
                       Args ...args)
{
    // assert both same size etc...

    auto filter_func_index = [&](int i) {return filter_func( args[i]... );};

    for (int i = 0; i<full_adjacency.size(); i++)
    {
        if (filter_func_index(i)) primitives_data->push_back({i});
    }

//    std::cout << "getPoints returning " << primitives_data->size() << " points" << std::endl;
//    if (primitives_data->size()<10)
//    {
//        for (const auto &prim_ind : *primitives_data)
//        {
//            std::cout << " point: " << prim_ind.index << std::endl;
//        }
//    }
}

#endif // PLANET_H
