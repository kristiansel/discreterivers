#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <iostream>

#include "../common/gfx_primitives.h"
#include "../common/typetag.h"
#include "../common/threads/threadqueue.h"
#include "../common/macro/macrodebugassert.h"
#include "transform.h"
#include "shader.h"
#include "renderflags.h"
#include "scenenode.h"
#include "guirender/guinode.h"
#include "guirender/bgelement/guishader.h"
#include "guirender/textelement/guitextshader.h"
#include "guirender/imageelement/guiimageshader.h"
#include "guirender/guitextvertices.h"
#include "guirender/guifont.h"

namespace vmath = Vectormath::Aos;

namespace gfx
{

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
    using GFXTickJob = std::function<void(void)>;
public:
    OpenGLRenderer(int w, int h);

    // methods
    void draw(const Camera &camera, const gui::GUINode &gui_root) const;
    void preDrawUpdate();

    // mutators
    inline void toggleWireframe() { mRenderFlags.toggleFlag(RenderFlags::Wireframe); }
    void resize(int w, int h);

    // getters
    inline SceneNode &getSceneRoot() { return mSceneRoot; }
    inline int getWidth() const { return mWidth; }
    inline int getHeight() const { return mHeight; }

    inline void addGFXTickJob(const GFXTickJob &job) { mGFXTickJobQueue.push(job); } // could move here...

private:
    int mWidth;
    int mHeight;

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

    SceneNode mSceneRoot;

    mutable std::vector<LightObject> mLightObjectsVector;

    // GUI shader stuff
    gui::GUIShader mGUIShader;
    gui::GUITextShader mGUITextShader;
    gui::GUIImageShader mGUIImageShader;

    // For loading async
    Threads::ThreadQueue<GFXTickJob> mGFXTickJobQueue;

    // private methods
    inline void drawGUI(const gui::GUINode &gui_root) const;
    inline void drawGUIRecursive(const gui::GUINode &gui_node, vmath::Matrix4 parent_transform, float w_abs, float h_abs) const;

    inline void drawScene(const Camera &camera, const SceneNode &scene_root) const;
    inline void drawSceneRecursive(const SceneNode &scene_root, const vmath::Matrix4 parent_transform) const;
};

} // namespace gfx
#endif // OPENGLRENDERER_H
