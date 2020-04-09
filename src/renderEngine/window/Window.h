//
// Created by bcub3d-desktop on 8/4/20.
//

#ifndef OPENGLMAP_WINDOW_H
#define OPENGLMAP_WINDOW_H

// openGLPlotLive Includes
#include "../../../openGLPlotLive/src/window/IWindow.h"

// Project Includes
#include <utilities/settings.h>


class Window : public GLPL::IWindow {
public:
    // Constructor
    Window(Settings* settings);
    // Functions
    int getWidth();
    int getHeight();
    GLFWwindow* getWindow();

private:
    // Functions
    void initGLFW();
    void initGLAD();

    // Data
    Settings* settings;
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;

    // Keys
    bool keys[1024];
    bool toggleKeys[1024];
};


#endif //OPENGLMAP_WINDOW_H
