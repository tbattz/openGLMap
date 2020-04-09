/*
 * loadingScreen.h
 *
 *  Created on: 2Apr.,2017
 *      Author: bcub3d-desktop
 */

#ifndef LOADINGSCREEN_H_
#define LOADINGSCREEN_H_

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// Standard Includes
#include <vector>
#include <iostream>
#include <string>

// Define Fonts based on OS
#ifdef _WIN32
	#define FONTPATH "C:/Windows/Fonts/Arial.ttf"
#elif __linux__
	#define FONTPATH "/usr/share/fonts/truetype/abyssinica/AbyssinicaSIL-R.ttf"
#endif


// openGL Includes
#include "utilities/settings.h"
#include "renderEngine/fonts.h"

/* Classes */
class LoadingScreen {
public:
	// Define version number
	std::string OPENGLMAP_VERSION = "0.1";

	/* Data */
	GLFWwindow* window;
	int* screenWidthPt;
	int* screenHeightPt;
	std::vector<std::string> messages;
	std::string headMessage = "Loading openGLMap Version " + OPENGLMAP_VERSION + "\n";
	GLFont textFont;
	Shader* textShader;


	/* Constructor */
	LoadingScreen(GLFWwindow* window, Settings* settings);

	/* Functions */
	void drawColourScreen();
	void drawMessages();
	void appendLoadingMessage(std::string newMessage);

};


#endif /* LOADINGSCREEN_H_ */
