/*
 * main.cpp
 *
 *  Created on: 1Jan.,2017
 *      Author: bcub3d-desktop
 *
 *  Based on tutorials from http://www.learnopengl.com/#!Introduction
 */

// Standard Includes
#include <string>
#include <unistd.h>
#include <memory>

// GLEW (OpenGL Extension Wrangler Library)
#include <GL/glew.h>

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// openGL Includes
#include "../src/shader.h"
#include "../src/camera.h"
#include "../src/model.h"
#include "../src/fonts.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL Image loading library
#include <SOIL.h>

// Free Type Library
#include <ft2build.h>
#include FT_FREETYPE_H

// Screen Properties
GLuint screenWidth = 1920, screenHeight = 1080;

// Camera View Setup
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// X Position
GLfloat xval = 0.0f;

// Functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

int main(int argc, char* argv[]) {
	// Parse Command Line Arguments
	bool wireFrameOn = false;
	bool fpsOn = false;
	int opt;
	while((opt = getopt(argc, argv, "wf")) != -1) {
		switch(opt) {
		case 'w': wireFrameOn = true; break;
		case 'f': fpsOn = true; break;
		}
	}

	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE); // Set core profile
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(screenWidth,screenHeight,"openGLRace",nullptr,nullptr);
	glfwMakeContextCurrent(window);

	// Setup Callbacks for user input
	glfwSetKeyCallback(window,key_callback);
	glfwSetCursorPosCallback(window,mouse_callback);
	glfwSetScrollCallback(window,scroll_callback);

	// Disable Cursor
	//glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

	// Initialise GLEW - setup OpenGL pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Set viewport size
	glViewport(0,0,screenWidth,screenHeight); // Origin is bottom left

	// Wireframe command line argument
	if(wireFrameOn) {
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}

	// Test for objects in front of each other
	glEnable(GL_DEPTH_TEST);

	// Setup and compile shaders
	Shader shader("../Shaders/basic_lighting.vs","../Shaders/basic_lighting.frag");
	Shader lampShader("../Shaders/lamp.vs","../Shaders/lamp.frag");


	// Load Models
	Model ourModel("../Models/wheel/wheelTest.obj");
	Model ground("../Models/wheel/ground.obj");
	Model lamp("../Models/wheel/lampCube.obj");

	// Light Attributes
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	// Load Font
	GLFont* arialFontPt;
	Shader textShader = setupFontShader("../Shaders/font.vs", "../Shaders/font.frag",screenWidth,screenHeight);
	Shader* textShaderPt = &textShader;
	if(fpsOn) {
		arialFontPt = new GLFont("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf");
		textShaderPt = new Shader(setupFontShader("../Shaders/font.vs", "../Shaders/font.frag",screenWidth,screenHeight));
	}

	// Set Texture Units
	shader.Use();
	glUniform1i(glGetUniformLocation(shader.Program,"material.diffuse"),0);
	glUniform1i(glGetUniformLocation(shader.Program,"material.specular"),1);




	// Game Loop
	while(!glfwWindowShouldClose(window)) {
		// Set Frame Time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check Events
		glfwPollEvents();
		do_movement();

		// Clear the colour buffer
		glClearColor(0.64f, 0.64f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();

        GLint lightPosLoc             = glGetUniformLocation(shader.Program, "light.position");
        GLint lightSpotdirLoc         = glGetUniformLocation(shader.Program, "light.direction");
        GLint viewPosLoc              = glGetUniformLocation(shader.Program, "viewPos");
        glUniform3f(lightPosLoc,             camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(lightSpotdirLoc,         camera.Front.x, camera.Front.y, camera.Front.z);
        glUniform3f(viewPosLoc,              camera.Position.x, camera.Position.y, camera.Position.z);
        // Set lights properties
        glUniform3f(glGetUniformLocation(shader.Program, "light.ambient"),   0.5f, 0.5f, 0.5f);
        // We set the diffuse intensity a bit higher; note that the right lighting conditions differ with each lighting method and environment.
        // Each environment and lighting type requires some tweaking of these variables to get the best out of your environment.
        glUniform3f(glGetUniformLocation(shader.Program, "light.diffuse"),   0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(shader.Program, "light.specular"),  1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "light.constant"),  1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "light.linear"),    0.09);
        glUniform1f(glGetUniformLocation(shader.Program, "light.quadratic"), 0.032);
        // Set material properties
        glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 2.0f);

		// Transformation Matrices
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight,0.1f,100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program,"view"),1,GL_FALSE,glm::value_ptr(view));

		// Draw ground
		glm::mat4 model2;
		model2 = glm::translate(model2, glm::vec3(0.0f,-1.75f, 0.0f)); // Translate down
		model2 = glm::scale(model2, glm::vec3(0.2f, 0.2f, 0.2f)); // Scale to screen
		glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE,glm::value_ptr(model2));
		ground.Draw(shader);

		// Draw Model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f,-1.75f, xval)); // Translate down
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); // Scale to screen
		glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE,glm::value_ptr(model));
		ourModel.Draw(shader);

		// Print FPS
		if(fpsOn) {
			std::stringstream ss;
			ss << int(1.0f/deltaTime) << " fps";
			arialFontPt->RenderText(textShaderPt,ss.str(),screenWidth-150.0f,screenHeight-50.0f,1.0f,glm::vec3(0.0f, 1.0f, 0.0f));
			//std::cout << (1.0f/deltaTime) << "fps" << "\r";
		}

		// Swap buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
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

	camera.ProcessMouseMovement(xoffset,yoffset);
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
