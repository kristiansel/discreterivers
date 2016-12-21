#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <algorithm>
#include <iostream>

#include "../common/gfx_primitives.h"
#include "../common/typetag.h"
#include "../common/macro/macrodebugassert.h"
#include "primitives.h"
#include "geometry.h"
#include "material.h"
#include "transform.h"
#include "shader.h"
#include "renderflags.h"
#include "gui/guinode.h"
#include "gui/guishader.h"
#include "gui/guifontrenderer.h"

namespace vmath = Vectormath::Aos;

namespace gfx
{

struct sceneobject_tag{};
typedef ID<sceneobject_tag, int, -1> sceneobject_id;

struct light_tag{};
typedef ID<light_tag, int, -1> light_id;

struct scenenode_tag{};
typedef ID<scenenode_tag, int, -1> scenenode_id;


struct SceneNode;
struct Light;

struct SceneNodeHandle;
struct LightHandle;

class OpenGLContextDependent // base class that constructs opengl context before derived is constructed
{
public:
    OpenGLContextDependent() // create OpenGL context using GLEW
    {
        // glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
          // Problem: glewInit failed, something is seriously wrong.
          std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
          DEBUG_ASSERT((GLEW_OK != err));
        }
        std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    }
};

class OpenGLRenderer : public OpenGLContextDependent // Need to get a context before constructing OpenGLRenderer
{
public:
    OpenGLRenderer(int w, int h);

    SceneNodeHandle addSceneNode();

    //inline void addGUINode(vmath::Vector4 &&color, gui::GUITransform &&gui_transform);

    //template <typename ...Args>
    //inline void addGUINode(Args... args);

    inline void addGUINode(vmath::Vector4 &&color, gui::GUITransform &&gui_transform, std::string &&text = "",
                std::initializer_list<gui::GUINode> &&children = {});

    SceneNode * getSceneNodePtr(scenenode_id id);

    void toggleWireframe() { mRenderFlags.toggleFlag(RenderFlags::Wireframe); }

    void draw(const Camera &camera) const;

    void resize(int w, int h);

private:
    // global render flags
    RenderFlags mRenderFlags;

    // Main scene shader stuff
    Shader mMainShader;

    // should be shared among shaders
    struct LightObject
    {
        vmath::Vector4 mPosition;
        vmath::Vector4 mColor;
    };

    std::vector<SceneNode> mSceneNodesVector;
    mutable std::vector<LightObject> mLightObjectsVector;

    // GUI shader stuff
    gui::GUIShader mGUIShader;

    std::vector<gui::GUINode> mGUINodesVector;

    gui::GUIFontRenderer mGUIFontRenderer; // move this!
};

class SceneObject
{
public:
    SceneObject(const Material &material, const Geometry &geometry)
        : mMaterial(material), mGeometry(geometry) {}

    void toggleVisible() { mFlags.toggleFlag(RenderFlags::Hidden); }
    void setWireframe(bool w) { w ? mFlags.setFlag(RenderFlags::Wireframe) : mFlags.clearFlag(RenderFlags::Wireframe); }

    RenderFlags getRenderFlags() const { return mFlags; }

    Material mMaterial;
    Geometry mGeometry;
    RenderFlags mFlags;
private:
    SceneObject();
};


struct SceneNodeHandle
{
    SceneNodeHandle(SceneNodeHandle&& o) :
        mOpenGLRenderer(std::move(o.mOpenGLRenderer)), mID(std::move(o.mID)) {}

    SceneNodeHandle(OpenGLRenderer * const opengl_renderer, scenenode_id id) :
        mOpenGLRenderer(opengl_renderer), mID(id) {}

    SceneNode& operator*() const { return *(mOpenGLRenderer->getSceneNodePtr(mID)); }
    SceneNode* operator->() const { return mOpenGLRenderer->getSceneNodePtr(mID); }

    scenenode_id getSceneNodeID() const {return mID;}

private:
    SceneNodeHandle();

