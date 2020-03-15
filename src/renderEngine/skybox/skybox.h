/*
 * cubemap.h
 *
 *  Created on: 26Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef SKYBOX_H_
#define SKYBOX_H_

// SOIL Includes
#include <SOIL.h>

// Project Includes
#include "renderEngine/shader.h"

// Standard Includes
#include <vector>
using std::vector;


class Skybox {
public:
	// Buffers
	GLuint skyboxVAO;
	GLuint skyboxVBO;

	// Texture
	GLuint cubemapTexture;
	int width, height;

	// Set skybox vertices
	GLfloat skyboxVertices[108] = {
	        // Positions
	        -1.0f,  1.0f, -1.0f,
	        -1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,
	         1.0f,  1.0f, -1.0f,
	        -1.0f,  1.0f, -1.0f,

	        -1.0f, -1.0f,  1.0f,
	        -1.0f, -1.0f, -1.0f,
	        -1.0f,  1.0f, -1.0f,
	        -1.0f,  1.0f, -1.0f,
	        -1.0f,  1.0f,  1.0f,
	        -1.0f, -1.0f,  1.0f,

	         1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,

	        -1.0f, -1.0f,  1.0f,
	        -1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f, -1.0f,  1.0f,
	        -1.0f, -1.0f,  1.0f,

	        -1.0f,  1.0f, -1.0f,
	         1.0f,  1.0f, -1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	        -1.0f,  1.0f,  1.0f,
	        -1.0f,  1.0f, -1.0f,

	        -1.0f, -1.0f, -1.0f,
	        -1.0f, -1.0f,  1.0f,
	         1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,
	        -1.0f, -1.0f,  1.0f,
	         1.0f, -1.0f,  1.0f
	    };


	/* Constructor */
	Skybox(vector<const GLchar*> faces);

	/* Functions */
	void Draw(Shader shader);

private:
	/* Functions */
	void createAndSetupBuffers();
	void setupTexture(vector<const GLchar*> faces);
	GLuint loadCubemap(vector<const GLchar*> faces);
};



#endif /* SKYBOX_H_ */
