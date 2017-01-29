/*
 * telemOverlay.h
 *
 *  Created on: 26Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef TELEMOVERLAY_H_
#define TELEMOVERLAY_H_

// GL Includes
#include <GL/glew.h>

// openGL Includes
#include "../src/mavAircraft.h"



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

	TelemOverlay(MavAircraft* mavAircraftPt,Shader* telemTextShaderPt,GLFont* telemFontPt, GLfloat screenWidth, GLfloat screenHeight) {
		// Aircraft Information
		this->mavAircraftPt = mavAircraftPt;
		this->aircraftPosition = glm::vec3(mavAircraftPt->position);

		// Font Information
		this->telemTextShaderPt = telemTextShaderPt;
		this->telemFontPt = telemFontPt;
		this->width = screenWidth;
		this->height = screenHeight;

		// Setup Buffers
		createAndSetupBuffers();


	}

	void Draw(Shader shader, glm::mat4 projection, glm::mat4 view, Camera* cameraPt) {
		// Convert position from double to float
		aircraftPosition = glm::vec3(mavAircraftPt->position);

		// Calculate Normalised Device Coordinates
		glm::mat4 model;
		model = glm::translate(model,glm::vec3(mavAircraftPt->position[0],mavAircraftPt->position[2],mavAircraftPt->position[1]));// Translate first due to GLM ordering, rotations opposite order
		glm::vec4 clip = (projection * view * glm::vec4(mavAircraftPt->position[0],mavAircraftPt->position[2],mavAircraftPt->position[1],1.0f));
		ndc = glm::vec3(clip[0]/clip[3],clip[1]/clip[3],clip[2]/clip[3]);

		// Calculate Scale
		glm::vec3 diff = (glm::vec3)mavAircraftPt->position - cameraPt->Position;
		float dist = glm::sqrt(dot(diff,diff));
		scale = ((0.3-1)/1000)*dist + 1;
		if(scale<0.3) {
			scale = 0.3;
		}

		// Update Uniforms
		float scale = 0.3;
		glUniform3fv(glGetUniformLocation(shader.Program,"ndc"),1,glm::value_ptr(ndc));
		glUniform1f(glGetUniformLocation(shader.Program,"scale"),scale);

		// Draw Overlay
		if(ndc[2]<1) {
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

	void DrawAirspeed() {
		// Find position on screen
		glm::vec2 pos = convertNDC2Screen();

		// Draw Airspeed
		std::stringstream ss;
		ss << std::setprecision(3) << mavAircraftPt->airspeed << " m/s";
		telemFontPt->RenderText(telemTextShaderPt,ss.str(),pos[0],pos[1],1.0f,glm::vec3(0.0f, 1.0f, 0.0f),0);
	}

	glm::vec2 convertNDC2Screen() {
		// Calculate Position on screen
		float x,y;
		x = (width/2.0)*(ndc[0]-1) + width;
		y = (height/2.0)*(ndc[1]-1) + height;

		// Add offset
		y -= 30;
		x -= 25;

		return glm::vec2(x,y);
	}

	void createAndSetupBuffers() {
		/* Create Buffers */
		glGenVertexArrays(1,&VAO);
		glGenBuffers(1,&VBO);
		glGenBuffers(1,&EBO);

		/* Setup Buffers */
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),&vertices[0],GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),&indices[0],GL_STATIC_DRAW);

		/* Position Attributes */
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(GLfloat),(GLvoid*)0);


		glBindVertexArray(0); // Unbind VAO
	}

};



#endif /* TELEMOVERLAY_H_ */
