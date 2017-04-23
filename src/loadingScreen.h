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
#include "../src/fonts.h"

class LoadingScreen {
public:
	// Define version number
	string OPENGLMAP_VERSION = "0.1";

	/* Data */
	GLFWwindow* window;
	float* screenWidthPt;
	float* screenHeightPt;
	std::vector<string> messages;
	string headMessage = "Loading openGLMap Version " + OPENGLMAP_VERSION + "\n";
	GLFont textFont;
	Shader textShader = Shader("../Shaders/font.vs", "../Shaders/font.frag");


	/* Functions */
	// Initialiser
	LoadingScreen(GLFWwindow* window, float* screenWidthPt, float* screenHeightPt) : textFont(FONTPATH) {
		this->window = window;
		this->screenWidthPt = screenWidthPt;
		this->screenHeightPt = screenHeightPt;

		// Setup Font Shader
		this->textShader = setupFontShader("../Shaders/font.vs", "../Shaders/font.frag", *screenWidthPt, *screenHeightPt);

		// Draw Blank Screen
		drawColourScreen();

		// Draw Mesages
		drawMessages();

	}

	void drawColourScreen() {
		// Check Events
		glfwPollEvents();
		// Clear the colour buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void drawMessages() {
		// Redraw Blank Screen
		drawColourScreen();

		// Draws the head message and then the messages, the latter in reverse order
		textFont.RenderText(&textShader,headMessage,0.0f,*screenHeightPt,1.0f,glm::vec3(0.0f, 1.0f, 0.0f),0);

		// Print Messages in reverse order
		string tempString = "";
		for(int i = messages.size()-1; i > -1; i--) {
			tempString += messages[i] + "\n";
		}
		textFont.RenderText(&textShader,tempString,0.0f,*screenHeightPt-47,0.5f,glm::vec3(1.0f, 1.0f, 1.0f),0);

		// Swap buffers
		glfwSwapBuffers(window);
	}

	void appendLoadingMessage(string newMessage) {
		// Appends load screen message and redraws loading screen
		messages.push_back(newMessage);
		drawMessages();
	}
};



#endif /* LOADINGSCREEN_H_ */
