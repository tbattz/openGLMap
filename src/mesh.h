 /*
 * mesh.h
 *
 *  Created on: 1Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef MESH_H_
#define MESH_H_

// Standard Includes
#include <iostream>
#include <vector>
using std::string;
using std::vector;

// GL Includes
#include <GL/glew.h>

// Assimp Includes
#include <assimp/postprocess.h>

// GLM Includes
#include <glm/gtc/matrix_transform.hpp>

// Project Includes
#include "shader.h"


struct Vertex {
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// TexCoords
	glm::vec2 TexCoords;
};

struct Texture {
	GLuint id;
	string type;
	aiString path;
};

/* Classes */
class Mesh {
public:
	/* Mesh Data */
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;


	/* Constructor */
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);

	/* Functions */
	void Draw(Shader shader);

private:
	/* Render Data */
	GLuint VAO, VBO, EBO;

	/* Functions */
	void setupMesh();

};

#endif /* MESH_H_ */
