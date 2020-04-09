//
// Created by bcub3d-desktop on 12/1/20.
//

#ifndef OPENGLMAP_RENDERENGINE_H
#define OPENGLMAP_RENDERENGINE_H


// GLAD - Multi Language GL Loader-Generator
#include <glad/glad.h>

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// Project Includes
#include "utilities/settings.h"
#include "renderEngine/loadingScreen/loadingScreen.h"
#include "../../openGLPlotLive/src/lines/lineColours.h"
#include <objects/worldObject/WorldObjectController.h>
#include <objects/worldObject/WorldGeoObjectController.h>
#include <objects/worldObject/SimpleObjectController.h>
#include <renderEngine/camera.h>
#include <objects/satTiles/SatTileGroupController.h>
#include <renderEngine/axes/AxesView.h>
#include <objects/externalInput/MavlinkGeoObjectController.h>
#include <renderEngine/window/Window.h>


/* Classes */
class RenderEngine {
public:
    /* Data */
    Window windowObj;
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
    Shader* axesShader;

    /* Fonts */
    GLFont* fpsFontPt;
    GLFont* telemFont;
    GLFont* helpFont;

    /* Colours */
    std::vector<glm::vec3> colorVec = {LC_BLUE, LC_RED, LC_GREEN, LC_YELLOW, LC_CYAN, LC_MAGENTA, LC_SILVER, LC_GRAY, LC_MAROON, LC_OLIVE, LC_DARKGREEN, LC_PURPLE, LC_TEAL, LC_NAVY};


    /* Constructor */
    RenderEngine(Settings *settings);

    /* Functions */
    void createLoadingScreen();
    void setupShaders();
    void loadFontShaders();

    std::shared_ptr<Camera> getCamera();

    void registerWorldObjectController(std::shared_ptr<IWorldObjectController> worldObjectController);
    void registerTileController(std::shared_ptr<SatTileGroupController> satTileGroupController);

    void preRender();
    void renderFrame();

    void setWireFrameBool(bool wireFrameOn);
    void setFpsOn(bool fpsOn);

    void registerAxesView(std::shared_ptr<AxesView> axesView);
    void toggleAxes(bool axesOnBool);

private:
    std::shared_ptr<Camera> camera;

    std::vector<std::shared_ptr<IWorldObjectController>> worldObjectControllerList2;

    std::vector<std::shared_ptr<WorldObjectController>> worldObjectControllerList;
    std::vector<std::shared_ptr<WorldGeoObjectController>> worldGeoObjectControllerList;
    std::vector<std::shared_ptr<SimpleObjectController>> simpleObjectControllerList;
    std::vector<std::shared_ptr<MavlinkGeoObjectController>> mavlinkGeoObjectControllerList;
    std::shared_ptr<SatTileGroupController> satTileGroupController;

    bool axesOn = false;
    std::shared_ptr<AxesView> axesView;

};

#endif //OPENGLMAP_RENDERENGINE_H
