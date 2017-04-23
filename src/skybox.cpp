/*
 * skybox.cpp
 *
 *  Created on: 23Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "skybox.h"


/* Constructor */
Skybox::Skybox(vector<const GLchar*> faces) {
	// Create Buffers
	createAndSetupBuffers();

	// Setup Texture
	setupTexture(faces);

};

/* Functions */
void Skybox::Draw(Shader shader) {
	glDepthFunc(GL_LEQUAL); // Change depth function so depth test passes when values are equal to depth buffer's content
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // Set depth function back to default
}

void Skybox::createAndSetupBuffers() {
	/* Create Buffers */
	glGenVertexArrays(1,&skyboxVAO);
	glGenBuffers(1,&skyboxVBO);

	/* Setup Buffers */
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);

	/* Position Attributes */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(GLfloat),(GLvoid*)0);

	/* Unbind VAO */
	glBindVertexArray(0);
}

void Skybox::setupTexture(vector<const GLchar*> faces) {
	/* Setup Faces */
	cubemapTexture = loadCubemap(faces);

}

GLuint Skybox::loadCubemap(vector<const GLchar*> faces) {
	// Loads a cubemap texture using 6 texture faces
	// +X: Right, 	-X: Left
	// +Y: Top, 	-Y: Bottom
	// +Z: Front,	-Z: Back

	// Generate Texture
	GLuint textureID;
	glGenTextures(1,&textureID);
	unsigned char* image;

	// Load Texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for(GLuint i=0; i<faces.size(); i++) {
		image = SOIL_load_image(faces[i],&width,&height,0,SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP,0);

	return textureID;
}