    OpenGLRenderer * const mOpenGLRenderer;
    scenenode_id mID;
};


struct SceneObjectHandle;

struct SceneNode
{
    Transform transform;

    SceneObjectHandle addSceneObject(const Geometry &geometry,
                                  const Material &material,
                                  const Transform &transform = Transform());

    LightHandle addLight(const vmath::Vector4 &color,
                         const Transform &transform = Transform());

    SceneObject * getSceneObjectPtr(sceneobject_id id);

    Light * getLightPtr(light_id id);

    const std::vector<SceneObject> &getSceneObjects() const {return mSceneObjects;}
    const std::vector<Light> &getLights() const {return mLights;}

private:
    std::vector<SceneObject> mSceneObjects;
    std::vector<Light> mLights;

    scenenode_id mParent;
};


struct SceneObjectHandle
{
    SceneObjectHandle(const SceneObjectHandle &so) :
        mSceneNode(so.mSceneNode), mID(so.mID) {}

    SceneObjectHandle(SceneObjectHandle&& so) :
        mSceneNode(std::move(so.mSceneNode)), mID(std::move(so.mID)) {}

    SceneObjectHandle(SceneNode * const scene_node, sceneobject_id id) :
        mSceneNode(scene_node), mID(id) {}

    SceneObjectHandle& operator=(const SceneObjectHandle &so)
        {mSceneNode = so.mSceneNode; mID = so.mID;}

    SceneObject& operator*() const { assert (mSceneNode); return *(mSceneNode->getSceneObjectPtr(mID)); }
    SceneObject* operator->() const { assert (mSceneNode); return mSceneNode->getSceneObjectPtr(mID); }

    sceneobject_id getSceneObjectID() const {return mID;}

private:
    SceneObjectHandle();

    SceneNode * mSceneNode;
    sceneobject_id mID;
};

struct Light
{
    Light(const Transform &transform, const vmath::Vector4 &color)
        : mTransform(transform), mColor(color) {}

    Transform mTransform;
    vmath::Vector4 mColor;
};

struct LightHandle
{
    LightHandle() :
        mSceneNode(nullptr), mID(0) {}

    LightHandle(const LightHandle &l) :
        mSceneNode(l.mSceneNode), mID(l.mID) {}

    LightHandle(SceneNode * const scene_node, light_id id) :
        mSceneNode(scene_node), mID(id) {}

    LightHandle& operator=(const LightHandle &l)
        {mSceneNode = l.mSceneNode; mID = l.mID;}

    Light& operator*() const { assert (mSceneNode); return *(mSceneNode->getLightPtr(mID)); }
    Light* operator->() const { assert (mSceneNode); return mSceneNode->getLightPtr(mID); }

    light_id getLightID() const {return mID;}

private:

    SceneNode * mSceneNode;
    light_id mID;
};


/*inline void OpenGLRenderer::addGUINode(vmath::Vector4 &&color, gui::GUITransform && gui_transform)
{
    mGUINodesVector.emplace_back(std::move(color), std::move(gui_transform));
}*/


//template <typename ...Args>
//inline void OpenGLRenderer::addGUINode(Args... args)
//{
//    mGUINodesVector.emplace_back( std::forward<Args>(args)... );
//}

// nice template doesn't deduce type arguments.... :(

// "std::forward essentially does template type deduction from function call. Scott Meyers describes in his
// Effective Modern C++ book this situation as one of the perfect forwarding failure cases. As he said in the
// book, one simple workaround is to use auto:"

inline void OpenGLRenderer::addGUINode(vmath::Vector4 &&color, gui::GUITransform &&gui_transform, std::string &&text,
            std::initializer_list<gui::GUINode> &&children)
{
    mGUINodesVector.emplace_back(std::move(color), std::move(gui_transform), std::move(text), std::move(children));
}


} // namespace gfx
#endif // OPENGLRENDERER_H
