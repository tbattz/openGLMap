//
// Created by bcub3d-desktop on 13/3/20.
//

#ifndef OPENGLMAP_SIMPLESQUAREVIEW_H
#define OPENGLMAP_SIMPLESQUAREVIEW_H


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



class SimpleSquareView {
public:
    /* Constructor */
    SimpleSquareView(std::string filename);

    /* Functions */
    void setupTile(std::string filename);
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
    void calculateVerticesIndices();
    void createAndSetupBuffers();
    void setupTexture();
};


#endif //OPENGLMAP_SIMPLESQUAREVIEW_H
