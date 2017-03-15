#ifndef CLIMATE_H
#define CLIMATE_H

#include "../../common/gfx_primitives.h"
#include "../../common/mathext.h"
#include "../../common/macro/macrodebugassert.h"

namespace AltPlanet {

namespace Climate {

// Humidity  ________________________________________  High
//               |            |            |
//  |  tundra    | Boreal     | Temperate  | Tropical   |
//  |            | forest     | rainforest | rainforest |
//  |            |            | (light     | (bright    |
//  |  (purple)  | (darkgreen)|  green)    |  green)    |
//  |_ _ _ _ _ _ |_ _ _ _ _ _ |____________|____________|
//  |            |            |            |            |
//  |            | Boreal     | Temperate  | Tropical   |
//  |  tundra    | forest     | seasonal   | seasonal   |
//  |            |            | forest     | savannah   |
//  |  (purple)  | (darkgreen)| (green)    | (brown)    |
//  |____________|____________|____________|____________|
//  |            |            |            |            |
//  |  arctic/   | Desert       Desert       Desert     |
//  |  rocky     | grey rock  | red rock   | dunes      |
//  |            |                                      |
//  |  (white)   |  (grey)    | (red)      | (yellow)   |
//      _________|___________________________
// Low                                         Temperature
    static const int tex_h = 3;
    static const int tex_w = 4;
    static const int tex_size = 3*tex_h*tex_w;

    struct PixelArray {
        int w, h;
        std::array<float, tex_size> pixels;
    };

    inline PixelArray createClimateZonePixels()
    {
        PixelArray out;
        out.h = 3;
        out.w = 4;
        out.pixels = {
            1.0f, 1.0f, 1.0f,  0.5f, 0.5f, 0.5f,   0.6f, 0.2f, 0.0f,  0.8f, 0.8f, 0.0f,
            0.4f, 0.0f, 0.7f,  0.0f, 0.7f, 0.3f,   0.0f, 1.0f, 0.0f,  0.6f, 0.4f, 0.3f,
            0.4f, 0.0f, 0.7f,  0.0f, 0.7f, 0.3f,   0.3f, 1.0f, 0.3f,  0.6f, 1.0f, 0.6f
        };
        return out;
    }

    inline PixelArray createClimateColorPixels()
    {
        PixelArray out;
        out.h = 3;
        out.w = 4;
        out.pixels = {
            1.00f, 1.0f, 1.0f,  0.4f, 0.5f, 0.3f,   0.8f, 0.7f, 0.3f,  0.9f, 0.9f, 0.5f,
            0.4f, 0.5f, 0.15f,  0.0f, 0.18f, 0.08f,   0.0f, 0.25f, 0.0f,  0.6f, 0.5f, 0.3f,
            0.4f, 0.5f, 0.15f,  0.0f, 0.18f, 0.08f,   0.08f, 0.22f, 0.03f,  0.0f, 0.3f, 0.0f
        };
        return out;
    }

    typedef std::array<float, 2> ClimateCoords;

    // map the irradiance to the shape of the arcsin curve to shrink arctic/tundra/desert biomes
    inline float mapIrrArcSin(float irr_in)
    {
        return (1.5f + std::asin(2.0f*irr_in-1.0f))/3.0f;
    }

    // or use the square root function! Aiai, big mistake
    //inline float mapIrrSqrt(float irr_in)
    //{
    //    return std::sqrt(irr_in);
    //}


    inline std::vector<ClimateCoords> getClimateCoords(const std::vector<float> &irradiance, const std::vector<float> &humidity)
    {
        DEBUG_ASSERT(irradiance.size()==humidity.size());
        // gief `zip` plx
        std::vector<gfx::TexCoords> out(irradiance.size());
        for (int i=0; i<irradiance.size(); i++) {
            out[i] = { mapIrrArcSin(irradiance[i]), humidity[i] };
        }

        return out;
    }

    // Heights scaled between 0=sea level and 1=highest mountain
    /*inline std::vector<float> scaledHeights(std::vector<float> heights, float sea_level)
    {
        std::vector<float> out(height.size());

        MathExt::normalizeFloatVec(heights, )

        return out;
    }*/

}

}

#endif // CLIMATE_H
