#include "spacehash3d.h"

#include <iostream>

SpaceHash3D::SpaceHash3D(const std::vector<vmath::Vector3> &points) :
    mGridToPointMap(nullptr)
{
    rehash(points);
}

SpaceHash3D::~SpaceHash3D()
{
    delete [] mGridToPointMap;
}

void SpaceHash3D::rehash(const std::vector<vmath::Vector3> &points)
{
    delete [] mGridToPointMap;
    mPoints.clear();
    mPoints = points;

    // find min and max
    float low_float = std::numeric_limits<float>::lowest(); // woops woops: min!=lowest
    float max_float = std::numeric_limits<float>::max();
    mMin = {max_float, max_float, max_float};
    mMax = {low_float, low_float, low_float};
    for (const auto &p : points)
    {
        for (int i = 0; i<3; i++)
        {
            if (p[i]<mMin[i]) mMin[i] = p[i];
            if (p[i]>mMax[i]) mMax[i] = p[i];
        }
    }
    float margin_fraction = 0.5f;
    mMin = (1.0f+margin_fraction)*mMin;
    mMax = (1.0f+margin_fraction)*mMax;

    vmath::Vector3 side_lengths = mMax-mMin;
    DEBUG_ASSERT((side_lengths[0]>0.0f && side_lengths[1]>0.0f && side_lengths[2]>0.0f ));

    float volume = side_lengths[0]*side_lengths[1]*side_lengths[2];
    mPointCubeVolDensity = mPoints.size()/volume;

    float area = 2.f*(side_lengths[0]*side_lengths[1]+side_lengths[0]*side_lengths[2]+side_lengths[1]*side_lengths[2]);
    mPointCubeAreaDensity = mPoints.size()/area;

    float xy_aspect = side_lengths[1]/side_lengths[0];
    float xz_aspect = side_lengths[2]/side_lengths[0];

    mNx = cbrt((float)(mPoints.size())/(xy_aspect*xz_aspect*AV_PTS_PER_CELL_PREF));
    mNx = std::max(mNx, 1);
    mNy = std::max(int(xy_aspect*(mNx)), 1);
    mNz = std::max(int(xz_aspect*(mNx)), 1);

    /*std::cout << "min = "; vmath::print(mMin);
    std::cout << "max = "; vmath::print(mMax);
    std::cout << "nx = " << mNx << std::endl;
    std::cout << "ny = " << mNy << std::endl;
    std::cout << "nz = " << mNz << std::endl;*/

    mNGrid = mNx*mNy*mNz;
    mGridToPointMap = new std::vector<int> [mNGrid];

    for (int i = 0; i<mPoints.size(); i++)
    {
        int I = findPointCell(mPoints[i]);
        mGridToPointMap[I].push_back(i);
    }
}

float SpaceHash3D::getPointDensity() const
{
    return mPointCubeVolDensity;
}
