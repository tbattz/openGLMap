//
// Created by bcub3d-desktop on 14/3/20.
//

#ifndef OPENGLMAP_AXESVIEW_H
#define OPENGLMAP_AXESVIEW_H

// Standard Includes
#include <vector>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Project Includes
#include <renderEngine/shader.h>


class AxesView {
public:
    /* Constructor */
    AxesView();

    /* Functions */
    void Draw(Shader shader);

private:
    /* Data */
    // Axes Data
    float len = 100.0;
    std::vector<GLfloat> pts; // [x0, y0, z0, x1, y1, z1, ...]
    // Buffers
    GLuint VAO, VBO;
    // Line Colors
    std::vector<glm::vec4> colors = {glm::vec4(1.0, 0.0, 0.0, 1.0), // Red - x axes
                                     glm::vec4(0.0, 1.0, 0.0, 1.0), // Green - y axes
                                     glm::vec4(0.0, 0.0, 1.0, 1.0)}; // Blue - z axes

    /* Functions */
    void calculatePts();
    void createAndSetupBuffers();

};


#endif //OPENGLMAP_AXESVIEW_H
