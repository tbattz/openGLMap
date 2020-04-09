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

// openGLPlotLive Includes
#include "../openGLPlotLive/src/fonts.h"
#include "../openGLPlotLive/src/plot/plot.h"

// openGL Includes
#include "utilities/settings.h"
#include "renderEngine/shader.h"
#include "renderEngine/objectLoading/model.h"
#include "renderEngine/fonts.h"
#include "renderEngine/light.h"
#include "mavlinkReceive.h"
#include "renderEngine/skybox/skybox.h"
#include "telemOverlay.h"
#include "volumes.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SOIL Image loading library
#include <SOIL.h>

// Free Type Library
#include <ft2build.h>

#include FT_FREETYPE_H

// Simulation Engine
#include "renderEngine/RenderEngine.h"
// Input Controller
#include "userInput/InputController.h"
// Object Controllers
#include <objects/worldObject/WorldObjectController.h>
#include <objects/worldObject/WorldGeoObjectController.h>
#include <objects/externalInput/MavlinkGeoObjectController.h>
// Satelite Tiles
#include <objects/satTiles/SatTileGroupController.h>


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
	// TODO - Fix relative paths
	Settings settings("../../Configs/currentConfig.txt");

    /* ======================================================
     *                Create Simulation RenderEngine
       ====================================================== */
    RenderEngine renderEngine(&settings);

    /* ======================================================
     *                 Create Input Controller
       ====================================================== */
    std::shared_ptr<RenderEngine> renderEnginePt = std::make_shared<RenderEngine>(renderEngine);
    InputController inputController = InputController(renderEnginePt);

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
    renderEngine.setWireFrameBool(wireFrameOn);

	// Fps command line argument
	renderEngine.setFpsOn(fpsOn);

	// World Axes
	std::shared_ptr<AxesView> axesView = std::shared_ptr<AxesView>(new AxesView());
	renderEngine.registerAxesView(axesView);
    renderEngine.toggleAxes(true);


	/* Create controllers */
    const GLchar* path = settings.aircraftConList[0].filepath.c_str(); // TODO - Fix this hardcoding
    std::string host = settings.aircraftConList[0].ipString;
    std::string port = settings.aircraftConList[0].port;
    // XYZ Aircraft
    std::shared_ptr<IWorldObjectController> worldObjectBaseController = std::shared_ptr<IWorldObjectController>(new WorldObjectController(path));
    std::shared_ptr<WorldObjectController> worldObjectController = std::static_pointer_cast<WorldObjectController>(worldObjectBaseController);
    renderEngine.registerWorldObjectController(worldObjectBaseController);
    // LatLonAlt Aircraft
    glm::dvec3 origin = glm::dvec3(settings.origin[0], settings.origin[1], settings.origin[2]);
    /*std::shared_ptr<WorldGeoObjectController> worldGeoObjectController;
    worldGeoObjectController = std::make_shared<WorldGeoObjectController>(path, origin, origin);
    renderEngine.registerWorldGeoObjController(worldGeoObjectController);*/
    // MavAircraft
    std::string name = "MavAircraft_1";
    std::shared_ptr<IWorldObjectController> mavlinkGeoObjectController;
    mavlinkGeoObjectController = std::shared_ptr<IWorldObjectController>(new MavlinkGeoObjectController(name, host, port, path, origin, origin));
    renderEngine.registerWorldObjectController(mavlinkGeoObjectController);

    // Set aircraft for camera
    renderEngine.getCamera()->setCurrentAircraft(mavlinkGeoObjectController);

	// Create Skybox
    renderEngine.loadingScreen->appendLoadingMessage("Loading skybox.");
	vector<const GLchar*> faces;
	// TODO - Fix relative paths
	faces.push_back("../../Models/skybox/right.png");
	faces.push_back("../../Models/skybox/left.png");
	faces.push_back("../../Models/skybox/top.png");
	faces.push_back("../../Models/skybox/bottom.png");
	faces.push_back("../../Models/skybox/back.png");
	faces.push_back("../../Models/skybox/front.png");
	Skybox skybox(faces);

	/* ======================================================
	 *                       Overlays
	   ====================================================== */
	// Create Origin
	//glm::vec3 origin = worldOrigin;

	// Create Tiles
	//GLfloat fovX = 48.3/2.0;
	//GLfloat fovY = 36.8/2.0;
	//TileList imageTileList(origin, fovX, fovY, renderEngine.window);
	// Get Tile Information
	//imageTileList.updateTileList("../ImageData",renderEngine.loadingScreen);

	// Create Satellite Tiles
	//SatTileList satTileList(origin,&mavAircraftList[0]);
	//glm::vec3 origin = glm::vec3(settings.origin[0], settings.origin[1], settings.origin[2]);
	std::shared_ptr<SatTileGroupController> satTileGroupController = std::make_shared<SatTileGroupController>(origin, HYBRID);
	//satTileGroupController->setWorldObjectController(mavlinkGeoObjectController);
	satTileGroupController->setWorldObjectController(mavlinkGeoObjectController);
	renderEngine.registerTileController(satTileGroupController);

	/* ======================================================
	 *                        Volumes
	   ====================================================== */
	// Create Volumes
	//std::vector<Volume> volumeList;
	/*VolumeList volumeList(&camera);
	for(unsigned int i=0; i<settings.volumeList.size(); i++) {
		// Create Volume
		volumeList.addVolume(Volume(origin, settings.volumeList[i]));
	}*/


	/* ======================================================
	 *                         Lights
	   ====================================================== */
    renderEngine.loadingScreen->appendLoadingMessage("Loading Lights.");
	// Load Lights
	DirectionalLight myDirLight({-0.2,-1.0,-0.3}, {0.5,0.5,0.5}, {0.4,0.4,0.4}, {0.5,0.5,0.5}, renderEngine.lightingShader,0);

	// Set number of lights
	glUniform1i(glGetUniformLocation(renderEngine.lightingShader->Program,"numLights.nDirLight"),1);
	glUniform1i(glGetUniformLocation(renderEngine.lightingShader->Program,"numLights.nPointLight"),0);
	glUniform1i(glGetUniformLocation(renderEngine.lightingShader->Program,"numLights.nSpotLight"),0);

	/* ======================================================
	 *                     Plotting Data
	   ====================================================== */
	// Create Window Dimensions Class
	//GLPL::WinDimensions winDim(renderEngine.window);
	// Setup Shader
	GLPL::Shader plot2dShader("../../openGLPlotLive/Shaders/plot2d.vs","../../openGLPlotLive/Shaders/plot2d.frag");
	GLPL::Shader textShader("../../openGLPlotLive/Shaders/font.vs", "../../openGLPLotLive/Shaders/font.frag");
	// Create Plot
	//GLPL::Plot myplot(0.75, 0.0, 0.25, 0.25, &settings, &textShader);
	// Create Line
	// Position
	/*MavAircraft* plotAircraftPt = &mavAircraftList[0];
	GLPL::Line2DVecfVecGLMV3 pos1(&(plotAircraftPt->posTimeLog),&(plotAircraftPt->posLog),0);
	pos1.colour = LC_BLUE;
	GLPL::Line2DVecfVecGLMV3 pos2(&(plotAircraftPt->posTimeLog),&(plotAircraftPt->posLog),1);
	pos2.colour = LC_BLUE;
	GLPL::Line2DVecfVecGLMV3 pos3(&(plotAircraftPt->posTimeLog),&(plotAircraftPt->posLog),2);
	pos3.colour = LC_BLUE;
	// Real Position
	GLPL::Line2DVecfVecGLMV3 rpos1(&(plotAircraftPt->timePositionHistory),&(plotAircraftPt->positionHistory),0,GL_POINTS);
	GLPL::Line2DVecfVecGLMV3 rpos2(&(plotAircraftPt->timePositionHistory),&(plotAircraftPt->positionHistory),1,GL_POINTS);
	GLPL::Line2DVecfVecGLMV3 rpos3(&(plotAircraftPt->timePositionHistory),&(plotAircraftPt->positionHistory),2,GL_POINTS);
	// Attitude
	GLPL::Line2DVecfVecGLMV3 att1(&(plotAircraftPt->tempTime2),&(plotAircraftPt->attLog),0);
	att1.colour = LC_RED;
	GLPL::Line2DVecfVecGLMV3 att2(&(plotAircraftPt->tempTime2),&(plotAircraftPt->attLog),1);
	att2.colour = LC_RED;
	GLPL::Line2DVecfVecGLMV3 att3(&(plotAircraftPt->tempTime2),&(plotAircraftPt->attLog),2);
	att3.colour = LC_RED;
	GLPL::Line2DVecfVecGLMV3 ratt1(&(plotAircraftPt->timeAttitudeHistory),&(plotAircraftPt->attitudeHistory),0,GL_POINTS);
	GLPL::Line2DVecfVecGLMV3 ratt2(&(plotAircraftPt->timeAttitudeHistory),&(plotAircraftPt->attitudeHistory),1,GL_POINTS);
	GLPL::Line2DVecfVecGLMV3 ratt3(&(plotAircraftPt->timeAttitudeHistory),&(plotAircraftPt->attitudeHistory),2,GL_POINTS);
	// Velocity
	GLPL::Line2DVecfVecGLMV3 vel1(&(plotAircraftPt->posTimeLog),&(plotAircraftPt->velLog),0);
	vel1.colour = LC_GREEN;
	GLPL::Line2DVecfVecGLMV3 vel2(&(plotAircraftPt->posTimeLog),&(plotAircraftPt->velLog),1);
	vel2.colour = LC_GREEN;
	GLPL::Line2DVecfVecGLMV3 vel3(&(plotAircraftPt->posTimeLog),&(plotAircraftPt->velLog),2);
	vel3.colour = LC_GREEN;*/

	// Path Plotting
	/*num = settings.aircraftConList.size();
	std::vector<GLPL::Line2DVecGLMV3> mapList;
	mapList.reserve(num);
	for(unsigned int i=0; i<settings.aircraftConList.size(); i++) {
		mapList.push_back(GLPL::Line2DVecGLMV3(&(mavAircraftList[i].positionHistory),1,0));
		mapList[i].colour = renderEngine.colorVec[i];
		myplot.axes.addLine(&mapList[i]);
	}

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
//	myplot.axes.addLine(&vel3);*/

	/*myplot.axes.autoScaleRound = false;
	myplot.axes.equalAxes = true;*/
	//myplot.axes.maxXRange = 10.0;

	/* ======================================================
	 *                     Drawing Loop
	   ====================================================== */
	// Game Loop
	while(!glfwWindowShouldClose(renderEngine.window)) {
		// Set Frame Time
		GLfloat currentFrame = glfwGetTime();
		inputController.setDeltaTime(currentFrame - inputController.getLastFrameTime());
		inputController.setLastFrameTime(currentFrame);
		////deltaTime = currentFrame - lastFrame;
		////lastFrame = currentFrame;

		// Check Events
		glfwPollEvents();

		// Update Aircraft Position
		/*for(unsigned int i=0; i<mavAircraftList.size(); i++) {
			mavAircraftList[i].updatePositionAttitude();
		}*/
        worldObjectController->incrementPosition();

		// Do keyboard movement
		inputController.do_movement();

		// Update Satellite tiles
		satTileGroupController->updateTiles();

		// Clear the colour buffer
		//glClearColor(0.64f, 0.64f, 1.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //renderEngine.lightingShader->Use();

		// Update View
		//camera.setupView(&mavAircraftList);
		std::shared_ptr<Camera> camera = inputController.getCamera();
        camera->setupView();

		// Update View Position Uniform
		GLint viewPosLoc = glGetUniformLocation(renderEngine.lightingShader->Program, "viewPos");
        glUniform3f(viewPosLoc, camera->Position.x, camera->Position.y, camera->Position.z);

		// Transformation Matrices
        int screenWidth = settings.xRes;
        int screenHeight = settings.yRes;
		glm::mat4 projection = glm::perspective(camera->Zoom, (float)screenWidth/(float)screenHeight,0.1f,10000.0f);
		glm::mat4 view = camera->GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(renderEngine.lightingShader->Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(renderEngine.lightingShader->Program,"view"),1,GL_FALSE,glm::value_ptr(view));

        renderEngine.tileShader->Use();
        glUniformMatrix4fv(glGetUniformLocation(renderEngine.tileShader->Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(renderEngine.tileShader->Program,"view"),1,GL_FALSE,glm::value_ptr(view));

        // Draw Models

		renderEngine.renderFrame();

		// Draw Model
		/*for(unsigned int i=0; i<mavAircraftList.size(); i++) {
			mavAircraftsList[i].Draw(*(renderEngine.lightingShader));
		}*/


		// Draw telem overlay
		/*renderEngine.simpleShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(renderEngine.simpleShader->Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(renderEngine.simpleShader->Program,"view"),1,GL_FALSE,glm::value_ptr(view));
		for(unsigned int i=0; i<telemOverlayList.size(); i++) {
			telemOverlayList[i].Draw(*(renderEngine.simpleShader), projection, view, &camera);
		}*/

		// Draw tile(s)
		/*renderEngine.tileShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(renderEngine.tileShader->Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(renderEngine.tileShader->Program,"view"),1,GL_FALSE,glm::value_ptr(view));
		// Check for new files
		if ((currentFrame - fileChecklast) > 0.3) {
			// Update image file list
			//imageTileList.updateTileList("../ImageData",&loadingScreen);
			// Load satellite tiles
			satTileList.updateTiles();
			// Update file check time
			fileChecklast = currentFrame;
		}*/



		// Draw tiles
		/*imageTileList.Draw(*(renderEngine.tileShader));
		// Draw Satellite tiles
		satTileList.Draw(*(renderEngine.tileShader));

		// Draw Volumes
		renderEngine.volumeShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(renderEngine.volumeShader->Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(renderEngine.volumeShader->Program,"view"),1,GL_FALSE,glm::value_ptr(view));*/
		/*for(unsigned int i=0; i<settings.volumeList.size(); i++) {
			volumeList[i].Draw(volumeShader);
		}*/
		//volumeList.Draw(volumeShader);
		//lineShader.Use();
		//glUniformMatrix4fv(glGetUniformLocation(lineShader.Program,"projection"),1,GL_FALSE,glm::value_ptr(projection));
		//glUniformMatrix4fv(glGetUniformLocation(lineShader.Program,"view"),1,GL_FALSE,glm::value_ptr(view));
		/*for(unsigned int i=0; i<settings.volumeList.size(); i++) {
			volumeList[i].DrawLines(lineShader);
		}*/
		//volumeList.DrawLines(lineShader);


		// Draw Skybox last
		renderEngine.skyboxShader->Use();
		view = glm::mat4(glm::mat3(camera->GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(renderEngine.skyboxShader->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(renderEngine.skyboxShader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1i(glGetUniformLocation(renderEngine.skyboxShader->Program, "skybox"), 0);
		skybox.Draw(*(renderEngine.skyboxShader));

		// Draw Plot
		//GLPL::preLoopDraw(false,&winDim);
		/*rpos1.updateInternalData();
		rpos2.updateInternalData();
		rpos3.updateInternalData();
		pos1.updateInternalData();
		pos2.updateInternalData();
		pos3.updateInternalData();
		ratt1.updateInternalData();
		ratt2.updateInternalData();
		ratt3.updateInternalData();
		att1.updateInternalData();s
		att2.updateInternalData();
		att3.updateInternalData();
		vel1.updateInternalData();
		vel2.updateInternalData();
		vel3.updateInternalData();*/
		/*for(unsigned int i=0; i<settings.aircraftConList.size(); i++) {
			mapList[i].updateInternalData();
		}*/
		//myplot.Draw(plot2dShader);


		// Draw Airspeed
		//telemOverlay.DrawAirspeed();

		// Print FPS
		if(fpsOn) {
			std::stringstream ss;
			ss << int(1.0f/inputController.getDeltaTime()) << " fps";
			renderEngine.fpsFontPt->RenderText(renderEngine.textShader,ss.str(),screenWidth-150.0f,screenHeight-50.0f,1.0f,glm::vec3(0.0f, 1.0f, 0.0f),0);
			//std::cout << (1.0f/deltaTime) << "fps" << "\r";
		}

		// Overlay Help Menu
		//std::cout << toggleKeys[GLFW_KEY_H] << '\n';
		////if(toggleKeys[GLFW_KEY_H]) {
        if(inputController.getToggleKeyStateByIndex(GLFW_KEY_H)) {
			std::stringstream sh;
			// Note: Text lines rendered from bottom to top
			sh << "HELP MENU\n";
			sh << "Toggle Help:              h\n";
			sh << "Increment view:           v-b\n";
			sh << "Increment aircraft:       z-x\n";
			sh << "Increment track view:     n\n";
			sh << "Toggle Mouse Movement:  p\n";
			renderEngine.helpFont->RenderText(renderEngine.textShader,sh.str(),0.0f,0.05f,1.0f,glm::vec3(1.0f, 1.0f, 0.0f),1);
		}

		// Swap buffers
		glfwSwapBuffers(renderEngine.window);

		// Sleep to lower framerate
		//std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0/5.0)));

	}

	glfwTerminate();
	// Close mavlink socket
	/*for(unsigned int i=0; i<mavSocketList.size(); i++) {
		mavSocketList[i].closeSocket();
		threadList[i]->join();
	}

	// Stop Satellite Tile Threads
	satTileList.stopThreads();*/

	return 0;
}




