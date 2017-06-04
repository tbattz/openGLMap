/*
 * volumes.h
 *
 *  Created on: 1Jun.,2017
 *      Author: bcub3d-desktop
 */

#ifndef VOLUMES_H_
#define VOLUMES_H_

// Standard Includes
#include <string>
#include <vector>
#include <algorithm>

// GL Includes
#include <GL/glew.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Project Includes
#include "settings.h"
#include "shader.h"

/* Classes */
class Volume {
public:
	/* Data */
	glm::dvec3 						 origin;
	glm::dvec3						 ecefOrigin;
	std::string 					 name;
	std::vector<int> 				 rgb;
	float 							 alpha;
	std::vector<std::vector<double>> pts;
	std::vector<GLfloat> 			 vertices;
	std::vector<GLuint>				 indices;
	// Buffers
	GLuint VAO, VBO, EBO;

	/* Constructor */
	Volume(glm::vec3 origin, volumeDef volDef);

	/* Functions */
	void Draw(Shader shader);
	void createTriangles();
	std::vector<std::vector<int>> triangulatePolygon();
	bool linesCross(std::vector<double> pt1, std::vector<double> pt2, std::vector<double> pt3, std::vector<double> pt4);

private:
	/* Functions */
	void createAndSetupBuffers();


};



#endif /* VOLUMES_H_ */
