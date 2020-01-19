/*
 * fonts.cpp
 *
 *  Created on: 22Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "fonts.h"


GLFont::GLFont(const GLchar* fontPath) {
	/* Initialises a font. */

	// Enable Blending for transparent text background
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialise Font
	FT_Library ft;
	if(FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << '\n';
	}
	FT_Face face;
	if(FT_New_Face(ft,fontPath,0,&face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << '\n';
	}
	// Set Font Size
	FT_Set_Pixel_Sizes(face,0,48);

	// Load Glyphs
	glPixelStoref(GL_UNPACK_ALIGNMENT,1); // Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	// Load first 128 Glyphs
	for(GLubyte c = 0; c < 128; c++) {
		// Load Single glyph
		if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR:FREETYPE: Failed to load Glyph" << '\n';
			continue;
		}
		// Generate Texture
		GLuint texture;
		glGenTextures(1,&texture);
		glBindTexture(GL_TEXTURE_2D,texture);
		glTexImage2D(
				GL_TEXTURE_2D,0,GL_RED,face->glyph->bitmap.width,face->glyph->bitmap.rows,0,GL_RED,GL_UNSIGNED_BYTE,face->glyph->bitmap.buffer);
		// Set Texture Options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Store characters
		Character character = {texture,glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
								glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
								face->glyph->advance.x};
		this->Characters.insert(std::pair<GLchar, Character>(c,character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Clear Resources
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO and VBO for quads
	glGenVertexArrays(1,&this->VAO);
	glGenBuffers(1,&this->VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER,this->VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*6*4,NULL,GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof(GLfloat),0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
}

void GLFont::RenderText(Shader* shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, int align) {
	/* Renders text to the screen in a given location. */
	// align: 0 - Left Top Align
	// align: 1 - Left Bottom Align
	// align: 2	- Horizontal Centre Align
	// align: 3 - Vertical Centre Align
	// Activate correct shader
	shader->Use();
	glUniform3f(glGetUniformLocation(shader->Program, "textColor"),color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->VAO);

	// Iterate through each character
	if(align==0) {
		GLFont::drawLeftTopAligned(text, x, y, scale);
	} else if (align==1) {
		GLFont::drawLeftBottomAligned(text, x, y, scale);
	} else if (align==2) {
		GLFont::drawHCentreAligned(text, x, y, scale);
	} else if (align==3) {
		GLFont::drawVCentreAligned(text, x, y, scale);
	}

	// Unbind Arrays
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D,0);
}

void GLFont::drawLeftTopAligned(std::string text, GLfloat x, GLfloat y, GLfloat scale) {
	// Draw text as left top aligned
	std::string::const_iterator c;
	bool firstRow = false;
	float ystart = 0;
	float ymax = 0;
	float yoffset = 0;
	float xstart = 0;

	xstart = this->Characters[0].Bearing.x;

	// Get Offsets Height
	for(c=text.begin(); c != text.end(); c++) {
		Character ch = this->Characters[*c];
		if(*c=='\n') {
			// Find y starting position
			if(!firstRow) {
				firstRow = true;
				ystart = ymax;
				ymax = 0;
			}
			ymax = 0;
		} else {
			// Get maximum y
			GLfloat h = ch.Size.y * scale;
			if(h > ymax) {
				ymax = h;
			}
		}
	}

	// Reset ymax
	ymax = 0;

	// Draw glyphs
	for(c=text.begin(); c != text.end(); c++) {
		Character ch = this->Characters[*c];
		if(*c=='\n') {
			// Calculate offset
			yoffset += ymax*1.25;
			ymax = 0;
			x = xstart;
		} else {
			GLfloat xpos = x + ch.Bearing.x * scale;
			GLfloat ypos = y - ((ch.Size.y - ch.Bearing.y) * scale) - yoffset - ystart; // Accounts for g and p offsets

			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;

			if(h > ymax) {
				ymax = h;
			}

			// Update the VBO for each character
			GLfloat vertices[6][4] = {
					{xpos,		ypos + h,	0.0, 0.0},
					{xpos,		ypos,		0.0, 1.0},
					{xpos + w,	ypos,		1.0, 1.0},
					{xpos,		ypos + h,	0.0, 0.0},
					{xpos + w,	ypos,		1.0, 1.0},
					{xpos + w,	ypos + h,	1.0, 0.0},
			};
			// Render glyph texture over quad face
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update VBO memory
			glBindBuffer(GL_ARRAY_BUFFER,this->VBO);
			glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices,GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER,0);
			// Render quad face
			glDrawArrays(GL_TRIANGLES,0,6);
			// Advance pos to next glyph (advance numb is 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 6)
		}
	}
}

void GLFont::drawLeftBottomAligned(std::string text, GLfloat x, GLfloat y, GLfloat scale) {
	// Draw text as left bottom aligned
	std::string::const_iterator c;
	bool firstRow = false;
	float ystart = 0;
	float ymax = 0;
	float yoffset = 0;
	float xstart = 0;

	xstart = this->Characters[0].Bearing.x;

	// Get Offsets Height
	for(c=text.begin(); c != text.end(); c++) {
		Character ch = this->Characters[*c];
		if(*c=='\n') {
			// Find y starting position
			if(!firstRow) {
				firstRow = true;
			}
			ystart += ymax;
			ymax = 0;
		} else {
			// Get maximum y
			GLfloat h = ch.Size.y * scale;
			if(h > ymax) {
				ymax = h;
			}
		}
	}

	// Reset ymax
	ymax = 0;

	// Draw glyphs
	for(c=text.begin(); c != text.end(); c++) {
		Character ch = this->Characters[*c];
		if(*c=='\n') {
			// Calculate offset
			yoffset += ymax*1.25;
			ymax = 0;
			x = xstart;
		} else {
			GLfloat xpos = x + ch.Bearing.x * scale;
			GLfloat ypos = y - ((ch.Size.y - ch.Bearing.y) * scale) - yoffset + ystart; // Accounts for g and p offsets

			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;

			if(h > ymax) {
				ymax = h;
			}

			// Update the VBO for each character
			GLfloat vertices[6][4] = {
					{xpos,		ypos + h,	0.0, 0.0},
					{xpos,		ypos,		0.0, 1.0},
					{xpos + w,	ypos,		1.0, 1.0},
					{xpos,		ypos + h,	0.0, 0.0},
					{xpos + w,	ypos,		1.0, 1.0},
					{xpos + w,	ypos + h,	1.0, 0.0},
			};
			// Render glyph texture over quad face
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update VBO memory
			glBindBuffer(GL_ARRAY_BUFFER,this->VBO);
			glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices,GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER,0);
			// Render quad face
			glDrawArrays(GL_TRIANGLES,0,6);
			// Advance pos to next glyph (advance numb is 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 6)
		}
	}
}

