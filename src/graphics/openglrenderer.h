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

namespace vmath = Vectormath::Aos;

namespace gfx
{

class OpenGLRenderer;

struct primitive_type_tag{};
typedef decltype(GL_TRIANGLES) PRIMITIVE_GL_CODE;
typedef ID<primitive_type_tag, PRIMITIVE_GL_CODE, GL_TRIANGLES> gl_primitive_type;

struct Vertices
{
    explicit Vertices(const std::vector<vmath::Vector4> &position_data,
                      const std::vector<vmath::Vector4> &normal_data);

    inline GLuint getVertexArrayObject() const       {return mVertexArrayObject;}
    inline GLuint getPositionArrayBuffer() const     {return mPositionArrayBuffer;}
    inline GLuint getNormalArrayBuffer() const       {return mNormalArrayBuffer;}


private:
    Vertices();

    GLuint mVertexArrayObject;
    GLuint mPositionArrayBuffer;
    GLuint mNormalArrayBuffer;


};

struct Primitives
{
    template<class PrimitiveType>
    explicit Primitives(const std::vector<PrimitiveType> &primitive_data);

    inline GLuint getElementArrayBuffer() const         {return mElementArrayBuffer;}
    inline GLsizeiptr getNumIndices() const             {return mNumIndices;}
    inline gl_primitive_type getPrimitiveType() const   {return mPrimitiveType;}

private:
    Primitives();

    GLuint mElementArrayBuffer; // element array buffer
    GLsizeiptr mNumIndices;
    gl_primitive_type mPrimitiveType;
};

struct Geometry
{
    explicit Geometry(const Vertices &vertices, const Primitives &primitives);

    template<class PrimitiveType>
    explicit Geometry(const std::vector<vmath::Vector4> &vertices,
             const std::vector<vmath::Vector4> &normals,
             const std::vector<PrimitiveType> &primitives);

    // so that they cannot be reassigned after creation
    inline const Vertices& getVertices() const {return mVertices;}
    inline const Primitives& getPrimitives() const {return mPrimitives;}

private:
    Geometry();

    Vertices mVertices;
    Primitives mPrimitives;
};

struct Material
{
    Material() : color(1.0f), wireframe(false), visible(true) {}
    Material(const Material &m) : color(m.color), wireframe(m.wireframe), visible(m.visible){}
    Material(const vmath::Vector4 &color_in) : color(color_in), wireframe(false), visible(true) {}


    // get
    const vmath::Vector4 &getColor() const {return color;}
    const bool &getWireframe() const {return wireframe;}
    const bool &getVisible() const {return visible;}

    // set
    void setColor(const vmath::Vector4 &color_in) {color=color_in;}
    void setWireframe(bool w) {wireframe=w;}
    void setVisible(bool v) {visible=v;}


private:
    vmath::Vector4 color;
    bool wireframe;
    bool visible;
};

struct Transform
{
    Transform() : position(0.0f, 0.0f, 0.0f), rotation(vmath::Quat::identity()), scale(1.0f, 1.0f, 1.0f) {}
    Transform(const Transform &t) : position(t.position), rotation(t.rotation), scale(t.scale) {}
    vmath::Vector3 position;
    vmath::Quat rotation;
    vmath::Vector3 scale;

    vmath::Matrix4 getTransformMatrix() const
    {
        return vmath::Matrix4::translation(position) * vmath::Matrix4::rotation(rotation) * vmath::Matrix4::scale(scale);
    }
};

struct sceneobject_tag{};
typedef ID<sceneobject_tag, int, -1> sceneobject_id;

struct light_tag{};
typedef ID<light_tag, int, -1> light_id;

struct scenenode_tag{};
typedef ID<scenenode_tag, int, -1> scenenode_id;

struct Light
{
    Light(const Transform &transform, const vmath::Vector4 &color)
        : mTransform(transform), mColor(color) {}

    Transform mTransform;
    vmath::Vector4 mColor;
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

struct DrawObject
{
    DrawObject(const vmath::Matrix4 &matrix, const Material &material, const Geometry &geometry)
        : mMatrix(matrix), mMaterial(material), mGeometry(geometry) {}

    vmath::Matrix4 mMatrix;
    Material mMaterial;
    Geometry mGeometry;
};

struct LightObject
{
    LightObject(const vmath::Vector4 &position, const vmath::Vector4 &color)
        : mPosition(position), mColor(color) {}

    vmath::Vector4 mPosition;
    vmath::Vector4 mColor;
};

struct SceneNode;
struct Light;

struct SceneNodeHandle;
struct LightHandle;

void checkOpenGLErrors(const std::string &error_check_label);

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

    std::vector<SceneNode> mSceneNodesVector;
    mutable std::vector<DrawObject> mDrawObjectsVector;

    mutable std::vector<LightObject> mLightObjectsVector;

    inline static void drawDrawObject(const DrawObject &draw_object, const Camera &camera, const Uniforms &uniforms, bool global_wireframe = false);
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

template<class PrimitiveType>
Primitives::Primitives(const std::vector<PrimitiveType> &primitives_data)
{
    const GLuint *indices = (GLuint *)&primitives_data[0];
    mNumIndices = primitives_data.size() * sizeof(PrimitiveType);

    glGenBuffers(1, &mElementArrayBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementArrayBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices, indices, GL_STATIC_DRAW);

    gl_primitive_type gl_primitive =
        std::is_same<PrimitiveType, gfx::Line>::value   ?       // if
            gl_primitive_type(GL_LINES)     :
        std::is_same<PrimitiveType, gfx::Point>::value  ?       // else if
            gl_primitive_type(GL_POINTS)    :
     // std::is_same<PrimitiveType, gfx::Triangle>::value ?     // else
            gl_primitive_type(GL_TRIANGLES);

    mPrimitiveType = gl_primitive;
}

template<class PrimitiveType>
Geometry::Geometry(const std::vector<vmath::Vector4> &position_data,
                   const std::vector<vmath::Vector4> &normal_data,
                   const std::vector<PrimitiveType> &primitive_data) :
    mVertices(Vertices(position_data, normal_data)), mPrimitives(primitive_data) {}

} // namespace gfx
#endif // OPENGLRENDERER_H
