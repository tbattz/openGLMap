//
// Created by bcub3d-desktop on 22/1/20.
//

#include "WorldObjectController.h"

WorldObjectController::WorldObjectController(const GLchar* path) : IWorldObjectController(), worldObjectView(path) {

    // Create object
    this->worldObjectModel = WorldObjectModel();

}

void WorldObjectController::draw(Shader shader) {
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
    this->worldObjectView.Draw(shader);

}

void WorldObjectController::updatePosition(glm::dvec3 position, glm::dvec3 attitude) {
    this->worldObjectModel.setPosition(position);
    this->worldObjectModel.setAttitude(attitude);
}

glm::dvec3 WorldObjectController::getPosition() {
    return this->worldObjectModel.getPosition();
};

void WorldObjectController::incrementPosition() {
    glm::dvec3 currPos = worldObjectModel.getPosition();
    glm::dvec3 diff = glm::dvec3(0.1f, 0.0f, 0.0f);
    glm::dvec3 newPos = currPos + diff;

    worldObjectModel.setPosition(newPos);
}

std::shared_ptr<WorldObjectModel> WorldObjectController::getWorldObjectModel() {
    return std::make_shared<WorldObjectModel>(worldObjectModel);
}

