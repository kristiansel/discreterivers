#ifndef CLIMATE_H
#define CLIMATE_H

#include "../common/gfx_primitives.h"

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

    inline PixelArray createClimatePixels()
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

    inline std::vector<gfx::TexCoords> getClimateTexcoords(const std::vector<float> &irradiance, const std::vector<float> &humidity)
    {
        assert((irradiance.size()==humidity.size()));
        // gief `zip` plx
        std::vector<gfx::TexCoords> out(irradiance.size());
        for (int i=0; i<irradiance.size(); i++) {
            out[i] = { irradiance[i], humidity[i] };
        }

        return out;
    }

}

}

#endif // CLIMATE_H
