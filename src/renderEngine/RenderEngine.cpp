//
// Created by bcub3d-desktop on 12/1/20.
//

#include "../settings.h"
#include "RenderEngine.h"
#include "../window.h"

/* Constructor */
RenderEngine::RenderEngine(Settings* settings) {
    this->settings = settings;


    /* Setup Window */
    setupWindow();

    /* Create Loading Screen */
    createLoadingScreen();

    /* Setup Shaders */
    setupShaders();
    loadFontShaders();

}

void RenderEngine::setupWindow() {
    // Init GLFW
    window = initGLFW(settings);

    // Initialise GLEW - renderEngine OpenGL pointers
    initGLEW();
}

void RenderEngine::createLoadingScreen() {
    loadingScreen = new LoadingScreen(window, settings);
}

void RenderEngine::setupShaders() {
    // TODO - Remove hard-coding of shader files
    // Setup and compile shaders
    loadingScreen->appendLoadingMessage("Loading lightingShader.");
    lightingShader = new Shader("../Shaders/multiple_lighting.vs","../Shaders/multiple_lighting.frag");
    loadingScreen->appendLoadingMessage("Loading tileShader.");
    tileShader = new Shader("../Shaders/tileImage.vs","../Shaders/tileImage.frag");
    loadingScreen->appendLoadingMessage("Loading skyboxShader.");
    skyboxShader = new Shader("../Shaders/skybox.vs","../Shaders/skybox.frag");
    loadingScreen->appendLoadingMessage("Loading simpleShader.");
    simpleShader = new Shader("../Shaders/telemOverlay.vs","../Shaders/telemOverlay.frag");
    loadingScreen->appendLoadingMessage("Loading volumeShader.");
    volumeShader = new Shader("../Shaders/volume.vs","../Shaders/volume.frag");
    loadingScreen->appendLoadingMessage("Loading lineShader.");
    lineShader = new Shader("../Shaders/line.vs","../Shaders/line.frag");
}

void RenderEngine::loadFontShaders() {
    // Load Font Shader
    loadingScreen->appendLoadingMessage("Loading textShader.");
    // TODO - Remove hard-coding of shader files
    textShader = setupFontShader("../Shaders/font.vs", "../Shaders/font.frag", settings);

    // Load Telemetry Font
    telemFont = new GLFont(FONTPATH);

    // Help Font
    loadingScreen->appendLoadingMessage("Loading fonts.");
    helpFont = new GLFont(FONTPATH);
}

void RenderEngine::registerController(std::shared_ptr<WorldObjectController> worldObjController) {
    this->controllerList.push_back(worldObjController);
}

void RenderEngine::renderFrame() {
    for(unsigned int i =0; i < controllerList.size(); i++) {
        controllerList[i]->draw(*lightingShader);
    }

}

void RenderEngine::setWireFrameBool(bool wireFrameOn) {
    if(wireFrameOn) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
}

void RenderEngine::setFpsOn(bool fpsOn) {
    if(fpsOn) {
        fpsFontPt = new GLFont(FONTPATH);
    }
}
