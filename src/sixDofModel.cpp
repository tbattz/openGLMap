//
// Created by bcub3d-desktop on 13/1/20.
//



// Project Includes
#include "sixDofModel.h"

/* Constructor */
SixDofModel::SixDofModel(const GLchar* path) : Model(path) {
    // Set default id until assigned
    this->id = -1;
    // Set default position and attitude
    this->position = glm::dvec3(0.0f, 0.0f, 0.0f);
    this->attitude = glm::dvec3(0.0f, 0.0f, 0.0f);
}

/* Functions */
void SixDofModel::setId(int newId) {
    this->id = newId;
}

int SixDofModel::getId() {
    return this->id;
}

void SixDofModel::Draw(Shader shader) {
    // Do translation and rotation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position[0], position[2], position[1])); // Translate first due to GLM ordering, rotations opposite order
    model = glm::rotate(model, (float)attitude[2], glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate about y, yaw
    model = glm::rotate(model,(float)attitude[1],glm::vec3(0.0f,0.0f,1.0f)); // Rotate about z, pitch
    model = glm::rotate(model,(float)attitude[0],glm::vec3(1.0f,0.0f,0.0f)); // Rotate about x, roll

    // Update Uniforms
    glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE,glm::value_ptr(model));

    // Draw Model
    Model::Draw(shader);
}



