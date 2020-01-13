//
// Created by bcub3d-desktop on 13/1/20.
//

#ifndef OPENGLMAP_SIXDOFMODEL_H
#define OPENGLMAP_SIXDOFMODEL_H

// GL Includes
#include <GL/glew.h>

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// Standard Includes
#include <mutex>

// GLM Mathematics
#include <glm/gtc/type_ptr.hpp>

// Project Includes
#include "model.h"


// Derived Class
class SixDofModel : public Model {
private:
    // Identifier
    int id;
public:
    /* Data */
    // Position Information
    glm::dvec3          position;                       // (x,y,z) relative to origin

    // Attitude Information
    glm::dvec3          attitude;                       // roll (rad), pitch (rad), yaw (rad)

    /* Constructor */
    SixDofModel(const GLchar* path);

    /* Functions */
    void setId(int id);
    int getId();
    void Draw(Shader shader);

};

#endif //OPENGLMAP_SIXDOFMODEL_H
