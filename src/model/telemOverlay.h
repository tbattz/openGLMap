/*
 * telemOverlay.h
 *
 *  Created on: 26Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef TELEMOVERLAY_H_
#define TELEMOVERLAY_H_

// Standard Includes
#include "iomanip"

// Project Includes
#include "view/camera.h"


class TelemOverlay {
public:
	// Shape
    GLfloat vertices[12] = {
         0.1f,	-0.1f, 	0.0f,	// Top Right
		 0.1f,	-0.11f,  0.0f,	// Bottom Right
		-0.1f,	-0.11f,	0.0f,	// Bottom Left
		-0.1f,	-0.1f,  0.0f	// Top Left
    };
	GLuint indices[6] = { //Index starts at 0
			0, 1, 2,  // First Triangle
			2, 3, 0,  // Second Triangle
		};

	// Buffers
	GLuint VBO, VAO, EBO;

	// Aircraft Information
	glm::vec3 ndc;
	float scale;

	// Settings
	Settings* settings;

	// Font Information
	Shader* telemTextShaderPt;
	GLFont* telemFontPt;
	GLfloat width;
	GLfloat height;

	// Color
	int colorId;

	/* Constructor */
	TelemOverlay(int colorId, Settings* settings);

	/* Functions */
	void Draw(Shader shader, MavAircraft* mavAircraftPt, glm::mat4 projection, glm::mat4 view, Camera* cameraPt);
	void createAndSetupBuffers();


};



#endif /* TELEMOVERLAY_H_ */
