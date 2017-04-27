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
#include "window.h"
#include "settings.h"
#include "shader.h"
#include "model.h"
#include "fonts.h"
#include "light.h"
#include "imageTile.h"
#include "mavlinkReceive.h"
#include "mavAircraft.h"
#include "skybox.h"
#include "telemOverlay.h"
#include "satTiles.h"

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
	 *                      Load Settings
	   ====================================================== */
	Settings settings("../Configs/currentConfig.txt");

	/* ======================================================
	 *                     Setup Window
	   ====================================================== */
	// Init GLFW
	GLFWwindow* window = initGLFW(&settings);

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
	loadingScreen.appendLoadingMessage("Loading lightingShader.");
	Shader lightingShader("../Shaders/multiple_lighting.vs","../Shaders/multiple_lighting.frag");
	loadingScreen.appendLoadingMessage("Loading tileShader.");
	Shader tileShader("../Shaders/tileImage.vs","../Shaders/tileImage.frag");
	loadingScreen.appendLoadingMessage("Loading skyboxShader.");
	Shader skyboxShader("../Shaders/skybox.vs","../Shaders/skybox.frag");
	loadingScreen.appendLoadingMessage("Loading simpleShader.");
	Shader simpleShader("../Shaders/telemOverlay.vs","../Shaders/telemOverlay.frag");

	/* ======================================================
	 *                         Fonts
	   ====================================================== */
	// Load Font Shader
	loadingScreen.appendLoadingMessage("Loading textShader.");
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
	loadingScreen.appendLoadingMessage("Loading fonts.");
	GLFont helpFont = GLFont(FONTPATH);


	/* ======================================================
	 *                        Models
	   ====================================================== */
	// Load Models
	loadingScreen.appendLoadingMessage("Loading mavAircraft.");
	MavAircraft mavAircraft("../Models/X8/x8Fixed.obj",glm::vec3(-37.958926f, 145.238343f, 0.0f));

	// Create thread to recieve Mavlink messages
	loadingScreen.appendLoadingMessage("Creating mavSocket.");
	MavSocket mavSocket("192.168.1.1", "14550",&mavAircraft);
	std::thread mavThread(&MavSocket::startSocket,&mavSocket);

	// Create Skybox
	loadingScreen.appendLoadingMessage("Loading skybox.");
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
	loadingScreen.appendLoadingMessage("Loading telemetry overlay.");
	TelemOverlay telemOverlay(&mavAircraft,&textShader,&telemFont,screenWidth,screenHeight);

	// Create Origin
	glm::vec3 origin = glm::vec3(-37.958945f, 145.238349f, 0.0f);

	// Create Tiles
	GLfloat fovX = 48.3/2.0;
	GLfloat fovY = 36.8/2.0;
	TileList imageTileList(origin, fovX, fovY, window);
	// Get Tile Information
	imageTileList.updateTileList("../ImageData",&loadingScreen);

	// Create Satellite Tiles
	SatTileList satTileList(origin,&mavAircraft);

	/* ======================================================
	 *                         Lights
	   ====================================================== */
	loadingScreen.appendLoadingMessage("Loading Lights.");
	// Load Lights
	DirectionalLight myDirLight({-0.2,-1.0,-0.3}, {0.5,0.5,0.5}, {0.4,0.4,0.4}, {0.5,0.5,0.5}, &lightingShader,0);

	// Set number of lights
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nDirLight"),1);
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nPointLight"),0);
	glUniform1i(glGetUniformLocation(lightingShader.Program,"numLights.nSpotLight"),0);

	/* ======================================================
	 *                     Plotting Data
	   ====================================================== */
	// Create Window Dimensions Class
	GLPL::WinDimensions winDim(window);
	// Setup Shader
	GLPL::Shader plot2dShader("../openGLPlotLive/Shaders/plot2d.vs","../openGLPlotLive/Shaders/plot2d.frag");
	// Create Plot
	GLPL::Plot myplot(0.75, 0.0, 0.25, 0.25, &winDim);
	// Create Line
	// Position
	GLPL::Line2DVecfVecGLMV3 pos1(&(mavAircraft.tempTime),&(mavAircraft.tempPos),0);
	pos1.colour = LC_BLUE;
	GLPL::Line2DVecfVecGLMV3 pos2(&(mavAircraft.tempTime),&(mavAircraft.tempPos),1);
	pos2.colour = LC_BLUE;
	GLPL::Line2DVecfVecGLMV3 pos3(&(mavAircraft.tempTime),&(mavAircraft.tempPos),2);
	pos3.colour = LC_BLUE;
	// Real Position
	GLPL::Line2DVecfVecGLMV3 rpos1(&(mavAircraft.timePositionHistory),&(mavAircraft.positionHistory),0,GL_POINTS);
	GLPL::Line2DVecfVecGLMV3 rpos2(&(mavAircraft.timePositionHistory),&(mavAircraft.positionHistory),1,GL_POINTS);
	GLPL::Line2DVecfVecGLMV3 rpos3(&(mavAircraft.timePositionHistory),&(mavAircraft.positionHistory),2,GL_POINTS);
	// Attitude
	GLPL::Line2DVecfVecGLMV3 att1(&(mavAircraft.tempTime2),&(mavAircraft.tempAtt),0);
	att1.colour = LC_RED;
	GLPL::Line2DVecfVecGLMV3 att2(&(mavAircraft.tempTime2),&(mavAircraft.tempAtt),1);
	att2.colour = LC_RED;
	GLPL::Line2DVecfVecGLMV3 att3(&(mavAircraft.tempTime2),&(mavAircraft.tempAtt),2);
	att3.colour = LC_RED;
	GLPL::Line2DVecfVecGLMV3 ratt1(&(mavAircraft.timeAttitudeHistory),&(mavAircraft.attitudeHistory),0,GL_POINTS);
	GLPL::Line2DVecfVecGLMV3 ratt2(&(mavAircraft.timeAttitudeHistory),&(mavAircraft.attitudeHistory),1,GL_POINTS);
	GLPL::Line2DVecfVecGLMV3 ratt3(&(mavAircraft.timeAttitudeHistory),&(mavAircraft.attitudeHistory),2,GL_POINTS);
	// Velocity
	GLPL::Line2DVecfVecGLMV3 vel1(&(mavAircraft.tempTime),&(mavAircraft.tempVel),0);
	vel1.colour = LC_GREEN;
	GLPL::Line2DVecfVecGLMV3 vel2(&(mavAircraft.tempTime),&(mavAircraft.tempVel),1);
	vel2.colour = LC_GREEN;
	GLPL::Line2DVecfVecGLMV3 vel3(&(mavAircraft.tempTime),&(mavAircraft.tempVel),2);
	vel3.colour = LC_GREEN;

	// Path Plotting
	GLPL::Line2DVecGLMV3 map(&(mavAircraft.positionHistory),1,0);

	// Add line to axes
