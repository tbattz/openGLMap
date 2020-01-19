/*
 * window.h
 *
 *  Created on: 1Apr.,2017
 *      Author: bcub3d-desktop
 */

#ifndef WINDOW_H_
#define WINDOW_H_

// GLEW (OpenGL Extension Wrangler Library)
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// openGL Includes
#include "view/camera.h"
#include "settings.h"


/* Camera and Screen Setup */
// Camera View Setup
extern Camera camera;
extern bool keys[1024];
extern bool toggleKeys[1024];
extern GLfloat lastX, lastY;
extern bool firstMouse;

// Screen Dimensions
extern GLfloat screenHeight;
extern GLfloat screenWidth;

extern GLfloat deltaTime;
extern GLfloat lastFrame;

// X Position
extern GLfloat xval;


/* Functions */
GLFWwindow* initGLFW(Settings* settings);
void initGLEW();
void key_callback(GLFWwindow* window,int key,int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();


#endif /* WINDOW_H_ */
