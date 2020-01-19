/*
 * loadingScreen.cpp
 *
 *  Created on: 28Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "loadingScreen.h"


/* Constructor */
LoadingScreen::LoadingScreen(GLFWwindow* window, Settings* settings)  : textFont(FONTPATH) {
	this->window = window;
	this->screenWidthPt = &(settings->xRes);
	this->screenHeightPt = &(settings->yRes);

	// Setup Font Shader
	this->textShader = setupFontShader("../Shaders/font.vs", "../Shaders/font.frag", settings);

	// Draw Blank Screen
	drawColourScreen();

	// Draw Mesages
	drawMessages();
}

/* Functions */
void LoadingScreen::drawColourScreen() {
	// Check Events
	glfwPollEvents();
	// Clear the colour buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LoadingScreen::drawMessages() {
	// Redraw Blank Screen
	drawColourScreen();

	// Draws the head message and then the messages, the latter in reverse order
	textFont.RenderText(textShader,headMessage,0.0f,*screenHeightPt,1.0f,glm::vec3(0.0f, 1.0f, 0.0f),0);

	// Print Messages in reverse order
	string tempString = "";
	for(int i = messages.size()-1; i > -1; i--) {
		tempString += messages[i] + "\n";
	}
	textFont.RenderText(textShader,tempString,0.0f,*screenHeightPt-47,0.5f,glm::vec3(1.0f, 1.0f, 1.0f),0);

	// Swap buffers
	glfwSwapBuffers(window);
}

void LoadingScreen::appendLoadingMessage(string newMessage) {
	// Appends load screen message and redraws loading screen
	messages.push_back(newMessage);
	drawMessages();
}


