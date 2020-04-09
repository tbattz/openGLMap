//
// Created by bcub3d-desktop on 12/1/20.
//

#include "utilities/settings.h"
#include "RenderEngine.h"

#include <memory>


/* Constructor */
RenderEngine::RenderEngine(Settings* settings) : windowObj(settings) {
    this->settings = settings;

    /* Create Window */
    window = windowObj.getWindow();

    /* Create Loading Screen */
    createLoadingScreen();

    /* Setup Shaders */
    setupShaders();
    loadFontShaders();

    /* Create Camera */
    camera = std::make_shared<Camera>(glm::vec3(0.0f, 3.0f, 3.0f));

}


void RenderEngine::createLoadingScreen() {
    loadingScreen = new LoadingScreen(window, settings);
}

void RenderEngine::setupShaders() {
    // TODO - Remove hard-coding of shader files
    // Setup and compile shaders
    loadingScreen->appendLoadingMessage("Loading lightingShader.");
    lightingShader = new Shader("../../Shaders/multiple_lighting.vs","../../Shaders/multiple_lighting.frag");
    loadingScreen->appendLoadingMessage("Loading tileShader.");
    tileShader = new Shader("../../Shaders/tileImage.vs","../../Shaders/tileImage.frag");
    loadingScreen->appendLoadingMessage("Loading skyboxShader.");
    skyboxShader = new Shader("../../Shaders/skybox.vs","../../Shaders/skybox.frag");
    loadingScreen->appendLoadingMessage("Loading simpleShader.");
    simpleShader = new Shader("../../Shaders/telemOverlay.vs","../../Shaders/telemOverlay.frag");
    loadingScreen->appendLoadingMessage("Loading volumeShader.");
    volumeShader = new Shader("../../Shaders/volume.vs","../../Shaders/volume.frag");
    loadingScreen->appendLoadingMessage("Loading lineShader.");
    lineShader = new Shader("../../Shaders/line.vs","../../Shaders/line.frag");
    loadingScreen->appendLoadingMessage("Loading axesShader.");
    axesShader = new Shader("../../Shaders/axes.vs", "../../Shaders/axes.frag");
}

void RenderEngine::loadFontShaders() {
    // Load Font Shader
    loadingScreen->appendLoadingMessage("Loading textShader.");
    // TODO - Remove hard-coding of shader files
    textShader = setupFontShader("../../Shaders/font.vs", "../../Shaders/font.frag", settings);

    // Load Telemetry Font
    telemFont = new GLFont(FONTPATH);

    // Help Font
    loadingScreen->appendLoadingMessage("Loading fonts.");
    helpFont = new GLFont(FONTPATH);
}

std::shared_ptr<Camera> RenderEngine::getCamera() {
    return this->camera;
}

void RenderEngine::registerWorldObjectController(std::shared_ptr<IWorldObjectController> worldObjectController) {
    this->worldObjectControllerList2.push_back(worldObjectController);
}

void RenderEngine::registerTileController(std::shared_ptr<SatTileGroupController> satTileGroupController) {
    this->satTileGroupController = satTileGroupController;
}

void RenderEngine::preRender() {
    // Clear the colour buffer
    glClearColor(0.64f, 0.64f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update View
    camera->setupView();

    // Update View Position Uniforms
    lightingShader->Use();
    GLint viewPosLoc = glGetUniformLocation(lightingShader->Program, "viewPos");
    glUniform3f(viewPosLoc, camera->Position.x, camera->Position.y, camera->Position.z);

    // Transformation Matrices
    int screenWidth = settings->xRes;
    int screenHeight = settings->yRes;
    glm::mat4 projection = glm::perspective(camera->Zoom, (float)screenWidth/(float)screenHeight,0.1f,10000.0f);
    glm::mat4 view = camera->GetViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader->Program,"view"),1,GL_FALSE,glm::value_ptr(view));

    tileShader->Use();
    glUniformMatrix4fv(glGetUniformLocation(tileShader->Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(tileShader->Program,"view"),1,GL_FALSE,glm::value_ptr(view));

    if(axesOn) {
        axesShader->Use();
        glUniformMatrix4fv(glGetUniformLocation(axesShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(axesShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    }

}

void RenderEngine::renderFrame() {
    /* Setup Rendering */
    preRender();

    /* World Object Models */
    this->lightingShader->Use();
    for(unsigned int i=0; i < worldObjectControllerList2.size(); i++) {
        worldObjectControllerList2[i]->draw(*lightingShader);
    }

    /* World Object Models */
    this->lightingShader->Use();
    for(unsigned int i=0; i < worldObjectControllerList.size(); i++) {
        worldObjectControllerList[i]->draw(*lightingShader);
    }
    /* World Geo Object Models */
    this->lightingShader->Use();
    for(unsigned int i=0; i < worldGeoObjectControllerList.size(); i++) {
        worldGeoObjectControllerList[i]->draw(*lightingShader);
    }
    /* Simple Object Models */
    this->lightingShader->Use();
    for(unsigned int i=0; i < simpleObjectControllerList.size(); i++) {
        simpleObjectControllerList[i]->draw(*lightingShader);
    }
    /* Mavlink Geo Object Models */
    this->lightingShader->Use();
    for(unsigned int i=0; i < mavlinkGeoObjectControllerList.size(); i++) {
        mavlinkGeoObjectControllerList[i]->draw(*lightingShader);
    }
    /* Satellite Tiles */
    if (satTileGroupController != nullptr) {
        this->tileShader->Use();
        satTileGroupController->draw(*tileShader);
    }

    /* Draw Axes */
    if(axesOn) {
        this->axesShader->Use();
        axesView->Draw(*axesShader);
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

void RenderEngine::registerAxesView(std::shared_ptr<AxesView> axesView) {
    this->axesView = axesView;
}

void RenderEngine::toggleAxes(bool axesOnBool) {
    this->axesOn = axesOnBool;
}




