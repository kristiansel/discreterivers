#ifndef TEXTURE_H
#define TEXTURE_H

#include "gfxcommon.h"
#include "SDL_image.h"
#include "../common/typetag.h"

namespace gfx {

class Texture
{
public:
    inline explicit Texture(const char * filename) { loadTextureFromFile(filename); }
    inline explicit Texture(const vmath::Vector4 &color);

    Texture()
    {
        loadTextureFromFile("planet_terrain.jpg");
        std::cout << "Texture() default constructed: " << mTextureID << std::endl;
    }

    /*
    Texture(const Texture &tex) : mTextureID(tex.mTextureID)
    {
        // weak pointer sematics, no new construction
        std::cout << "Texture(const Texture &tex) copy from " << tex.mTextureID << ", to " << mTextureID << std::endl;
    }*/

    /*
    Texture(Texture &&tex) : mTextureID(tex.mTextureID)
    {
        // should delete other...
        std::cout << "Texture(Texture &&tex) move from " << tex.mTextureID << ", to " << mTextureID << std::endl;
    }*/


    /** Copy assignment operator */
    /*Texture& operator= (const Texture& other)
    {
        Texture tmp(other);         // re-use copy-constructor
        *this = std::move(tmp);     // re-use move-assignment
        return *this;
    }*/

    /** Move assignment operator */
    /*Texture& operator= (Texture&& other) noexcept
    {
        // leak current texture
        mTextureID = other.mTextureID;
        // set other texture id to null...
        return *this;
    }*/


    inline GLuint getTextureID() const {return mTextureID;}
    /*inline gl_texture_type getTextureType() const   {return mTextureType;}*/

private:
    inline void loadTextureFromFile(const char * filename);
    inline void loadTextureFromPixels(void * pixels, int w, int h);

    GLuint mTextureID; // Pointer type, Textures is not a POD
    /*gl_texture_type mTextureType;*/
};

inline void Texture::loadTextureFromPixels(void * pixels, int w, int h)
{
    // Create one OpenGL texture
    glGenTextures(1, &mTextureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, pixels);

    // Nice trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
}

inline void Texture::loadTextureFromFile(const char * filename)
{
    std::cout << "creating texture " << filename << std::endl;

    SDL_Surface *image;
    image = IMG_Load(filename);
    if(!image)
    {
        // image failed to load
        std::cout << "IMG_Load: " << IMG_GetError() << std::endl;
        std::cout << filename << " format not supported, or the image data is corrupted" << std::endl;
        assert(false);
    }
    // report success
    std::cout << filename << " successfully loaded" << std::endl;
    std::cout << "height: " << image->h << std::endl;
    std::cout << "width: " << image->w << std::endl;
    std::cout << "pitch: " << image->pitch << std::endl;

    loadTextureFromPixels(image->pixels, image->w, image->h);

    // free the loaded image
    SDL_FreeSurface(image);
}

inline Texture::Texture(const vmath::Vector4 &color)
{
    const auto &c = color;
    float pixels[] = {
        c[0], c[1], c[2],   c[0], c[1], c[2],
        c[0], c[1], c[2],   c[0], c[1], c[2],
    };

    int w = 2;
    int h = 2;

    /*float pixels[] = {
        1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
    }; // this doesn't work as expected even though texture coordinates are correct..

    int w = 2;
    int h = 2;*/

    loadTextureFromPixels(pixels, w, h);
}



} // namespace gfx

#endif // TEXTURE_H
