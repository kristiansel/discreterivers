#ifndef NOISE3D_H
#define NOISE3D_H

#define _VECTORMATH_DEBUG
#include "../../dep/vecmath/vectormath_aos.h"
namespace vmath = Vectormath::Aos;

#include <vector>

// TODO: Template this on dimension size NoiseND<int N>

class Noise3D
{
public:
    Noise3D(float width, float height, float min_noise_scale, int seed);
    ~Noise3D();

    float sample(const vmath::Vector3 &point);
protected:
    struct ijk {int inds[3];};
    inline ijk ijkFromPoint(const vmath::Vector3 &point, int num_side_pts);
    inline int localToGlobal(const ijk &indices, int num_side_pts);
    inline vmath::Vector3 findGridPtLocation(const ijk &indices, int num_side_pts);
private:
    float ** mGridLevels;
    int mNumGridLevels;

    float mMaxDim;
    vmath::Vector3 mMin;
    vmath::Vector3 mMax;
};

#endif // NOISE3D_H
