#ifndef GUI_H
#define GUI_H

#include "../graphics/guirender/guinode.h"
#include "../common/initptr.h"

namespace gui {

class GUI;

void createGUI(GUI &gui_root);

class GUI
{
public:
    GUI(int w, int h, int dpi, float scale_factor) : mWidth(w), mHeight(h), mScaleFactor(scale_factor),
        mGUIRoot(gfx::gui::GUITransform({0.50f, 0.50f}, {1.0f, 1.0f})),
        mHoveredNode(&mGUIRoot), mFontRenderer("res/fonts/IMFePIrm28P.ttf", (unsigned int)(24.0f * scale_factor)),
        mMouseCapturedNode(nullptr), mActiveNode(nullptr)
    {
        mGUIRoot.clickPassThru();
    }

    gfx::gui::GUINode &getGUIRoot()
    {
        //std::cout << " root node ptr: " << (&mGUIRoot) << std::endl;
        return mGUIRoot;
    }

    bool handleMouseButtonDown(int32_t x, int32_t y, gfx::gui::MouseButton button)
    {
        float abs_width = getWindowAbsWidth();
        float abs_height = getWindowAbsHeight();

        float gui_x = (float)(x)/(float)(mWidth);
        float gui_y = (float)(y)/(float)(mHeight);
        //std::cout << "clicked " << gui_x << ", " << gui_y << std::endl;
        gfx::gui::GUINodePtr mouse_down_node = mGUIRoot.getDeepestClicked(gui_x, gui_y, abs_width, abs_height);

        if (mouse_down_node) mouse_down_node->handleEvent(gfx::gui::MouseButtonDownEvent{button, x, y});

        mMouseCapturedNode = mouse_down_node;
        mActiveNode        = mouse_down_node;

        return (mouse_down_node != nullptr);
    }

    void handleMouseButtonUp(int32_t x, int32_t y, gfx::gui::MouseButton button)
    {
        if (mMouseCapturedNode) mMouseCapturedNode->handleEvent(gfx::gui::MouseButtonUpEvent{button, x, y});

        // reset the captured node
        mMouseCapturedNode = nullptr;
    }

    void handleMouseMoved(int32_t x, int32_t y, int32_t x_rel, int32_t y_rel)
    {
        float abs_width = getWindowAbsWidth();
        float abs_height = getWindowAbsHeight();

        float gui_x = (float)(x)/(float)(mWidth);
        float gui_y = (float)(y)/(float)(mHeight);

        gfx::gui::GUINodePtr current_hovered = mGUIRoot.getDeepestHovered(gui_x, gui_y, abs_width, abs_height);

        if (mHoveredNode != current_hovered)
        {
            //std::cout << "Hovered element changed!" << std::endl;
            //std::cout << "    old: " << mPreviousHovered << ", new: " << current_hovered  << std::endl;

            //auto unused = mGUIRoot.getDeepestHovered(gui_x, gui_y, true);
            if (mHoveredNode)
            {
                //mPreviousHovered->mouseLeave.invokeCallbacks();
                mHoveredNode->handleEvent(gfx::gui::MouseLeaveEvent());
            }
            if (current_hovered)
            {
                //current_hovered->mouseEnter.invokeCallbacks();
                current_hovered->handleEvent(gfx::gui::MouseEnterEvent());
            }
            mHoveredNode = current_hovered;
        }

        if (mMouseCapturedNode)
        {
            //std::cout << "x_rel " << x_rel << ", y_rel " << y_rel << std::endl;
            mMouseCapturedNode->handleEvent(gfx::gui::MouseDragEvent{x_rel, y_rel});
        }
    }

    void resize(int w, int h)
    {
        mWidth = w;
        mHeight = h;
    }

    //inline const gfx::gui::GUIFont &getFontRenderer() const { return mFontRenderer; }
    inline const gfx::gui::GUIFont &getDefaultFont() const { return mFontRenderer; }

    inline float getAbsFromPix(float pix) { return pix/mScaleFactor; }
    inline float getWindowAbsWidth() { return getAbsFromPix((float)(mWidth)); }
    inline float getWindowAbsHeight() { return getAbsFromPix((float)(mHeight)); }

private:
    GUI();

    int mWidth;
    int mHeight;
    float mScaleFactor;

    gfx::gui::GUIFont mFontRenderer;

    gfx::gui::GUINode    mGUIRoot;
    stdext::InitPtr<gfx::gui::GUINode> mHoveredNode; // previous deepest hovered
    stdext::InitPtr<gfx::gui::GUINode> mActiveNode; // active
    stdext::InitPtr<gfx::gui::GUINode> mMouseCapturedNode;   //
    //stdext::InitPtr<gfx::gui::GUINode>

};

}

#endif // GUI_H
