#ifndef GUI_H
#define GUI_H

#include "../graphics/guirender/guinode.h"

namespace gui {

void createGUI(gfx::gui::GUINode &gui_root,
               const gfx::gui::GUIFontRenderer &font_renderer,
               int width, int height);

class GUI
{
public:
    GUI(int w, int h, int dpi) : mWidth(w), mHeight(h), mDPI(dpi),
        mGUIRoot(gfx::gui::GUITransform({0.50f, 0.50f}, {1.0f, 1.0f})),
        mPreviousHovered(&mGUIRoot) {}

    gfx::gui::GUINode &getGUIRoot()
    {
        //std::cout << " root node ptr: " << (&mGUIRoot) << std::endl;
        return mGUIRoot;
    }

    void handleMouseClick(uint16_t x, uint16_t y)
    {
        // transform pixels to gui coordinates
        float gui_x = (float)(x)/(float)(mWidth);
        float gui_y = (float)(y)/(float)(mHeight);
        //std::cout << "clicked " << gui_x << ", " << gui_y << std::endl;
        mGUIRoot.handleMouseClick(gui_x, gui_y);
    }

    void handleMouseMoved(uint16_t x, uint16_t y)
    {
        // transform pixels to gui coordinates
        float gui_x = (float)(x)/(float)(mWidth);
        float gui_y = (float)(y)/(float)(mHeight);

        gfx::gui::GUINodePtr current_hovered = mGUIRoot.getDeepestHovered(gui_x, gui_y);
        if (current_hovered != mPreviousHovered)
        {
            //std::cout << "Hovered element changed!" << std::endl;
            //std::cout << "    old: " << mPreviousHovered << ", new: " << current_hovered  << std::endl;

            //auto unused = mGUIRoot.getDeepestHovered(gui_x, gui_y, true);
            if (mPreviousHovered) mPreviousHovered->mouseLeave.invokeCallbacks();
            if (current_hovered) current_hovered->mouseEnter.invokeCallbacks();
            mPreviousHovered = current_hovered;
        }
    }

    void resize(int w, int h)
    {
        mWidth = w;
        mHeight = h;
    }

private:
    GUI();

    int mWidth;
    int mHeight;

    int mDPI;

    // could put DPI info here?
    // yes, should be a good idea...

    gfx::gui::GUINode    mGUIRoot;
    gfx::gui::GUINodePtr mPreviousHovered;

};

}

#endif // GUI_H
