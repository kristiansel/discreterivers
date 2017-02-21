#include "noise3d.h"
#include "../mathext.h"
#include "../interpolation.hpp"

#include <algorithm>

using namespace MathExt;

inline int getNumSidePts(int grid_lvl)
{
    return 1 + pow(2, grid_lvl); // is this double -> int conversion a timebomb?
}

inline int getNumGridPts(int grid_lvl)
{
    int num_side = getNumSidePts(grid_lvl);
    return num_side * num_side * num_side;
}

Noise3D::Noise3D(float width, float height, float min_noise_scale, int seed) :
    mMaxDim(std::max(width, height))
{
    // Seed the pseudo-random number generator
    //srand(seed);

    // find min and max points
    float half_max_dim = mMaxDim/2.0f;
    mMax = vmath::Vector3(half_max_dim, half_max_dim, half_max_dim);
    mMin = -mMax;

    // construct the grid
    mNumGridLevels = ceil(log2(mMaxDim/min_noise_scale));
    mGridLevels = new float* [mNumGridLevels];

    //std::cout << "mNumGridLevels: " << mNumGridLevels << std::endl;

    for (int i_lvl = 0; i_lvl<mNumGridLevels; i_lvl++)
    {
        float* &gridPts = mGridLevels[i_lvl];
        int num_grid_pts = getNumGridPts(i_lvl);
        gridPts = new float [num_grid_pts];

        float noise_lvl_scalefactor = i_lvl == 0 ? 0.0f : 1.0f/static_cast<float>(i_lvl);
        // assign the grid points random value
        for (int i_pts = 0; i_pts<num_grid_pts; i_pts++)
        {
            gridPts[i_pts] = frand(-noise_lvl_scalefactor, noise_lvl_scalefactor);
        }
    }
}

Noise3D::~Noise3D()
{
    for (int i_gridlvl = 0; i_gridlvl<mNumGridLevels; i_gridlvl++)
    {
        delete [] mGridLevels[i_gridlvl];
    }

    delete [] mGridLevels;
}

inline Noise3D::ijk Noise3D::ijkFromPoint(const vmath::Vector3 &point, int num_side_pts)
{
    ijk out;
    for (int n = 0; n<3; n++)
        out.inds[n] = MathExt::wrap(floor((point[n]-mMin[n])/(mMax[n]-mMin[n])*float(num_side_pts)), num_side_pts);

    return out;
}

inline int Noise3D::localToGlobal(const ijk &indices, int num_side_pts)
{
    return indices.inds[0] + indices.inds[1] * num_side_pts + indices.inds[2] * num_side_pts * num_side_pts;
}

inline vmath::Vector3 Noise3D::findGridPtLocation(const ijk &indices, int num_side_pts)
{
    float l = mMaxDim/static_cast<float>(num_side_pts);
    return mMin + vmath::Vector3(indices.inds[0]*l, indices.inds[1]*l, indices.inds[2]*l);
}

float Noise3D::sample(const vmath::Vector3 &point)
{
    // find the corresponding grid cell corners at all grid levels
    // interpolate grid noise values to position in cell
    // sum interpolated grid values
    float sum = 0.0f;
    for (int i_lvl = 0; i_lvl<mNumGridLevels; i_lvl++)
    {
        float const * const &gridPts = mGridLevels[i_lvl];
        int num_side_pts = getNumSidePts(i_lvl);

        ijk indices = ijkFromPoint(point, num_side_pts);

        float p[4][4][4];
        for (int d_i = 0; d_i<4; d_i++)
        {
            int i = MathExt::wrap(indices.inds[0]+(d_i-1), num_side_pts);
            for (int d_j = 0; d_j<4; d_j++)
            {
                int j = MathExt::wrap(indices.inds[1]+(d_j-1), num_side_pts);
                for (int d_k = 0; d_k<4; d_k++)
                {
                    int k = MathExt::wrap(indices.inds[2]+(d_k-1), num_side_pts);

                    int I = localToGlobal({i, j, k}, num_side_pts);

                    p[d_i][d_j][d_k] = gridPts[I];
                }
            }
        }

        float cell_side_length = mMaxDim/static_cast<float>(num_side_pts);
        vmath::Vector3 point_cell_diff = point - findGridPtLocation(indices, num_side_pts);

        float tx = point_cell_diff[0]/cell_side_length;
        float ty = point_cell_diff[1]/cell_side_length;
        float tz = point_cell_diff[2]/cell_side_length;

        float term = interpolate::tricubic(p, tx, ty, tz);

        sum +=term;
    }

    return sum;
}
