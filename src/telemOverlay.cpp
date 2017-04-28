/*
 * telemOverlay.cpp
 *
 *  Created on: 23Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "telemOverlay.h"

/* Constructor */
TelemOverlay::TelemOverlay(MavAircraft* mavAircraftPt,Shader* telemTextShaderPt,GLFont* telemFontPt, Settings* settings) {
	// Aircraft Information
	this->mavAircraftPt = mavAircraftPt;
	this->aircraftPosition = glm::vec3(mavAircraftPt->position);

	// Font Information
	this->telemTextShaderPt = telemTextShaderPt;
	this->telemFontPt = telemFontPt;
	this->width = settings->xRes;
	this->height = settings->yRes;

	// Setup Buffers
	createAndSetupBuffers();
}

/* Functions */
void TelemOverlay::Draw(Shader shader, glm::mat4 projection, glm::mat4 view, Camera* cameraPt) {
	// Convert position from double to float
	aircraftPosition = glm::vec3(mavAircraftPt->position);

	// Calculate Normalised Device Coordinates
	glm::mat4 model;
	model = glm::translate(model,glm::vec3(mavAircraftPt->position[0],mavAircraftPt->position[2],mavAircraftPt->position[1]));// Translate first due to GLM ordering, rotations opposite order
	glm::vec4 clip = (projection * view * glm::vec4(mavAircraftPt->position[0],mavAircraftPt->position[2],mavAircraftPt->position[1],1.0f));
	ndc = glm::vec3(clip[0]/clip[3],clip[1]/clip[3],clip[2]/clip[3]);

	// Calculate Scale
	glm::vec3 diff = (glm::vec3)mavAircraftPt->position - cameraPt->Position;
	float dist = sqrt(dot(diff,diff));
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

void TelemOverlay::DrawAirspeed() {
	// Find position on screen
	glm::vec2 pos = convertNDC2Screen();

	// Draw Airspeed
	std::stringstream ss;
	ss << std::setprecision(3) << mavAircraftPt->airspeed << " m/s";
	telemFontPt->RenderText(telemTextShaderPt,ss.str(),pos[0],pos[1],1.0f,glm::vec3(0.0f, 1.0f, 0.0f),0);
}

glm::vec2 TelemOverlay::convertNDC2Screen() {
	// Calculate Position on screen
	float x,y;
	x = (width/2.0)*(ndc[0]-1) + width;
	y = (height/2.0)*(ndc[1]-1) + height;

	// Add offset
	y -= 30;
	x -= 25;

	return glm::vec2(x,y);
}

void TelemOverlay::createAndSetupBuffers() {
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
