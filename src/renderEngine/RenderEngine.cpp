//
// Created by bcub3d-desktop on 12/1/20.
//

#include "../settings.h"
#include "RenderEngine.h"

#include <memory>


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

    /* Create Camera */
    camera = std::make_shared<Camera>(glm::vec3(0.0f, 3.0f, 3.0f));

}

void RenderEngine::setupWindow() {
    // Init GLFW
    initGLFW();

    // Initialise GLEW - renderEngine OpenGL pointers
    initGLEW();
}

void RenderEngine::initGLFW() {
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE); // Set core profile
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

    // Screen Properties
    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if(settings->fullscreen) {
        settings->xRes = mode->width;
        settings->yRes = mode->height;
    }
    glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
    if(settings->fullscreen) {
        this->window = glfwCreateWindow(settings->xRes,settings->yRes,"openGLMap",monitors[settings->screenID-1],nullptr);
    } else {
        this->window = glfwCreateWindow(settings->xRes,settings->yRes,"openGLMap",nullptr,nullptr);
    }
    glfwMakeContextCurrent(this->window);

    // Set viewport size
    glViewport(0,0,settings->xRes,settings->yRes); // Origin is bottom left

    // Disable Cursor
    //glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

    // Test for objects in front of each other
    glEnable(GL_DEPTH_TEST);

}

void RenderEngine::initGLEW() {
    // Initialise GLEW - setup OpenGL pointers
    glewExperimental = GL_TRUE;
    glewInit();
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

void RenderEngine::registerWorldObjController(std::shared_ptr<WorldObjectController> worldObjController) {
    this->controllerList.push_back(worldObjController);
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

}

void RenderEngine::renderFrame() {
    /* Setup Rendering */
    preRender();

    /* World Object Models */
    this->lightingShader->Use();
    for(unsigned int i =0; i < controllerList.size(); i++) {
        controllerList[i]->draw(*lightingShader);
    }
    /* Satellite Tiles */
    this->tileShader->Use();
    satTileGroupController->draw(*tileShader);



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


