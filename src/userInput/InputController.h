//
// Created by bcub3d-desktop on 24/1/20.
//

#ifndef OPENGLMAP_INPUTCONTROLLER_H
#define OPENGLMAP_INPUTCONTROLLER_H

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// Project Includes
#include <renderEngine/RenderEngine.h>



class InputController {
public:
    /* Constructor */
    InputController(std::shared_ptr<RenderEngine> renderEngine);

    /* Functions */
    void setupInputCallbacks();

    std::shared_ptr<Camera> getCamera();
    bool getToggleKeyStateByIndex(int index);
    bool getFirstMouseState();
    GLfloat getLastX();
    GLfloat getLastY();
    GLfloat getLastFrameTime();
    GLfloat getDeltaTime();
    void setKeysByIndex(int index, bool boolean);
    void setToggleKeysByIndex(int index, bool boolean);
    void setLastXY(GLfloat lastX, GLfloat lastY);
    void setFirstMouseState(bool firstMouseState);
    void setDeltaTime(GLfloat newDeltaTime);
    void setLastFrameTime(GLfloat newLastFrameTime);

    void do_movement();

private:
    std::shared_ptr<RenderEngine> renderEngine;

    bool keys[1024] = {false};
    bool toggleKeys[1024] = {false};
    GLfloat lastX = 400, lastY = 300;
    bool firstMouse = true;

    GLfloat xval = 0.0f;

    // Timing
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

};



#endif //OPENGLMAP_INPUTCONTROLLER_H
