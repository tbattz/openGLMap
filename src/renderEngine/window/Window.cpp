//
// Created by bcub3d-desktop on 8/4/20.
//


// GLAD - Multi Language GL Loader-Generator
#include <glad/glad.h>

// Standard Includes
#include <iostream>

// Project Includes
#include "Window.h"


Window::Window(Settings* settings) {
    this->settings = settings;

    // Setup GLFW
    Window::initGLFW();

}

int Window::getWidth() {
    return settings->xRes;
}

int Window::getHeight() {
    return settings->yRes;
}

GLFWwindow *Window::getWindow() {
    return this->window;
}

void Window::initGLFW() {
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

    // Initialise GLAD
    initGLAD();

    // Set viewport size
    glViewport(0,0,settings->xRes,settings->yRes); // Origin is bottom left

    // Disable Cursor
    //glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

    // Test for objects in front of each other
    glEnable(GL_DEPTH_TEST);

}

void Window::initGLAD() {
    // Initialise GLAD - setup OpenGL pointers
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
}



