//
// Created by bcub3d-desktop on 12/1/20.
//

#ifndef OPENGLMAP_RENDERENGINE_H
#define OPENGLMAP_RENDERENGINE_H

// GLEW (OpenGL Extension Wrangler Library)
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

#include "../settings.h"
#include "../loadingScreen.h"
#include "../../openGLPlotLive/src/lineColours.h"

/* Classes */
class RenderEngine {
public:
    /* Data */
    GLFWwindow* window;
    Settings* settings;
    LoadingScreen* loadingScreen;

    /* Shaders */
    Shader* lightingShader;
    Shader* tileShader;
    Shader* skyboxShader;
    Shader* simpleShader;
    Shader* volumeShader;
    Shader* lineShader;
    Shader* textShader;

    /* Fonts */
    GLFont* fpsFontPt;
    GLFont* telemFont;
    GLFont* helpFont;

    /* Colours */
    std::vector<glm::vec3> colorVec = {LC_BLUE, LC_RED, LC_GREEN, LC_YELLOW, LC_CYAN, LC_MAGENTA, LC_SILVER, LC_GRAY, LC_MAROON, LC_OLIVE, LC_DARKGREEN, LC_PURPLE, LC_TEAL, LC_NAVY};


    /* Constructor */
    RenderEngine(Settings *settings);

    /* Functions */
    void setupWindow();
    void createLoadingScreen();
    void setupShaders();
    void loadFontShaders();

    void setWireFrameBool(bool wireFrameOn);
    void setFpsOn(bool fpsOn);

};

#endif //OPENGLMAP_RENDERENGINE_H
