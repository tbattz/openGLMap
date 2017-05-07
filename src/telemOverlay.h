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
#include "camera.h"


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
	MavAircraft* mavAircraftPt;
	glm::vec3 aircraftPosition;
	glm::vec3 ndc;
	float scale;

	// Font Information
	Shader* telemTextShaderPt;
	GLFont* telemFontPt;
	GLfloat width;
	GLfloat height;

	// Color
	glm::vec3 color;

	/* Constructor */
	TelemOverlay(MavAircraft* mavAircraftPt,Shader* telemTextShaderPt,GLFont* telemFontPt, glm::vec3 color, Settings* settings);

	/* Functions */
	void Draw(Shader shader, glm::mat4 projection, glm::mat4 view, Camera* cameraPt);
	void DrawAirspeed();
	glm::vec2 convertNDC2Screen();
	void createAndSetupBuffers();


};



#endif /* TELEMOVERLAY_H_ */
