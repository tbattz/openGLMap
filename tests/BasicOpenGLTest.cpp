// Simulation Engine
#include "renderEngine/RenderEngine.h"
#include "renderEngine/light.h"
// Skybox
#include "renderEngine/skybox/skybox.h"
// Input Controller
#include "userInput/InputController.h"
#include "objects/worldObject/SimpleObjectController.h"


#include <utilities/settings.h>
#include "gtest/gtest.h"



TEST(BasicTextures, loadSquareJPEGTexture) {
    // TODO - Fix relative paths
    Settings settings("../../Configs/currentConfig.txt");

    // Create Render Engine
    RenderEngine renderEngine(&settings);

    // Create Input Controller
    std::shared_ptr<RenderEngine> renderEnginePt = std::make_shared<RenderEngine>(renderEngine);
    InputController inputController = InputController(renderEnginePt);

    // Load lights
    renderEngine.loadingScreen->appendLoadingMessage("Loading Lights.");
    // Load Lights
    DirectionalLight myDirLight({-0.2,-1.0,-0.3}, {0.5,0.5,0.5}, {0.4,0.4,0.4}, {0.5,0.5,0.5}, renderEngine.lightingShader,0);

    // Set number of lights
    glUniform1i(glGetUniformLocation(renderEngine.lightingShader->Program,"numLights.nDirLight"),1);
    glUniform1i(glGetUniformLocation(renderEngine.lightingShader->Program,"numLights.nPointLight"),0);
    glUniform1i(glGetUniformLocation(renderEngine.lightingShader->Program,"numLights.nSpotLight"),0);

    // Create SimpleObject
    // Tile 1
    std::shared_ptr<SimpleObjectController> simpleObjectController;
    simpleObjectController = std::shared_ptr<SimpleObjectController>(new SimpleObjectController(glm::dvec3(0.0f, 0.0f, 0.0f), "../../SatTiles/18-236834-160981.jpeg"));
    renderEngine.registerSimpleObjController(simpleObjectController);

    // World Axes
    std::shared_ptr<AxesView> axesView = std::shared_ptr<AxesView>(new AxesView());
    renderEngine.registerAxesView(axesView);
    renderEngine.toggleAxes(true);

    // Create Skybox
    renderEngine.loadingScreen->appendLoadingMessage("Loading skybox.");
    vector<const GLchar*> faces;
    // TODO - Fix relative paths
    faces.push_back("../../Models/skybox/right.png");
    faces.push_back("../../Models/skybox/left.png");
    faces.push_back("../../Models/skybox/top.png");
    faces.push_back("../../Models/skybox/bottom.png");
    faces.push_back("../../Models/skybox/back.png");
    faces.push_back("../../Models/skybox/front.png");
    Skybox skybox(faces);


    // Game Loop
    while(!glfwWindowShouldClose(renderEngine.window)) {
        // Set Frame Time
        GLfloat currentFrame = glfwGetTime();
        inputController.setDeltaTime(currentFrame - inputController.getLastFrameTime());
        inputController.setLastFrameTime(currentFrame);

        // Check Events
        glfwPollEvents();

        // Do keyboard movement
        inputController.do_movement();

        // Update View
        std::shared_ptr<Camera> camera = inputController.getCamera();
        camera->setupView();

        // Update View Position Uniform
        GLint viewPosLoc = glGetUniformLocation(renderEngine.lightingShader->Program, "viewPos");
        glUniform3f(viewPosLoc, camera->Position.x, camera->Position.y, camera->Position.z);

        // Transformation Matrices
        int screenWidth = settings.xRes;
        int screenHeight = settings.yRes;
        glm::mat4 projection = glm::perspective(camera->Zoom, (float)screenWidth/(float)screenHeight,0.1f,10000.0f);
        glm::mat4 view = camera->GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(renderEngine.lightingShader->Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(renderEngine.lightingShader->Program,"view"),1,GL_FALSE,glm::value_ptr(view));

        renderEngine.tileShader->Use();
        glUniformMatrix4fv(glGetUniformLocation(renderEngine.tileShader->Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(renderEngine.tileShader->Program,"view"),1,GL_FALSE,glm::value_ptr(view));

        // Draw Models
        renderEngine.renderFrame();

        // Draw Skybox last
        renderEngine.skyboxShader->Use();
        view = glm::mat4(glm::mat3(camera->GetViewMatrix()));	// Remove any translation component of the view matrix
        glUniformMatrix4fv(glGetUniformLocation(renderEngine.skyboxShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(renderEngine.skyboxShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1i(glGetUniformLocation(renderEngine.skyboxShader->Program, "skybox"), 0);
        skybox.Draw(*(renderEngine.skyboxShader));

        // Swap buffers
        glfwSwapBuffers(renderEngine.window);
    }

    glfwTerminate();
}