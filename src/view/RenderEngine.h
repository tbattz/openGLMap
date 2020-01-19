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

// Standard Includes
#include <unordered_map>

#include "../settings.h"
#include "loadingScreen.h"
#include "model/objModel.h"
#include "model/satTiles.h"
#include "camera.h"

/* Classes */
class RenderEngine {
private:
    /* Identifiers */
    int lastId = 0;
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

    /* Models */
    // TODO - Find a better way to store objects and their IDs
    std::unordered_map<int, SixDofModel*> sixDofModels;


    /* Constructor */
    RenderEngine(Settings *settings);

    /* Functions */
    void setupWindow();
    void createLoadingScreen();
    void setupShaders();
    void loadFontShaders();

    void setWireFrameBool(bool wireFrameOn);
    void setFpsOn(bool fpsOn);

    void registerSixDofModel(SixDofModel* sixDofModel);
    void removeSixDofModel(SixDofModel* sixDofModel);

    void DrawFrame(Camera camera);
    //void Draw(ObjModel model);
    //void Draw(SatTile satTile);

};

#endif //OPENGLMAP_RENDERENGINE_H
