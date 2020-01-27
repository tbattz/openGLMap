//
// Created by bcub3d-desktop on 26/1/20.
//

#include "SatTileView.h"


/* Constructor */
SatTileView::SatTileView() {

}

/* Functions */
void SatTileView::setupTile(std::vector<std::vector<double>> xyOffsets, std::string filename) {
    /* Store filename */
    this->filename = filename;
    /* Calculate vertices and indices */
    calculateVerticesIndices(xyOffsets);
    /* Create and Setup Buffers */
    createAndSetupBuffers();
    /* Load Texture */
    setupTexture();
}

void SatTileView::calculateVerticesIndices(std::vector<std::vector<double>> xyOffsets) {
    /* Calculate Vertices */
    // Tiles geo position is top left corner
    // Image Texture coords start in bottom left, then rotated by 90 deg (for png)
    vertices = {
            // Positions								// Normals			// Texture Coords
            -xyOffsets[1][0],	0.0f, xyOffsets[1][1],			0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
            -xyOffsets[2][0],	0.0f, xyOffsets[2][1],			0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
            -xyOffsets[0][0],	0.0f, xyOffsets[0][1],			0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
            0.0f,			0.0f, 0.0f,					0.0f, 0.0f, 1.0f,	0.0f, 0.0f
    };

    /* Store Indices */
    this->indices = { // Indices start from zero
            0, 1, 3, // First triangle
            1, 2, 3  // Second triangle
    };
}

void SatTileView::createAndSetupBuffers() {
    /* Create Buffers */
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    /* Setup Buffers */
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat),&vertices[0],GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(GLfloat),&indices[0],GL_STATIC_DRAW);

    /* Position Attributes */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(GLfloat),(GLvoid*)0);
    /* Normal Attributes */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(GLfloat),(GLvoid*)(3*sizeof(GLfloat)));
    /* TexCoord Attributes */
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(GLfloat),(GLvoid*)(6*sizeof(GLfloat)));

    glBindVertexArray(0); // Unbind VAO
}

void SatTileView::setupTexture() {
    // Create Texture
    glGenTextures(1,&tileTexture);
    glBindTexture(GL_TEXTURE_2D,tileTexture);
    // Set parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    // Load texture
    unsigned char* image = SOIL_load_image(filename.c_str(),&width,&height,0,SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D,0);
}

void SatTileView::Draw(Shader shader) {
    // Bind Texture Units
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader.Program,"tileTexture"),0);
    glBindTexture(GL_TEXTURE_2D,tileTexture);

    // Draw tile
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);
    glBindVertexArray(0);

    // Set back to defaults
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,0);
}


