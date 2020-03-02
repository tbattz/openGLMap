//
// Created by bcub3d-desktop on 22/1/20.
//

#include "WorldObjectModel.h"

/* Constructors */
WorldObjectModel::WorldObjectModel() {
    this->position = glm::dvec3(0.0f, 0.0f, 0.0f);
    this->attitude = glm::dvec3(0.0f, 0.0f, 0.0f);
}

WorldObjectModel::WorldObjectModel(glm::dvec3 position, glm::dvec3 attitude) {
    this->position = position;
    this->attitude = attitude;
}

glm::dvec3 WorldObjectModel::getPosition() {
    return position;
}

glm::dvec3 WorldObjectModel::getAttitude() {
    return attitude;
}

void WorldObjectModel::setPosition(glm::dvec3 newPosition) {
    this->position = newPosition;
}

void WorldObjectModel::setAttitude(glm::dvec3 newAttitude) {
    this->attitude = newAttitude;
}