void GLFont::drawHCentreAligned(std::string text, GLfloat x, GLfloat y, GLfloat scale) {
	// Draw text as horizontal centre aligned
	std::string::const_iterator c;
	bool firstRow = false;
	float ymax = 0;
	float ystart = 0;
	float yoffset = 0;
	float xoffset = 0;
	float currWidth = 0;
	std::vector<float> lineWidth;
	float xstart = 0;

	xstart = -this->Characters[0].Bearing.x;

	// Get Offsets Height
	for(c=text.begin(); c != text.end(); c++) {
		Character ch = this->Characters[*c];
		if(*c=='\n') {
			// Find y starting position
			if(!firstRow) {
				firstRow = true;
				ystart = ymax;
				ymax = 0;
				lineWidth.push_back(currWidth);
				currWidth = 0;
			}
			ymax = 0;
		} else {
			// Get maximum y
			GLfloat h = ch.Size.y * scale;
			if(h > ymax) {
				ymax = h;
			}
			// Increment width
			currWidth += (ch.Size.x + ch.Bearing.x) * scale;
		}
	}

	// Reset ymax
	ymax = 0;
	// Set starting x offset
	lineWidth.push_back(currWidth);
	xoffset = lineWidth[0]/2.0;

	// Draw glyphs
	int i=0;
	for(c=text.begin(); c != text.end(); c++) {
		Character ch = this->Characters[*c];
		if(*c=='\n') {
			// Increment row
			i += 1;
			xoffset = lineWidth[i]/2.0;
			// Calculate offset
			yoffset += ymax*1.25;
			ymax = 0;
			x = xstart;
		} else {
			GLfloat xpos = x - xoffset + (ch.Bearing.x * scale) + xstart;
			GLfloat ypos = y - ((ch.Size.y - ch.Bearing.y) * scale) - yoffset - ystart; // Accounts for g and p offsets

			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;

			if(h > ymax) {
				ymax = h;
			}

			// Update the VBO for each character
			GLfloat vertices[6][4] = {
					{xpos,		ypos + h,	0.0, 0.0},
					{xpos,		ypos,		0.0, 1.0},
					{xpos + w,	ypos,		1.0, 1.0},
					{xpos,		ypos + h,	0.0, 0.0},
					{xpos + w,	ypos,		1.0, 1.0},
					{xpos + w,	ypos + h,	1.0, 0.0},
			};
			// Render glyph texture over quad face
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update VBO memory
			glBindBuffer(GL_ARRAY_BUFFER,this->VBO);
			glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices,GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER,0);
			// Render quad face
			glDrawArrays(GL_TRIANGLES,0,6);
			// Advance pos to next glyph (advance numb is 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 6)
		}
	}
}

