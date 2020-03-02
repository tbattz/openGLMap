//
// Created by bcub3d-desktop on 16/2/20.
//

#include "WorldGeoObjectModel.h"



// GLM Mathematics
#include <glm/glm.hpp>

WorldGeoObjectModel::WorldGeoObjectModel(glm::dvec3 origin) : WorldObjectModel() {
    this->origin = origin;


}

WorldGeoObjectModel::WorldGeoObjectModel(glm::dvec3 geoPosition, glm::dvec3 attitude, glm::dvec3 origin) {
    this->origin = origin;
    setGeoPosition(geoPosition);
    setAttitude(attitude);
}


glm::dvec3 WorldGeoObjectModel::getGeoPosition() {
    return this->geoPosition;
}

void WorldGeoObjectModel::setPosition(glm::dvec3 newPosition) {
    WorldObjectModel::setPosition(newPosition);
    // Calculate equivalent geo-position


}

void WorldGeoObjectModel::setGeoPosition(glm::dvec3 newGeoPosition) {
    this->geoPosition = newGeoPosition;
    // Calculate position in x, y, z
    glm::dvec3 newPosition = UnitConversions::geo2ENU(geoPosition, origin);
    WorldObjectModel::setPosition(newPosition);
}

