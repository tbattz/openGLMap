/*
 * window.cpp
 *
 *  Created on: 23Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "window.h"


/* Camera and Screen Setup */
// Camera View Setup
Camera camera(glm::vec3(0.0f, 3.0f, 3.0f));
bool keys[1024];
bool toggleKeys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

// Screen Dimensions
GLfloat screenHeight = 1080;
GLfloat screenWidth = 1920;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// X Position
GLfloat xval = 0.0f;


/* Functions */
GLFWwindow* initGLFW(Settings* s) {
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
	if(s->fullscreen) {
		s->xRes  = mode->width;
		s->yRes = mode->height;
	}
	glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
	GLFWwindow* window;
	if(s->fullscreen) {
		window = glfwCreateWindow(s->xRes,s->yRes,"openGLMap",monitors[s->screenID-1],nullptr);
	} else {
		window = glfwCreateWindow(s->xRes,s->yRes,"openGLMap",nullptr,nullptr);
	}
	glfwMakeContextCurrent(window);

	// Set viewport size
	glViewport(0,0,s->xRes,s->yRes); // Origin is bottom left

	// Disable Cursor
	//glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

	// Test for objects in front of each other
	glEnable(GL_DEPTH_TEST);

	// Setup Callbacks for user input
	glfwSetKeyCallback(window,key_callback);
	glfwSetCursorPosCallback(window,mouse_callback);
	glfwSetScrollCallback(window,scroll_callback);

	return window;
}

void initGLEW() {
	// Initialise GLEW - setup OpenGL pointers
	glewExperimental = GL_TRUE;
	glewInit();
}

// Smoothing key input callback
void key_callback(GLFWwindow* window,int key,int scancode, int action, int mode) {
	// ESC to close window
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window,GL_TRUE);
	}
	// Set key states
	if(action == GLFW_PRESS) {
		keys[key] = true;
		// Set toggle states
		toggleKeys[key] = !toggleKeys[key];
		// Change View
		if(key==GLFW_KEY_V) {
			camera.view += 1;
			if(camera.view > 3) {
				camera.view = 0;
			}
		}
	} else if (action == GLFW_RELEASE) {
		keys[key] = false;
	}
}

// Callback to position the camera
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// Reset mouse to center of window
	if(firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// Offset for smoother movement
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	// Don't move if camera movement paused
	if (!toggleKeys[80]) { // p key to toggle
		camera.ProcessMouseMovement(xoffset,yoffset);
	}
}

// Scrolling zoom callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		camera.ProcessMouseScroll(yoffset);
}

// Move camera using wasd keys
void do_movement() {
	// Camera Controls
	if(keys[GLFW_KEY_W]) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if(keys[GLFW_KEY_S]) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if(keys[GLFW_KEY_A]) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if(keys[GLFW_KEY_D]) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if(keys[GLFW_KEY_UP]) {
		xval += 0.05f;
	}
	if(keys[GLFW_KEY_DOWN]) {
		xval -= 0.05f;
	}
}


