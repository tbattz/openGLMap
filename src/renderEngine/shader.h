/*
 * shader.h
 *
 *  Created on: 27Dec.,2016
 *      Author: bcub3d-desktop
 */

#ifndef SHADER_H_
#define SHADER_H_

// GLAD - Multi Language GL Loader-Generator
#include <glad/glad.h>

// Standard Includes
#include <fstream>
#include <sstream>
#include <iostream>


class Shader {
public:
	// Program ID
	GLuint Program;

	/* Constructor to build the shader */
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	// Use the program
	void Use();

};

#endif /* SHADER_H_ */
