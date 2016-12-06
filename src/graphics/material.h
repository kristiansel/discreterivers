#ifndef MATERIAL_H
#define MATERIAL_H

#include "../common/gfx_primitives.h"
#include "texture.h"

namespace gfx {

class Material
{
private:
    /*Material() : mColor(1.0f), mWireframe(false), mVisible(true), mTexture(Texture())
    {

    }*/

public:
    explicit Material(const vmath::Vector4 &color_in) : mColor(color_in),
        mWireframe(false), mVisible(true), mTexture(Texture()) {}




    /*Material(const Material &m) : mColor(m.mColor), mWireframe(m.mWireframe),
        mVisible(m.mVisible), mTexture(m.mTexture)
    {
        std::cout << "Material(const Material &m): " << m.mTexture.mTextureID << std::endl;
        std::cout << "Material(const Material &m): " << mTexture.mTextureID << std::endl;
    }*/

    /*
    Material(Material &&m) : mColor(m.mColor), mWireframe(m.mWireframe),
        mVisible(m.mVisible), mTexture(std::move(mTexture))
    {

    }*/

    /** Copy assignment operator */
    /*Material& operator= (const Material& other)
    {
        Material tmp(other);         // re-use copy-constructor
        *this = std::move(tmp);      // re-use move-assignment
        return *this;
    }*/

    /** Move assignment operator */
    /*Material& operator= (Material&& other) noexcept
    {
        // delete[] data;
        mTexture = other.mTexture;
        // other.data = nullptr;
        return *this;
    }*/


    // get
    inline const vmath::Vector4 &getColor() const {return mColor;}
    inline const bool &getWireframe() const {return mWireframe;}
    inline const bool &getVisible() const {return mVisible;}
    inline const Texture &getTexture() const {return mTexture;}

    // set
    inline void setColor(const vmath::Vector4 &color_in) {mColor=color_in;}
    inline void setWireframe(bool w) {mWireframe=w;}
    inline void setVisible(bool v) {mVisible=v;}

private:
    vmath::Vector4 mColor;
    Texture mTexture;

    bool mWireframe;
    bool mVisible;
};

}

#endif // MATERIAL_H
