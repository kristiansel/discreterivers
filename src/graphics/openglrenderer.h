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
#include "primitives.h"
#include "geometry.h"
#include "material.h"
#include "transform.h"

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

class OpenGLRenderer
{
public:
    OpenGLRenderer(int width, int height);

    SceneNodeHandle addSceneNode();

    SceneNode * getSceneNodePtr(scenenode_id id);

    void toggleWireframe() { mGlobalWireframe = mGlobalWireframe ? false : true; }

    void draw() const;


private:
//    class ShaderProgram
//    {
//        friend class OpenGLRenderer;
        GLuint mShaderProgramID;
        struct Uniforms
        {
            GLint mv;
            GLint p;
            GLint color;
            GLint light_position;
            GLint light_color;
        } mUniforms;
//    } mShaderProgram;

    static bool checkShaderCompiled(GLuint shader);
    static bool checkProgramLinked(GLuint program);

    bool mGlobalWireframe;

    struct Camera
    {
        vmath::Matrix4 mProjectionMatrix;
        vmath::Matrix4 mCamMatrixInverse;
    } mCamera;

    struct DrawObject
    {
        vmath::Matrix4 mMatrix;
        Material mMaterial;
        Geometry mGeometry;
    };

    struct LightObject
    {
        vmath::Vector4 mPosition;
        vmath::Vector4 mColor;
    };

    std::vector<SceneNode> mSceneNodesVector;
    mutable std::vector<DrawObject> mDrawObjectsVector;
    mutable std::vector<LightObject> mLightObjectsVector;

    inline static void drawDrawObject(const DrawObject &draw_object, const Camera &camera, const Uniforms &uniforms, bool global_wireframe = false);
};

struct SceneObject
{
    SceneObject(const Transform &transform, const Material &material, const Geometry &geometry)
        : mTransform(transform), mMaterial(material), mGeometry(geometry) {}

    void toggleVisible() {if(mMaterial.getVisible()) mMaterial.setVisible(false); else mMaterial.setVisible(true);}

    Transform mTransform;
    Material mMaterial;
    Geometry mGeometry;
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


} // namespace gfx
#endif // OPENGLRENDERER_H
