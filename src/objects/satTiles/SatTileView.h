//
// Created by bcub3d-desktop on 26/1/20.
//

#ifndef OPENGLMAP_SATTILEVIEW_H
#define OPENGLMAP_SATTILEVIEW_H

// Standard Includes
#include <vector>

// Object Loading Includes
#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Project Includes
#include <renderEngine/shader.h>


enum ImageType {PNG, JPEG};

class SatTileView {
public:
    /* Constructor */
    SatTileView();

    /* Functions */
    void setupTile(std::vector<std::vector<double>> xyOffsets, std::string filename);
    void Draw(Shader shader);

private:
    /* Data */
    // Mesh Data
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    // Buffers
    GLuint VAO, VBO, EBO;
    // Textures
    GLuint tileTexture;
    int width, height;
    std::string filename;

    /* Functions */
    void calculateVerticesIndices(std::vector<std::vector<double>> xyOffsets, ImageType imageType);
    void createAndSetupBuffers();
    void setupTexture();
};


#endif //OPENGLMAP_SATTILEVIEW_H
