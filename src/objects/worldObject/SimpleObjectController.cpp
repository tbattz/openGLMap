//
// Created by bcub3d-desktop on 13/3/20.
//

#include "SimpleObjectController.h"

SimpleObjectController::SimpleObjectController(glm::dvec3 position, const char* filename) : IWorldObjectController(), simpleSquareView(filename) {

    // Create model
    this->worldObjectModel = WorldObjectModel(position, glm::dvec3(0.0f, 0.0f, 0.0f));

}

void SimpleObjectController::draw(Shader shader) {
    // Get Position and Orientation
    glm::vec3 position = this->worldObjectModel.getPosition();
    glm::vec3 attitude = this->worldObjectModel.getAttitude();

    // Do Translation and Rotation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(position[0],position[2],position[1]));// Translate first due to GLM ordering, rotations opposite order
    model = glm::rotate(model,(float)attitude[2],glm::vec3(0.0f,1.0f,0.0f)); // Rotate about y, yaw
    model = glm::rotate(model,(float)attitude[1],glm::vec3(0.0f,0.0f,1.0f)); // Rotate about z, pitch
    model = glm::rotate(model,(float)attitude[0],glm::vec3(1.0f,0.0f,0.0f)); // Rotate about x, roll

    // Update Uniforms
    glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE,glm::value_ptr(model));

    // Draw Model
    this->simpleSquareView.Draw(shader);
}

void SimpleObjectController::updatePosition(glm::dvec3 position, glm::dvec3 attitude) {
    this->worldObjectModel.setPosition(position);
    this->worldObjectModel.setAttitude(attitude);
}

glm::dvec3 SimpleObjectController::getPosition() {
    return this->worldObjectModel.getPosition();
};

std::shared_ptr<WorldObjectModel> SimpleObjectController::getWorldObjectModel() {
    return std::make_shared<WorldObjectModel>(worldObjectModel);
}
