/*
 * light.cpp
 *
 *  Created on: 22Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "light.h"


/* Functions */
/* Direction Light */
DirectionalLight::DirectionalLight(std::vector<GLfloat> direction, std::vector<GLfloat> ambient, std::vector<GLfloat> diffuse, std::vector<GLfloat> specular, Shader* lightingShader, int id) {
	/* Contains the information required for a directional light. */
	this->direction = direction;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->lightingShader = lightingShader;
	this->id = id;

	// Set Uniforms
	this->updateLightUniforms();
};

void DirectionalLight::updateLightUniforms() {
	this->lightingShader->Use();
	std::stringstream ss;
	std::string startString = "dirLights[";
	ss << startString << this->id << "].";
	glUniform1i(glGetUniformLocation(this->lightingShader->Program, "material.diffuse"),  0);
	glUniform1i(glGetUniformLocation(this->lightingShader->Program, "material.specular"), 1);
	glUniform1f(glGetUniformLocation(this->lightingShader->Program,"material.shininess"),32.0f);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "direction").c_str()),	this->direction[0],	this->direction[1],	this->direction[2]);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "ambient").c_str()),	this->ambient[0],	this->ambient[1],	this->ambient[2]);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "diffuse").c_str()),	this->diffuse[0],	this->diffuse[1],	this->diffuse[2]);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "specular").c_str()),	this->specular[0],	this->specular[1],	this->specular[2]);
};

/* Point Light */
PointLight::PointLight (std::vector<GLfloat> position, std::vector<GLfloat> ambient, std::vector<GLfloat> diffuse, std::vector<GLfloat> specular, GLfloat constant, GLfloat linear, GLfloat quadratic, Shader* lightingShader,int id) {
	/* Contains the information required for a point light. */
	this->position = position;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
	this->lightingShader = lightingShader;
	this->id = id;

	// Set Uniforms
	updateLightUniforms();
};

void PointLight::updateLightUniforms() {
	lightingShader->Use();
	std::stringstream ss;
	std::string startString = "pointLights[";
	ss << startString << this->id << "].";
	glUniform1i(glGetUniformLocation(this->lightingShader->Program, "material.diffuse"),  0);
	glUniform1i(glGetUniformLocation(this->lightingShader->Program, "material.specular"), 1);
	glUniform1f(glGetUniformLocation(this->lightingShader->Program,"material.shininess"),32.0f);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "position").c_str()),	this->position[0],	this->position[1],	this->position[2]);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "ambient").c_str()),	this->ambient[0],	this->ambient[1],	this->ambient[2]);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "diffuse").c_str()),	this->diffuse[0],	this->diffuse[1],	this->diffuse[2]);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "specular").c_str()),	this->specular[0],	this->specular[1],	this->specular[2]);
	glUniform1f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "constant").c_str()),	this->constant);
	glUniform1f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "linear").c_str()),	this->linear);
	glUniform1f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "quadratic").c_str()),this->quadratic);
}



SpotLight::SpotLight (std::vector<GLfloat> position, std::vector<GLfloat> direction, std::vector<GLfloat> ambient, std::vector<GLfloat> diffuse, std::vector<GLfloat> specular, GLfloat constant, GLfloat linear, GLfloat quadratic, GLfloat cutOff, GLfloat outerCutoff, Shader* lightingShader,int id) {
	/* Contains the information required for a spot light. */
	this->position = position;
	this->direction = direction;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
	this->cutOff = cutOff;
	this->outerCutoff = outerCutoff;
	this->lightingShader = lightingShader;
	this->id = id;

	// Set Uniforms
	updateLightUniforms();
};

void SpotLight::updateLightUniforms() {
	lightingShader->Use();
	std::stringstream ss;
	std::string startString = "spotLights[";
	ss << startString << this->id << "].";
	glUniform1i(glGetUniformLocation(this->lightingShader->Program, "material.diffuse"),  0);
	glUniform1i(glGetUniformLocation(this->lightingShader->Program, "material.specular"), 1);
	glUniform1f(glGetUniformLocation(this->lightingShader->Program,"material.shininess"),32.0f);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "position").c_str()),	this->position[0],	this->position[1],	this->position[2]);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "direction").c_str()),	this->direction[0],	this->direction[1],	this->direction[2]);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "ambient").c_str()),	this->ambient[0],	this->ambient[1],	this->ambient[2]);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "diffuse").c_str()),	this->diffuse[0],	this->diffuse[1],	this->diffuse[2]);
	glUniform3f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "specular").c_str()),	this->specular[0],	this->specular[1],	this->specular[2]);
	glUniform1f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "constant").c_str()),	this->constant);
	glUniform1f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "linear").c_str()),	this->linear);
	glUniform1f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "quadratic").c_str()),this->quadratic);
	glUniform1f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "cutOff").c_str()),this->cutOff);
	glUniform1f(glGetUniformLocation(this->lightingShader->Program, (ss.str() + "outerCutoff").c_str()),this->outerCutoff);
}