//	myplot.axes.addLine(&rpos1);
//	myplot.axes.addLine(&rpos2);
//	myplot.axes.addLine(&rpos3);
//	myplot.axes.addLine(&pos1);
//	myplot.axes.addLine(&pos2);
//	myplot.axes.addLine(&pos3);
//	myplot.axes.addLine(&ratt1);
//	myplot.axes.addLine(&ratt2);
//	myplot.axes.addLine(&ratt3);
//	myplot.axes.addLine(&att1);
//	myplot.axes.addLine(&att2);
//	myplot.axes.addLine(&att3);
//	myplot.axes.addLine(&vel1);
//	myplot.axes.addLine(&vel2);
//	myplot.axes.addLine(&vel3);
	myplot.axes.addLine(&map);

	myplot.axes.autoScaleRound = false;
	//myplot.axes.maxXRange = 10.0;

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

		// Update Aircraft Position
		mavAircraft.updatePositionAttitude();

		// Do keyboard movement
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
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight,0.1f,10000.0f);
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
		if ((currentFrame - fileChecklast) > 0.3) {
			// Update image file list
			//imageTileList.updateTileList("../ImageData",&loadingScreen);
			// Load satellite tiles
			satTileList.updateTiles();
			// Update file check time
			fileChecklast = currentFrame;
		}



		// Draw tiles
		imageTileList.Draw(tileShader);
		// Draw Satellite tiles
		satTileList.Draw(tileShader);

		// Draw Skybox last
		skyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
		skybox.Draw(skyboxShader);

		// Draw Plot
		GLPL::preLoopDraw(false,&winDim);
		rpos1.updateInternalData();
		rpos2.updateInternalData();
		rpos3.updateInternalData();
		pos1.updateInternalData();
		pos2.updateInternalData();
		pos3.updateInternalData();
		ratt1.updateInternalData();
		ratt2.updateInternalData();
		ratt3.updateInternalData();
		att1.updateInternalData();
		att2.updateInternalData();
		att3.updateInternalData();
		vel1.updateInternalData();
		vel2.updateInternalData();
		vel3.updateInternalData();
		map.updateInternalData();
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
		//std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0/5.0)));

	}

	glfwTerminate();
	// Close mavlink socket
	mavSocket.closeSocket();
	mavThread.join();
	// Stop Satellite Tile Threads
	satTileList.stopThreads();

	return 0;
}




