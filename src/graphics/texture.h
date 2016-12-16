#ifndef TEXTURE_H
#define TEXTURE_H

#include "gfxcommon.h"
#include "SDL_image.h"
#include "../common/typetag.h"

namespace gfx {

struct gl_type_type_tag{};
typedef decltype(GL_UNSIGNED_BYTE) GL_TYPE_TYPE;
typedef ID<gl_type_type_tag, GL_TYPE_TYPE, GL_UNSIGNED_BYTE> gl_type;


class Texture
{
public:
    using gl_mag_filter_t = decltype(GL_LINEAR);
    using gl_min_filter_t = decltype(GL_LINEAR_MIPMAP_LINEAR);
    enum class gl_texture_filter { nearest, linear };
    using filter = gl_texture_filter;

    inline explicit Texture(const char * filename) { loadTextureFromFile(filename); }
    inline explicit Texture(void * pixels, int w, int h, gl_type type, gl_texture_filter tex_filter)
    {
        loadTextureFromPixels(pixels, w, h, type, tex_filter);
    }
    inline explicit Texture(const vmath::Vector4 &color);

    Texture()
        { loadDefaultTexture(); std::cout << "Texture DEFAULT constructed: " << mTextureID << std::endl; }

    Texture(const Texture &tx) : mTextureID(tx.mTextureID)
        { std::cout << "Texture COPY constructed" << mTextureID << std::endl; }

    Texture(Texture &&tx)  : mTextureID(tx.mTextureID)
        { std::cout << "Texture MOVE constructed: " << mTextureID << std::endl; }

    Texture& operator= (const Texture& tx)
    {
        Texture tmp(tx);         // re-use copy-constructor
        *this = std::move(tmp);     // re-use move-assignment
        std::cout << "Texture COPY ASSIGN operator: " << mTextureID << std::endl;
        return *this;
    }

    /** Move assignment operator */
    Texture& operator= (Texture&& tx) noexcept
    {
        //delete[] data;
        mTextureID = tx.mTextureID; //data = other.data;
        //other.data = nullptr;
        std::cout << "Texture MOVE ASSIGN operator: " << mTextureID << std::endl;
        return *this;
    }

    inline GLuint getTextureID() const {return mTextureID;}
    /*inline gl_texture_type getTextureType() const   {return mTextureType;}*/
private:
    inline void loadTextureFromFile(const char * filename);
    inline void loadTextureFromPixels(void * pixels, int w, int h, gl_type type, gl_texture_filter tex_filter);
    inline void loadDefaultTexture();

    GLuint mTextureID; // Pointer type, Textures is not a POD
    /*gl_texture_type mTextureType;*/
};

inline void Texture::loadTextureFromPixels(void * pixels, int w, int h, gl_type type, gl_texture_filter tex_filter)
{
    // Create one OpenGL texture
    glGenTextures(1, &mTextureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_TYPE_TYPE(type), pixels);

    gl_mag_filter_t mag_filter;
    gl_min_filter_t min_filter;
    switch (tex_filter)
    {
        case (gl_texture_filter::nearest):
            mag_filter = GL_NEAREST;
            min_filter = GL_NEAREST_MIPMAP_NEAREST;
            break;
        case (gl_texture_filter::linear):
            mag_filter = GL_LINEAR;
            min_filter = GL_LINEAR_MIPMAP_LINEAR;
            break;
        default: // should be unreachable
            assert((false&&"invalid texture filtering spec"));
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
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

    loadTextureFromPixels(image->pixels, image->w, image->h, gl_type(GL_UNSIGNED_BYTE), gl_texture_filter::linear);

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

    loadTextureFromPixels(pixels, w, h, gl_type(GL_FLOAT), gl_texture_filter::linear);

    //loadTextureFromFile("planet_terrain.jpg");
}

inline void Texture::loadDefaultTexture()
{
    float pixels[] = {
        1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
    };

    int w = 2;
    int h = 2;

    loadTextureFromPixels(pixels, w, h, gl_type(GL_FLOAT), gl_texture_filter::linear);
}

} // namespace gfx

#endif // TEXTURE_H
