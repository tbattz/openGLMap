/*
 * loadingScreen.h
 *
 *  Created on: 2Apr.,2017
 *      Author: bcub3d-desktop
 */

#ifndef LOADINGSCREEN_H_
#define LOADINGSCREEN_H_

// GLEW (OpenGL Extension Wrangler Library)
#define GLEW_STATIC
#include <GL/glew.h>

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
	string OPENGLMAP_VERSION = "0.1";

	/* Data */
	GLFWwindow* window;
	int* screenWidthPt;
	int* screenHeightPt;
	std::vector<string> messages;
	string headMessage = "Loading openGLMap Version " + OPENGLMAP_VERSION + "\n";
	GLFont textFont;
	Shader* textShader;


	/* Constructor */
	LoadingScreen(GLFWwindow* window, Settings* settings);

	/* Functions */
	void drawColourScreen();
	void drawMessages();
	void appendLoadingMessage(string newMessage);

};


#endif /* LOADINGSCREEN_H_ */
