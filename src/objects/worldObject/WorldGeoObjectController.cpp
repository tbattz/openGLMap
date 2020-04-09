//
// Created by bcub3d-desktop on 22/1/20.
//

// Project Includes
#include "WorldGeoObjectController.h"
#include "IWorldObjectController.h"

WorldGeoObjectController::WorldGeoObjectController(const GLchar* path, glm::dvec3 geoPosition, glm::dvec3 origin) : IWorldObjectController(), worldGeoObjectModel(geoPosition, origin), worldObjectView(path) {

}

void WorldGeoObjectController::draw(Shader shader) {
    // Get Position and Orientation
    glm::vec3 position = this->worldGeoObjectModel.getPosition();
    glm::vec3 attitude = this->worldGeoObjectModel.getAttitude();

    // Do Translation and Rotation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(position[0],position[2],position[1]));  // Translate first due to GLM ordering, rotations opposite order
    model = glm::rotate(model,(float)attitude[2],glm::vec3(0.0f,1.0f,0.0f)); // Rotate about y, yaw
    model = glm::rotate(model,(float)attitude[1],glm::vec3(0.0f,0.0f,1.0f)); // Rotate about z, pitch
    model = glm::rotate(model,(float)attitude[0],glm::vec3(1.0f,0.0f,0.0f)); // Rotate about x, roll

    // Update Uniforms
    glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE,glm::value_ptr(model));

    // Draw Model
    this->worldObjectView.Draw(shader);

}

void WorldGeoObjectController::updateGeoPosition(glm::dvec3 geoPosition, glm::dvec3 attitude) {
    this->worldGeoObjectModel.setGeoPosition(geoPosition);
    this->worldGeoObjectModel.setAttitude(attitude);
}

glm::dvec3 WorldGeoObjectController::getPosition() {
    return this->worldGeoObjectModel.getPosition();
}

glm::dvec3 WorldGeoObjectController::getGeoPosition() {
    return this->worldGeoObjectModel.getGeoPosition();
};

std::shared_ptr<WorldGeoObjectModel> WorldGeoObjectController::getWorldGeoObjectModel() {
    return std::make_shared<WorldGeoObjectModel>(worldGeoObjectModel);
}

