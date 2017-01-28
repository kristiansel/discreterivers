#ifndef TEXTURE_H
#define TEXTURE_H

#include "gfxcommon.h"
#include "SDL_image.h"
#include "../common/typetag.h"
#include "../common/gfx_primitives.h"
#include "../common/resmanager/refcounted.h"

namespace gfx {

struct gl_type_type_tag{};
typedef decltype(GL_UNSIGNED_BYTE) GL_TYPE_TYPE;
typedef ID<gl_type_type_tag, GL_TYPE_TYPE, GL_UNSIGNED_BYTE> gl_type;

class Texture final : public Resource::RefCounted<Texture>
{
public:
    //===============================
    // Public types                //
    //===============================
    using gl_mag_filter_t = decltype(GL_LINEAR);
    using gl_min_filter_t = decltype(GL_LINEAR_MIPMAP_LINEAR);
    enum class gl_texture_filter { nearest, linear };
    using filter = gl_texture_filter;

    using gl_pixel_format_t = decltype(GL_RGB);
    enum class gl_pixel_format { red, rgb, rgba };
    using pixel_format = gl_pixel_format;
    gl_pixel_format_t getPixelFormat(gl_pixel_format pixel_format) {
        switch (pixel_format)
        {
            case (gl_pixel_format::red):     return GL_RED;
            case (gl_pixel_format::rgb):     return GL_RGB;
            case (gl_pixel_format::rgba):    return GL_RGBA;
            default: assert((false&&"invalid texture pixel format spec"));
        }
    }

    //===============================
    // Explicit data constructors  //
    //===============================
    inline explicit Texture(const char * filename);
    inline explicit Texture(void * pixels, int w, int h, gl_type type, gl_texture_filter tex_filter,
                            gl_pixel_format internal_format = gl_pixel_format::rgb,
                            gl_pixel_format format = gl_pixel_format::rgb, bool unpack_alignment = false);

    inline explicit Texture(const vmath::Vector4 &color);

    //===============================
    // Public member functions     //
    //===============================
    inline GLuint getTextureID() const { return mTextureID; }

    // No rule of five/lifecycle methods need to be implemented
    // that is handled by Resource::RefCounted<Texture> base class

private:
    Texture();

public:
    // provide the cleanup function to be used by the
    // Resource::RefCounted<Texture> base class
    inline void resourceDestruct();

private:
    //===============================
    // Private member variables    //
    //===============================
    GLuint mTextureID; // Pointer type, Textures is not a POD

    //===============================
    // Private helper functions    //
    //===============================
    inline void loadTextureFromFile(const char * filename);
    inline void loadTextureFromPixels(void * pixels, int w, int h, gl_type type, gl_texture_filter tex_filter,
                                       gl_pixel_format internal_format = gl_pixel_format::rgb,
                                       gl_pixel_format format = gl_pixel_format::rgb, bool unpack_alignment = false);
    inline void loadDefaultTexture();
};

//==============================================================
//                                                            //
// Implementation of inline functions                         //
//                                                            //
//==============================================================

inline Texture::Texture(const char * filename)
{
    // load
    loadTextureFromFile(filename);
}

inline Texture::Texture(void * pixels, int w, int h, gl_type type, gl_texture_filter tex_filter,
                        gl_pixel_format internal_format, gl_pixel_format format, bool unpack_alignment)
{
    loadTextureFromPixels(pixels, w, h, type, tex_filter, internal_format, format, unpack_alignment);
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


inline void Texture::resourceDestruct()
{
    if (mTextureID != 0)
    {
        std::cout << "deleting texture: " << mTextureID << std::endl;
        glDeleteTextures(1, &mTextureID);
    }
}

inline void Texture::loadTextureFromPixels(void * pixels, int w, int h, gl_type type, gl_texture_filter tex_filter,
                                           gl_pixel_format internal_format, gl_pixel_format format, bool unpack_alignment)
{
    // Create one OpenGL texture
    glGenTextures(1, &mTextureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, mTextureID);

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

    if (unpack_alignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, getPixelFormat(internal_format), w, h, 0, getPixelFormat(format), GL_TYPE_TYPE(type), pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
}

inline void Texture::loadTextureFromFile(const char * filename)
{
    std::cout << "creating texture " << filename << std::endl;

    SDL_Surface *image;
    image = IMG_Load(filename);

    if(image)
    {
        // report success
        std::cout << filename << " successfully loaded" << std::endl;
        //std::cout << "height: " << image->h << std::endl;
        //std::cout << "width: " << image->w << std::endl;
        //std::cout << "pitch: " << image->pitch << std::endl;

        loadTextureFromPixels(image->pixels, image->w, image->h, gl_type(GL_UNSIGNED_BYTE), gl_texture_filter::linear);
    }
    else
    {
        // image failed to load
        std::cerr << "IMG_Load: " << IMG_GetError() << std::endl;

        // load backup texture
        loadDefaultTexture();
    }

    // free the loaded image (if loaded, if not, should be safe to free null)
    SDL_FreeSurface(image);
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
