//
// Created by bcub3d-desktop on 25/1/20.
//

#ifndef OPENGLMAP_INPUTCALLBACKS_H
#define OPENGLMAP_INPUTCALLBACKS_H



// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// Project Includes
#include "InputController.h"


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Retrieve userInput
    InputController* controllerPt = (InputController*)glfwGetWindowUserPointer(window);
    // Get Camera
    std::shared_ptr<Camera> camera = controllerPt->getCamera();
    // ESC to close window
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window,GL_TRUE);
    }
    // Set key states
    if(action == GLFW_PRESS) {
        controllerPt->setKeysByIndex(key, true);
        ////keys[key] = true;
        int	camNum = 3;
        // Set toggle states
        controllerPt->setToggleKeysByIndex(key, !controllerPt->getToggleKeyStateByIndex(key));
        ////toggleKeys[key] = !toggleKeys[key];
        // Change View Forward
        if(key==GLFW_KEY_V) {
            camera->view += 1;
            if(camera->view > camNum) {
                camera->view = 0;
            }
        }
        // Change View Backward
        if(key==GLFW_KEY_B) {
            camera->view -=1;
            if(camera->view < 0) {
                camera->view = camNum;
            }
        }
        // Change Other Tracking Aircraft
        /*if(key==GLFW_KEY_N) {
            camera->otherAircraftID += 1;
            if(camera->otherAircraftID < 0) {
                camera->otherAircraftID = (int)camera->mavAircraftListPt->size() - 1;
            } else if (camera->otherAircraftID > (int)camera->mavAircraftListPt->size() - 1) {
                camera->otherAircraftID = 0;
            }
        }
        // Change Aircraft Forward
        if(key==GLFW_KEY_Z) {
            camera->aircraftID += 1;
            if (camera->aircraftID > (int)camera->mavAircraftListPt->size() - 1) {
                camera->aircraftID = 0;
            }
        }
        // Change Aircraft Backward
        if(key==GLFW_KEY_X) {
            camera->aircraftID -= 1;
            if (camera->aircraftID < 0) {
                camera->aircraftID = (int)camera->mavAircraftListPt->size() - 1 ;
            }
        }*/
    } else if (action == GLFW_RELEASE) {
        controllerPt->setKeysByIndex(key, false);
        //keys[key] = false;
    }

}

// Callback to position the camera
static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // Retrieve userInput
    InputController* controllerPt = (InputController*)glfwGetWindowUserPointer(window);
    // Get Camera
    std::shared_ptr<Camera> camera = controllerPt->getCamera();
    // Reset mouse to center of window
    if(controllerPt->getFirstMouseState()) {
        controllerPt->setLastXY(xpos, ypos);
        controllerPt->setFirstMouseState(false);
        ////lastX = xpos;
        ////lastY = ypos;
        ////firstMouse = false;
    }

    // Offset for smoother movement
    GLfloat xoffset = xpos - controllerPt->getLastX();
    GLfloat yoffset = controllerPt->getLastY() - ypos;

    controllerPt->setLastXY(xpos, ypos);
    ////lastX = xpos;
    ////lastY = ypos;

    // Don't move if camera movement paused
    ////if (!toggleKeys[80]) { // p key to toggle
    if (!controllerPt->getToggleKeyStateByIndex(80)) { // p key to toggle
        camera->ProcessMouseMovement(xoffset,yoffset);
    }
}

// Scrolling zoom callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Retrieve userInput
    InputController* controllerPt = (InputController*)glfwGetWindowUserPointer(window);
    // Get Camera
    std::shared_ptr<Camera> camera = controllerPt->getCamera();
    camera->ProcessMouseScroll(yoffset);
}

#endif //OPENGLMAP_INPUTCALLBACKS_H
