/*
 * light.h
 *
 *  Created on: 6Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef LIGHT_H_
#define LIGHT_H_

// GLEW (OpenGL Extension Wrangler Library)
#include <GL/glew.h>

// Project Includes
#include "renderEngine/shader.h"

// Standard Inclues
#include <vector>
using std::vector;


/* Classes */
class DirectionalLight {
public:
	vector<GLfloat> direction;
	vector<GLfloat> ambient;
	vector<GLfloat> diffuse;
	vector<GLfloat> specular;
	Shader* 		lightingShader;
	int				id;

	/* Constructor */
	DirectionalLight(vector<GLfloat> direction, vector<GLfloat> ambient, vector<GLfloat> diffuse, vector<GLfloat> specular, Shader* lightingShader, int id);

	/* Functions */
	void updateLightUniforms();
};

class PointLight {
public:
	vector<GLfloat> position;
	vector<GLfloat> ambient;
	vector<GLfloat> diffuse;
	vector<GLfloat> specular;
	GLfloat 		constant;
	GLfloat			linear;
	GLfloat			quadratic;
	Shader* 		lightingShader;
	int 			id;

	/* Constructor */
	PointLight (vector<GLfloat> position, vector<GLfloat> ambient, vector<GLfloat> diffuse, vector<GLfloat> specular, GLfloat constant, GLfloat linear, GLfloat quadratic, Shader* lightingShader,int id);

	/* Functions */
	void updateLightUniforms();
};

class SpotLight {
public:
	vector<GLfloat> position;
	vector<GLfloat> direction;
	vector<GLfloat> ambient;
	vector<GLfloat> diffuse;
	vector<GLfloat> specular;
	GLfloat 		constant;
	GLfloat			linear;
	GLfloat			quadratic;
	GLfloat			cutOff;
	GLfloat			outerCutoff;
	Shader* 		lightingShader;
	int 			id;

	/* Constructor */
	SpotLight (vector<GLfloat> position, vector<GLfloat> direction, vector<GLfloat> ambient, vector<GLfloat> diffuse, vector<GLfloat> specular, GLfloat constant, GLfloat linear, GLfloat quadratic, GLfloat cutOff, GLfloat outerCutoff, Shader* lightingShader,int id);

	/* Functions */
	void updateLightUniforms();

};


#endif /* LIGHT_H_ */
