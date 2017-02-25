#include "gui.h"
#include "../events/queuedevents.h"
#include "../events/immediateevents.h"

#include "submenus.h"

namespace gui {

void createGUI(GUI &gui)
{
    gfx::gui::GUINode &gui_root_node = gui.getGUIRoot();

    // main menu
    createMainMenu(gui, gui_root_node);

    // main menu submenus
    createNewGameMenu(gui, gui_root_node);
    createSaveGameMenu(gui, gui_root_node);
    createLoadGameMenu(gui, gui_root_node);
    createOptionsMenu(gui, gui_root_node);


    // deeper menus
    createChooseOriginMenu(gui, gui_root_node);

    // extra (goes in the bottom to be on top in the gui)
    createProfilingPane(gui, gui_root_node);
}

bool GUI::handleMouseButtonDown(int32_t x, int32_t y, gfx::gui::MouseButton button)
{
    float abs_width = getWindowAbsWidth();
    float abs_height = getWindowAbsHeight();

    float x_rel = (float)(x)/(float)(mWidth);
    float y_rel = (float)(y)/(float)(mHeight);
    //std::cout << "clicked " << x_rel << ", " << y_rel << std::endl;
    gfx::gui::GUINodePtr mouse_down_node = mGUIRoot.getDeepestClicked(x_rel, y_rel, abs_width, abs_height);

    if (mouse_down_node) mouse_down_node->handleEvent(gfx::gui::MouseButtonDownEvent{button, x, y});
    if (mActiveNode) mActiveNode->handleEvent(gfx::gui::FocusLostEvent());


    mMouseCapturedNode = mouse_down_node;
    mActiveNode        = mouse_down_node;

    if (mActiveNode) mActiveNode->handleEvent(gfx::gui::FocusGainedEvent());

    return (mouse_down_node != nullptr);
}

void GUI::handleMouseButtonUp(int32_t x, int32_t y, gfx::gui::MouseButton button)
{
    if (mMouseCapturedNode) mMouseCapturedNode->handleEvent(gfx::gui::MouseButtonUpEvent{button, x, y});

    // reset the captured node
    mMouseCapturedNode = nullptr;
}

void GUI::handleMouseMoved(int32_t x, int32_t y, int32_t x_mov, int32_t y_mov)
{
    float abs_width = getWindowAbsWidth();
    float abs_height = getWindowAbsHeight();

    float x_rel = (float)(x)/(float)(mWidth);
    float y_rel = (float)(y)/(float)(mHeight);

    // test with hovered stack

    // hover stack represents the previous getDeepestHovered search


    // end test with hovered stack
    //mNextHoverStack.clear();
    gfx::gui::GUINodePtr current_hovered = mGUIRoot.getDeepestHovered(x_rel, y_rel, abs_width, abs_height, mNextHoverStack);

    processHoverStacks();

    if (mMouseCapturedNode)
    {
        //std::cout << "x_rel " << x_rel << ", y_rel " << y_rel << std::endl;
        mMouseCapturedNode->handleEvent(gfx::gui::MouseDragEvent{x_mov*mScaleFactor, y_mov*mScaleFactor});
    }
}

void GUI::processHoverStacks()
{
    bool discrepancy = false;
    for (int i = 0; i<std::max(mNextHoverStack.size(), mPrevHoverStack.size()); i++)
    {
        gfx::gui::GUINodePtr next = i<mNextHoverStack.size() ? mNextHoverStack[i] : nullptr;
        gfx::gui::GUINodePtr prev = i<mPrevHoverStack.size() ? mPrevHoverStack[i] : nullptr;
        discrepancy = next != prev;
        if (discrepancy)
        {
            if (next) next->handleEvent(gfx::gui::MouseEnterEvent());
            if (prev) prev->handleEvent(gfx::gui::MouseLeaveEvent());
        }
    }
    mPrevHoverStack.clear();
    std::swap(mNextHoverStack, mPrevHoverStack);
}

void GUI::handleMouseWheelScroll(int32_t y)
{
    // send event to deepest hovered.
    gfx::gui::GUINodePtr &deepest_hovered = mPrevHoverStack.back();
    if (deepest_hovered) deepest_hovered->handleEvent(gfx::gui::MouseWheelScrollEvent{y});
}

void GUI::handleKeyPressEvent(SDL_Keycode k)
{
    // send key press event to active node
    if (mActiveNode) mActiveNode->handleEvent(gfx::gui::KeyPressEvent{k});
}

void GUI::resize(int w, int h)
{
    mWidth = w;
    mHeight = h;
    mGUIRoot.resize(getWindowAbsWidth(), getWindowAbsHeight());
}

void GUI::updateUIScaleFactor(float scale_factor)
{
    mScaleFactor = scale_factor;

    // update font textures... wow, really should make the change to a functional style,
    // adding more fonts would mean a lot of manual book keeping...
    mHeadingFont.updateUIScaleFactor(scale_factor);
    mLabelFont.updateUIScaleFactor(scale_factor);
    mBodyFont.updateUIScaleFactor(scale_factor);

    mGUIRoot.resize(getWindowAbsWidth(), getWindowAbsHeight());
}

}
