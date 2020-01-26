/*
 * mesh.cpp
 *
 *  Created on: 23Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "mesh.h"

/* Constructor */
Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	// Set vertex buffers and attribute pointers
	setupMesh();
}

void Mesh::Draw(Shader shader) {
	// Bind structures
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	for(GLuint i = 0; i< this->textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); // Switch to the correct texture unit
		// Get the texture number
		std::stringstream ss;
		string number;
		string name = this->textures[i].type;
		if(name == "texture_diffuse") {
			ss << diffuseNr++; // Transfer GLuint to stream
		} else if (name == "texture_specular") {
			ss << specularNr++; // Transfer GLuint to stream
		}
		number = ss.str();
		// Set sampler to correct texture unit
		glUniform1i(glGetUniformLocation(shader.Program,(name+number).c_str()),i);
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D,this->textures[i].id);
	}

	// Set each mesh's shininess property to the default
	glUniform1f(glGetUniformLocation(shader.Program,"material.shininess"),16.0f);

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES,this->indices.size(),GL_UNSIGNED_INT,0);
	// Unbind VAO
	glBindVertexArray(0);

	// Set back to defaults
	for(GLuint i = 0; i< this->textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D,0);
	}
}

void Mesh::setupMesh() {
	// Create buffer/arrays
	glGenVertexArrays(1,&this->VAO);
	glGenBuffers(1,&this->VBO);
	glGenBuffers(1,&this->EBO);
	glBindVertexArray(this->VAO);

	// Load Data into vertex Buffers
	glBindBuffer(GL_ARRAY_BUFFER,this->VBO);

	// Structs have sequential memory, so can just pass pointers to the struct
	glBufferData(GL_ARRAY_BUFFER,this->vertices.size()*sizeof(Vertex),&this->vertices[0],GL_STATIC_DRAW);

	// Put data into Element Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,this->indices.size()*sizeof(GLuint),&this->indices[0],GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,TexCoords));

	glBindVertexArray(0);
}