void GLFont::drawVCentreAligned(std::string text, GLfloat x, GLfloat y, GLfloat scale) {
	// Draw text as vertical centre aligned
	std::string::const_iterator c;
	bool firstRow = false;
	float ymax = 0;
	float ystart = 0;
	float yoffset = 0;
	float xoffset = 0;
	float currWidth = 0;
	std::vector<float> lineWidth;
	float xstart = 0;

	xstart = -this->Characters[0].Bearing.x;

	// Get Offsets Height
	for(c=text.begin(); c != text.end(); c++) {
		Character ch = this->Characters[*c];
		if(*c=='\n') {
			// Find y starting position
			if(!firstRow) {
				firstRow = true;
				ystart = ymax;
				ymax = 0;
				lineWidth.push_back(currWidth);
				currWidth = 0;
			}
			ymax = 0;
		} else {
			// Get maximum y
			GLfloat h = ch.Size.y * scale;
			if(h > ymax) {
				ymax = h;
			}
			// Increment width
			currWidth += (ch.Size.x + ch.Bearing.x) * scale;
		}
	}

	// Reset ymax
	ymax = 0;
	// Set starting x offset
	lineWidth.push_back(currWidth);
	xoffset = lineWidth[0]/2.0;

	// Draw glyphs
	int i=0;
	for(c=text.begin(); c != text.end(); c++) {
		Character ch = this->Characters[*c];
		if(*c=='\n') {
			// Increment row
			i += 1;
			xoffset = lineWidth[i]/2.0;
			// Calculate offset
			yoffset += ymax*1.25;
			ymax = 0;
			x = xstart;
		} else {
			GLfloat xpos = x - xoffset + (ch.Bearing.x * scale) + xstart;
			GLfloat ypos = y - ((ch.Size.y - ch.Bearing.y) * scale) - yoffset - ystart; // Accounts for g and p offsets

			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;

			// Adjust y position
			ypos -= h/2.0;

			if(h > ymax) {
				ymax = h;
			}

			// Update the VBO for each character
			GLfloat vertices[6][4] = {
					{xpos,		ypos + h,	0.0, 0.0},
					{xpos,		ypos,		0.0, 1.0},
					{xpos + w,	ypos,		1.0, 1.0},
					{xpos,		ypos + h,	0.0, 0.0},
					{xpos + w,	ypos,		1.0, 1.0},
					{xpos + w,	ypos + h,	1.0, 0.0},
			};
			// Render glyph texture over quad face
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update VBO memory
			glBindBuffer(GL_ARRAY_BUFFER,this->VBO);
			glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices,GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER,0);
			// Render quad face
			glDrawArrays(GL_TRIANGLES,0,6);
			// Advance pos to next glyph (advance numb is 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 6)
		}
	}
}


Shader* setupFontShader(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath, Settings* settings) {
	/* Compile and setup shader. */
	Shader* textShader = new Shader("../Shaders/font.vs","../Shaders/font.frag");
	textShader->Use();
	float screenWidth = settings->xRes;
	float screenHeight = settings->yRes;
	glm::mat4 textProjection = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);
	glUniformMatrix4fv(glGetUniformLocation(textShader->Program, "textProjection"), 1, GL_FALSE, glm::value_ptr(textProjection));


	return textShader;
}



