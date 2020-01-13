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

    // Initialise GLEW - engine OpenGL pointers
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

void RenderEngine::registerSixDofModel(SixDofModel* sixDofModel) {
    int newId = this->lastId + 1;
    this->sixDofModels.insert({newId, sixDofModel});
    sixDofModel->setId(newId);
    this->lastId = newId;
}

void RenderEngine::removeSixDofModel(SixDofModel* sixDofModel) {
    this->sixDofModels.erase(sixDofModel->getId());
}

void RenderEngine::DrawFrame(Camera camera) {
    /* Draw the frame for the current world. */
    // Clear the colour buffer
    glClearColor(0.64f, 0.64f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set correct shader
    this->lightingShader->Use();

    // Update View Position Uniform
    GLint viewPosLoc = glGetUniformLocation(this->lightingShader->Program, "viewPos");
    glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

    // Transformation Matrices
    int screenWidth = settings->xRes;
    int screenHeight = settings->yRes;
    glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight,0.1f,10000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(this->lightingShader->Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(this->lightingShader->Program,"view"),1,GL_FALSE,glm::value_ptr(view));

    /* Six Dof Models */
   for (auto i : this->sixDofModels) {
        i.second->Draw(*(this->lightingShader));
    }


}