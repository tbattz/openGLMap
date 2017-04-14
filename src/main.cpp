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

// openGL Includes
#include "../src/window.h"
#include "../src/shader.h"
#include "../src/model.h"
#include "../src/fonts.h"
#include "../src/light.h"
#include "../src/imageTile.h"
#include "../src/mavlinkReceive.h"
#include "../src/mavAircraft.h"
#include "../src/skybox.h"
#include "../src/telemOverlay.h"
#include "../src/loadingScreen.h"

// openGLPlotLive Includes
#include "../openGLPlotLive/src/fonts.h"
#include "../openGLPlotLive/src/line2d.h"
#include "../openGLPlotLive/src/plot.h"
#include "../openGLPlotLive/src/window.h"

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

// Define Fonts based on OS
#ifdef _WIN32
	#define FONTPATH "C:/Windows/Fonts/Arial.ttf"
#elif __linux__
	#define FONTPATH "/usr/share/fonts/truetype/abyssinica/AbyssinicaSIL-R.ttf"
#endif

// File check time tracking
GLfloat fileChecklast = 0.0f;



int main(int argc, char* argv[]) {
	/* ======================================================
	 *                     Setup Window
	   ====================================================== */
	// Init GLFW
	GLFWwindow* window = initGLFW(&screenWidth,&screenHeight);

	// Initialise GLEW - setup OpenGL pointers
	initGLEW();

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

	// Wireframe command line argument
	if(wireFrameOn) {
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}

	/* ======================================================
	 *                  Create Loading Screen
	   ====================================================== */
	LoadingScreen loadingScreen(window, &screenWidth, &screenHeight);

	/* ======================================================
	 *                  	  Shaders
	   ====================================================== */
	// Setup and compile shaders
	Shader lightingShader("../Shaders/multiple_lighting.vs","../Shaders/multiple_lighting.frag");
	loadingScreen.appendLoadingMessage("Finished lightingShader load attempt.");
	Shader tileShader("../Shaders/tileImage.vs","../Shaders/tileImage.frag");
	loadingScreen.appendLoadingMessage("Finished tileShader load attempt.");
	Shader skyboxShader("../Shaders/skybox.vs","../Shaders/skybox.frag");
	loadingScreen.appendLoadingMessage("Finished skyboxShader load attempt.");
	Shader simpleShader("../Shaders/telemOverlay.vs","../Shaders/telemOverlay.frag");
	loadingScreen.appendLoadingMessage("Finished simpleShader load attempt.");

	/* ======================================================
	 *                         Fonts
	   ====================================================== */
	// Load Font Shader
	Shader textShader = setupFontShader("../Shaders/font.vs", "../Shaders/font.frag",screenWidth,screenHeight);
	loadingScreen.appendLoadingMessage("Finished textShader load attempt.");

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
	loadingScreen.appendLoadingMessage("Finished loading fonts.");

	/* ======================================================
	 *                        Models
	   ====================================================== */
	// Load Models
	//Model mavAircraft("../Models/wheel/wheelTest2.obj");
	MavAircraft mavAircraft("../Models/X8/x8Fixed.obj",glm::vec3(-37.958926f, 145.238343f, 0.0f));
	loadingScreen.appendLoadingMessage("Finished loading mavAircraft.");

	// Create thread to recieve Mavlink messages
	//std::thread mavlinkThread(mavlinkMain,"192.168.1.1", "14550");
	MavSocket mavSocket("192.168.1.1", "14550",&mavAircraft);
	std::thread mavThread(&MavSocket::startSocket,&mavSocket);
	loadingScreen.appendLoadingMessage("Created mavSocket.");

	// Create Skybox
	vector<const GLchar*> faces;
	faces.push_back("../Models/skybox/right.png");
	faces.push_back("../Models/skybox/left.png");
	faces.push_back("../Models/skybox/top.png");
	faces.push_back("../Models/skybox/bottom.png");
	faces.push_back("../Models/skybox/back.png");
	faces.push_back("../Models/skybox/front.png");
	Skybox skybox(faces);
	loadingScreen.appendLoadingMessage("Finished loading skybox.");

	/* ======================================================
	 *                      Overlays
	   ====================================================== */
	// Create Telem Overlay
	TelemOverlay telemOverlay(&mavAircraft,&textShader,&telemFont,screenWidth,screenHeight);
	loadingScreen.appendLoadingMessage("Finished loading telemetry overlay.");

	// Create Tiles
	glm::vec3 origin = glm::vec3(-37.958926f, 145.238343f, 0.0f);
	GLfloat fovX = 48.3/2.0;
	GLfloat fovY = 36.8/2.0;
	TileList imageTileList(origin, fovX, fovY, window);

	// Get Tile Information
	imageTileList.updateTileList("../ImageData",&loadingScreen);

	/* ======================================================
	 *                         Lights
	   ====================================================== */
	// Load Lights
	DirectionalLight myDirLight({-0.2f,-1.0f,-0.3f}, {0.5f,0.5f,0.5f}, {0.4f,0.4f,0.4f}, {0.5f,0.5f,0.5f}, &lightingShader,0);

	// Set number of lights
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nDirLight"),1);
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nPointLight"),0);
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nSpotLight"),0);

	loadingScreen.appendLoadingMessage("Loaded Lights.");

	/* ======================================================
	 *                     Plotting Data
	   ====================================================== */
	// Create Window Dimensions Class
	GLPL::WinDimensions winDim(window);
	// Setup Shader
	GLPL::Shader plot2dShader("../openGLPlotLive/Shaders/plot2d.vs","../openGLPlotLive/Shaders/plot2d.frag");
	// Create Plot
	GLPL::Plot myplot(0.0, 0.25, 0.75, 0.75, &winDim);
	// Create Line
	//GLPL::Line2DVecGLMV3 line1(&(mavAircraft.positionHistory),1,0);
	//GLPL::Line2DVecfVecGLMV3 line1(&(mavAircraft.timePositionHistory),&(mavAircraft.positionHistory),0);
	GLPL::Line2DVecfVecGLMV3 line1(&(mavAircraft.tempTime),&(mavAircraft.tempVec),0);
	line1.colour = LC_BLUE;
	GLPL::Line2DVecfVecGLMV3 line2(&(mavAircraft.timePositionHistory),&(mavAircraft.positionHistory),0,GL_POINTS);
	// Add line to axes
	myplot.axes.addLine(&line2);
	myplot.axes.addLine(&line1);
	myplot.axes.autoScaleRound = false;
	myplot.axes.maxXRange = 10.0;

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

		// Update View
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
			imageTileList.updateTileList("../ImageData",&loadingScreen);
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

		// Draw Plot
		GLPL::preLoopDraw(false,&winDim);
		line1.updateInternalData();
		line2.updateInternalData();
		myplot.Draw(plot2dShader);


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
			sh << "Toggle Mouse Movement:  p\n";
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




