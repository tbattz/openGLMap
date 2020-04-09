/*
 * shader.cpp
 *
 *  Created on: 23Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "shader.h"


/* Constructor */
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	// Get vertex/fragment source from file path
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// Handle file exceptions
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
	try {
		// Open Files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read buffer into stream
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// Close Files
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream to GLchar array
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	} catch(std::ifstream::failure & e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << '\n';
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	/* Compile Shaders */
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex,1,&vShaderCode,NULL);
	glCompileShader(vertex);
	// Check for errors
	glGetShaderiv(vertex,GL_COMPILE_STATUS,&success);
	if(!success) {
		glGetShaderInfoLog(vertex,512,NULL,infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << '\n';
	}

	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment,1,&fShaderCode,NULL);
	glCompileShader(fragment);
	// Check for errors
	glGetShaderiv(fragment,GL_COMPILE_STATUS,&success);
	if(!success) {
		glGetShaderInfoLog(fragment,512,NULL,infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << '\n';
	}

	// Shader Program
	this->Program = glCreateProgram();
	glAttachShader(this->Program,vertex);
	glAttachShader(this->Program,fragment);
	glLinkProgram(this->Program);
	// Check for linking errors
	glGetProgramiv(this->Program,GL_LINK_STATUS,&success);
	if(!success) {
		glGetProgramInfoLog(this->Program,512,NULL,infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << '\n';
	}

	// Delete Shaders (no longer needed)
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

/* Functions */
void Shader::Use() {
	glUseProgram(this->Program);
}
