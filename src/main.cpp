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
#include <chrono>
#include <thread>

// GLEW (OpenGL Extension Wrangler Library)
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// openGL Includes
#include "../src/shader.h"
#include "../src/camera.h"
#include "../src/model.h"
#include "../src/fonts.h"
#include "../src/light.h"
#include "../src/imageTile.h"
#include "../src/mavlinkReceive.h"
#include "../src/mavAircraft.h"
#include "../src/skybox.h"
#include "../src/telemOverlay.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL Image loading library
#include <SOIL.h>

// Free Type Library
#include <ft2build.h>
#include FT_FREETYPE_H

using std::vector;

// Camera View Setup
Camera camera(glm::vec3(0.0f, 3.0f, 3.0f));
bool keys[1024];
bool toggleKeys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Define Fonts based on OS
#ifdef _WIN32
	#define FONTPATH "C:/Windows/Fonts/Arial.ttf"
#elif __linux__
	#define FONTPATH "/usr/share/fonts/truetype/abyssinica/AbyssinicaSIL-R.ttf"
#endif

// X Position
GLfloat xval = 0.0f;

// File check time tracking
GLfloat fileChecklast = 0.0f;

// Functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

int main(int argc, char* argv[]) {
	/* ======================================================
	 *                  Command Line Arguments
	   ====================================================== */
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
	/* ======================================================
	 *                     Setup Window
	   ====================================================== */
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE); // Set core profile
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

	// Screen Properties
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	GLfloat screenHeight = mode->height*0.9;
	GLfloat screenWidth  = mode->width*0.9;

	GLFWwindow* window = glfwCreateWindow(screenWidth,screenHeight,"openGLMap",nullptr,nullptr);
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

	/* ======================================================
	 *                  	  Shaders
	   ====================================================== */
	// Setup and compile shaders
	Shader lightingShader("../Shaders/multiple_lighting.vs","../Shaders/multiple_lighting.frag");
	Shader tileShader("../Shaders/tileImage.vs","../Shaders/tileImage.frag");
	Shader skyboxShader("../Shaders/skybox.vs","../Shaders/skybox.frag");
	Shader simpleShader("../Shaders/telemOverlay.vs","../Shaders/telemOverlay.frag");

	/* ======================================================
	 *                         Fonts
	   ====================================================== */
	// Load Font Shader
	Shader textShader = setupFontShader("../Shaders/font.vs", "../Shaders/font.frag",screenWidth,screenHeight);

	// Load Telemetry Font
	GLFont telemFont = GLFont(FONTPATH);

	// Load Font
	GLFont* fpsFontPt;

	Shader* textShaderPt = &textShader;
	if(fpsOn) {
		fpsFontPt = new GLFont(FONTPATH);
	}

	// Help Font
	GLFont helpFont = GLFont(FONTPATH);

	/* ======================================================
	 *                        Models
	   ====================================================== */
	// Load Models
	//Model mavAircraft("../Models/wheel/wheelTest2.obj");
	MavAircraft mavAircraft("../Models/X8/x8Fixed.obj",glm::vec3(-37.958926f, 145.238343f, 0.0f));

	// Create thread to recieve Mavlink messages
	//std::thread mavlinkThread(mavlinkMain,"192.168.1.1", "14550");
	MavSocket mavSocket("192.168.1.1", "14550",&mavAircraft);
	std::thread mavThread(&MavSocket::startSocket,&mavSocket);

	// Create Skybox
	vector<const GLchar*> faces;
	faces.push_back("../Models/skybox/right.png");
	faces.push_back("../Models/skybox/left.png");
	faces.push_back("../Models/skybox/top.png");
	faces.push_back("../Models/skybox/bottom.png");
	faces.push_back("../Models/skybox/back.png");
	faces.push_back("../Models/skybox/front.png");
	Skybox skybox(faces);

	/* ======================================================
	 *                      Overlays
	   ====================================================== */
	// Create Telem Overlay
	TelemOverlay telemOverlay(&mavAircraft,&textShader,&telemFont,screenWidth,screenHeight);

	// Create Tiles
	glm::vec3 origin = glm::vec3(-37.958926f, 145.238343f, 0.0f);
	GLfloat fovX = 48.3/2.0;
	GLfloat fovY = 36.8/2.0;
	TileList imageTileList(origin, fovX, fovY);

	// Get Tile Information
	imageTileList.updateTileList("../ImageData");

	/* ======================================================
	 *                         Lights
	   ====================================================== */
	// Load Lights
	DirectionalLight myDirLight({-0.2f,-1.0f,-0.3f}, {0.5f,0.5f,0.5f}, {0.4f,0.4f,0.4f}, {0.5f,0.5f,0.5f}, &lightingShader,0);

	// Set number of lights
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nDirLight"),1);
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nPointLight"),0);
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nSpotLight"),0);

	/* ======================================================
	 *                     Drawing Loop
	   ====================================================== */
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

		lightingShader.Use();

		// Update Chase View
		camera.setupView(&mavAircraft);

		// Update View Position Uniform
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		// Transformation Matrices
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight,0.1f,1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program,"view"),1,GL_FALSE,glm::value_ptr(view));


		// Draw Model
		mavAircraft.Draw(lightingShader);

		// Draw telem overlay
		simpleShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(simpleShader.Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(simpleShader.Program,"view"),1,GL_FALSE,glm::value_ptr(view));
		telemOverlay.Draw(simpleShader, projection, view, &camera);

		// Draw tile(s)
		tileShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(tileShader.Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(tileShader.Program,"view"),1,GL_FALSE,glm::value_ptr(view));
		// Check for new files
		if ((currentFrame - fileChecklast) > 1.0) {
			// Update image file list
			imageTileList.updateTileList("../ImageData");
			fileChecklast = currentFrame;
		}

		// Draw tiles
		for(unsigned int i = 0; i != imageTileList.tiles.size(); i++) {
			(imageTileList.tiles[i]).Draw(tileShader);
		}

		// Draw Skybox last
		skyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
		skybox.Draw(skyboxShader);

		// Draw Airspeed
		//telemOverlay.DrawAirspeed();

		// Print FPS
		if(fpsOn) {
			std::stringstream ss;
			ss << int(1.0f/deltaTime) << " fps";
			fpsFontPt->RenderText(textShaderPt,ss.str(),screenWidth-150.0f,screenHeight-50.0f,1.0f,glm::vec3(0.0f, 1.0f, 0.0f),0);
			//std::cout << (1.0f/deltaTime) << "fps" << "\r";
		}

		// Overlay Help Menu
		//std::cout << toggleKeys[GLFW_KEY_H] << '\n';
		if(toggleKeys[GLFW_KEY_H]) {
			std::stringstream sh;
			// Note: Text lines rendered from bottom to top
			sh << "HELP MENU\n";
			sh << "Toggle Help:              h\n";
			sh << "Increment view:           v\n";
			sh << "Toggle Mouse Movement: p\n";
			(&helpFont)->RenderText(textShaderPt,sh.str(),0.0f,0.0f,1.0f,glm::vec3(1.0f, 1.0f, 0.0f),1);
		}

		// Swap buffers
		glfwSwapBuffers(window);

		// Sleep to lower framerate
		//std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0/30.0)));

	}

	glfwTerminate();
	// Close mavlink socket
	mavSocket.closeSocket();
	mavThread.join();

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
