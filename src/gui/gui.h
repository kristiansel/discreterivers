#ifndef GUI_H
#define GUI_H

#include <stack>

#include "../graphics/guirender/guinode.h"

namespace gui {

class GUI;

void createGUI(GUI &gui_root);

class GUI
{
public:
    GUI(int w, int h, float scale_factor) : mWidth(w), mHeight(h), mScaleFactor(scale_factor),
        mGUIRoot(gfx::gui::GUITransform({0.50f, 0.50f}, {1.0f, 1.0f})),
        mHoveredNode(&mGUIRoot),
        mHeadingFont("res/fonts/EBGaramond-Regular.ttf", (32.0f), scale_factor),
        mLabelFont("res/fonts/Overlock-Regular.ttf",     (22.0f), scale_factor),
        mBodyFont("res/fonts/Overlock-Regular.ttf",      (16.0f), scale_factor),
        mMouseCapturedNode(nullptr), mActiveNode(nullptr)
    {
        mGUIRoot.clickPassThru();
    }

    gfx::gui::GUINode &getGUIRoot() { return mGUIRoot; }

    bool handleMouseButtonDown(int32_t x, int32_t y, gfx::gui::MouseButton button);
    void handleMouseButtonUp(int32_t x, int32_t y, gfx::gui::MouseButton button);
    void handleMouseMoved(int32_t x, int32_t y, int32_t x_mov, int32_t y_mov);
    void handleMouseWheelScroll(int32_t y);
    void handleKeyPressEvent(SDL_Keycode k);

    void resize(int w, int h);

    //inline const gfx::gui::GUIFont &getFontRenderer() const { return mLabelFont; }
    inline const gfx::gui::GUIFont &getDefaultFont() const { return mLabelFont; }
    inline const gfx::gui::GUIFont &getHeadingFont() const { return mHeadingFont; }
    inline const gfx::gui::GUIFont &getBodyFont() const { return mBodyFont; }

    inline float getAbsFromPix(float pix) { return pix/mScaleFactor; }
    inline float getWindowAbsWidth() { return getAbsFromPix((float)(mWidth)); }
    inline float getWindowAbsHeight() { return getAbsFromPix((float)(mHeight)); }

    void updateUIScaleFactor(float scale_factor);

private:
    GUI();

    void processHoverStacks();

    int mWidth;
    int mHeight;
    float mScaleFactor;

    gfx::gui::GUIFont mHeadingFont;
    gfx::gui::GUIFont mLabelFont;
    gfx::gui::GUIFont mBodyFont;


    gfx::gui::GUINode    mGUIRoot;
    gfx::gui::GUINodePtr mHoveredNode; // previous deepest hovered
    gfx::gui::GUINodePtr mActiveNode; // active
    gfx::gui::GUINodePtr mMouseCapturedNode;   //

    std::vector<gfx::gui::GUINodePtr> mPrevHoverStack;
    std::vector<gfx::gui::GUINodePtr> mNextHoverStack;


};

}

#endif // GUI_H
