//
// Created by bcub3d-desktop on 24/1/20.
//


#include <utility>

// Project Includes
#include <camera.h>
#include "InputController.h"
#include "inputCallbacks.h"


InputController::InputController(std::shared_ptr<RenderEngine> renderEngine) {   
    this->renderEngine = std::move(renderEngine);

    // Set pointer for callback functions to access this class
    glfwSetWindowUserPointer(this->renderEngine->window, reinterpret_cast<void*>(this));

    // Setup callbacks
    setupInputCallbacks();
}

std::shared_ptr<Camera> InputController::getCamera() {
    return this->renderEngine->getCamera();
}

bool InputController::getToggleKeyStateByIndex(int index) {
    return toggleKeys[index];
}

bool InputController::getFirstMouseState() {
    return firstMouse;
}

GLfloat InputController::getLastX() {
    return lastX;
};

GLfloat InputController::getLastY() {
    return lastY;
}

GLfloat InputController::getLastFrameTime() {
    return lastFrame;
};

GLfloat InputController::getDeltaTime() {
    return deltaTime;
};

void InputController::setKeysByIndex(int index, bool boolean) {
    keys[index] = boolean;
}

void InputController::setToggleKeysByIndex(int index, bool boolean) {
    toggleKeys[index] = boolean;
}

void InputController::setLastXY(GLfloat lastX, GLfloat lastY) {
    this->lastX = lastX;
    this->lastY = lastY;
};

void InputController::setFirstMouseState(bool firstMouseState) {
    firstMouse = firstMouseState;
};

void InputController::setDeltaTime(GLfloat newDeltaTime) {
    this->deltaTime = newDeltaTime;
};

void InputController::setLastFrameTime(GLfloat newLastFrameTime) {
    this->lastFrame = newLastFrameTime;
};


void InputController::setupInputCallbacks() {
    // Setup Callbacks for user input
    glfwSetKeyCallback(this->renderEngine->window, key_callback);
    glfwSetCursorPosCallback(this->renderEngine->window, mouse_callback);
    glfwSetScrollCallback(this->renderEngine->window, scroll_callback);
}



// Move camera using wasd keys
void InputController::do_movement() {
    // Get Camera
    std::shared_ptr<Camera> camera = renderEngine->getCamera();
    // Camera Controls
    if(keys[GLFW_KEY_W]) {
        camera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if(keys[GLFW_KEY_S]) {
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    }
    if(keys[GLFW_KEY_A]) {
        camera->ProcessKeyboard(LEFT, deltaTime);
    }
    if(keys[GLFW_KEY_D]) {
        camera->ProcessKeyboard(RIGHT, deltaTime);
    }
}


