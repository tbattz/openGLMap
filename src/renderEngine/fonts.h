/*
 * fonts.h
 *
 *  Created on: 5Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef FONTS_H_
#define FONTS_H_

// Free Type Library
#include <ft2build.h>
#include FT_FREETYPE_H

// GLM Mathematics
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Project Includes
#include "shader.h"
#include "settings.h"

// Standard Includes
#include <vector>
#include <map>


// Character Structure
struct Character {
	GLuint		TextureID;	// ID handle of the glyph texture
	glm::ivec2	Size;		// size of glyph
	glm::ivec2	Bearing;	// Offset from baseline to left/top of glyph
	GLuint		Advance;	// Offset to advance to next glyph
};


/* Functions */
Shader* setupFontShader(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath, Settings* settings);

/* Classes */
class GLFont {
public:
	std::map<GLchar, Character> Characters;
	GLuint VAO, VBO;

	/* Constructor */
	GLFont(const GLchar* fontPath);

	/* Rendering */
	void RenderText(Shader* shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, int align);

	/* Alignment */
	void drawLeftTopAligned(std::string text, GLfloat x, GLfloat y, GLfloat scale);
	void drawLeftBottomAligned(std::string text, GLfloat x, GLfloat y, GLfloat scale);
	void drawHCentreAligned(std::string text, GLfloat x, GLfloat y, GLfloat scale);
	void drawVCentreAligned(std::string text, GLfloat x, GLfloat y, GLfloat scale);
};



#endif /* FONTS_H_ */
