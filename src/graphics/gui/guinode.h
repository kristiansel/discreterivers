#ifndef GUINODE_H
#define GUINODE_H

#include <list>
#include <iostream>

#include "guitransform.h"
#include "guielement.h"
#include "guitextvertices.h"
#include "guifontrenderer.h"
#include "../texture.h"
#include "../../common/gfx_primitives.h"
#include "../../common/resmanager/refcounted.h"


namespace gfx {

class OpenGLRenderer;

namespace gui {

class GUINode : public Resource::RefCounted<GUINode>
{
public: // practically immutable?
    inline GUINode(vmath::Vector4 &&color, GUITransform &&gui_transform, const GUIFontRenderer * const font_renderer, const std::string &text = "",
            std::initializer_list<GUINode> &&children = {}, const Texture &texture = Texture(vmath::Vector4(1.0, 0.0, 0.0, 1.0)));

    // vector init needs copy constructor... strange? Ah, it is because of initializer list
    // elements of the list are always passed as const reference 18.9 in standard

    GUINode(GUINode &&gn) : Resource::RefCounted<GUINode>(std::move(gn)),
        mColor(gn.mColor), mGUITransform(gn.mGUITransform), /*mText(std::move(gn.mText)),*/
        mChildren(std::move(gn.mChildren)), mTexture(std::move(gn.mTexture)), mGUITextVertices(std::move(gn.mGUITextVertices)),
        mFontTextureAtlas(std::move(gn.mFontTextureAtlas))
    {}

    GUINode(const GUINode &gn) = default;
    //GUINode(const GUINode &gn) : Resource::RefCounted<GUINode>(gn),
    //    mColor(gn.mColor), mGUITransform(gn.mGUITransform), mChildren(gn.mChildren),
    //    mText(gn.mText) { /*std::cout << "GUINode COPY constructor called" << std::endl;*/ }

    // TODO: move assign operator etc, needed?

    inline const GUITransform &getTransform() const { return mGUITransform; }
    inline const vmath::Vector4 &getColor() const { return mColor; }
    inline const std::vector<GUINode> &getChildren() const { return mChildren; }
    inline const GLuint getTextureID() const { return mTexture.getTextureID(); }

    inline const GUITextVertices &getGUITextVertices() const { return mGUITextVertices; }
    inline const GLuint getFontAtlasTextureID() const { return mFontTextureAtlas.getTextureID(); }

    // Resource::RefCounted<GUINode>
    void resourceDestruct() { std::cout << "deleting gui node" << std::endl; }

private:
    GUINode();

    // private some operators?

    std::vector<GUINode> mChildren;

    // TODO: Get rid of the text, convert it to vertices upon construction
    //std::string mText;

    vmath::Vector4 mColor;

    GUITransform mGUITransform;
    Texture mTexture;

    GUITextVertices mGUITextVertices;
    Texture mFontTextureAtlas;

};

inline GUINode::GUINode(vmath::Vector4 &&color, GUITransform &&gui_transform,
                        const GUIFontRenderer * const font_renderer, const std::string &text,
                        std::initializer_list<GUINode> &&children, const Texture &texture) :
    mColor(std::move(color)), mGUITransform(std::move(gui_transform)), /*mText(std::move(text)),*/
    mChildren(std::move(children)), mTexture(texture), mGUITextVertices(font_renderer->render(text, gui_transform)),
    mFontTextureAtlas(font_renderer->getTextureAtlas())
{
    /*std::cout << "creating gui node" << std::endl;
    std::cout << "creating gui node" << std::endl;*/
}

} // gui

} // gfx

#endif // GUINODE_H
