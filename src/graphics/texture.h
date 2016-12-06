#ifndef TEXTURE_H
#define TEXTURE_H

#include "gfxcommon.h"
#include "SDL_image.h"

namespace gfx {

class Texture
{
public:
    explicit Texture(const char * filename) { loadTextureFromFile(filename); }

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
    void loadTextureFromFile(const char * filename);

    GLuint mTextureID; // Pointer type, Textures is not a POD
    /*gl_texture_type mTextureType;*/
};


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

    // Create one OpenGL texture
    glGenTextures(1, &mTextureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    // Nice trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    // free the loaded image
    SDL_FreeSurface(image);
}

} // namespace gfx

#endif // TEXTURE_H
