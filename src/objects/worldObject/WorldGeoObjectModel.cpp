//
// Created by bcub3d-desktop on 16/2/20.
//

#include "WorldGeoObjectModel.h"



// GLM Mathematics
#include <glm/glm.hpp>

WorldGeoObjectModel::WorldGeoObjectModel(glm::dvec3 geoPosition, glm::dvec3 origin) : WorldObjectModel() {
    this->origin = origin;
    WorldGeoObjectModel::setGeoPosition(geoPosition);
    WorldObjectModel::setAttitude(glm::dvec3(0.0f, 0.0f, 0.0f));
}

WorldGeoObjectModel::WorldGeoObjectModel(glm::dvec3 geoPosition, glm::dvec3 attitude, glm::dvec3 origin) {
    this->origin = origin;
    WorldGeoObjectModel::setGeoPosition(geoPosition);
    WorldObjectModel::setAttitude(attitude);
}


glm::dvec3 WorldGeoObjectModel::getGeoPosition() {
    return this->geoPosition;
}

void WorldGeoObjectModel::setPosition(glm::dvec3 newPosition) {
    WorldObjectModel::setPosition(newPosition);
    // Calculate equivalent geo-position
    glm::dvec3 newGeoPosition = UnitConversions::neu2Geo(newPosition, this->origin);
    WorldGeoObjectModel::setGeoPosition(newGeoPosition);
}

void WorldGeoObjectModel::setGeoPosition(glm::dvec3 newGeoPosition) {
    this->geoPosition = newGeoPosition;
    // Calculate position in x, y, z
    glm::dvec3 newPosition = UnitConversions::geo2NEU(geoPosition, origin);
    WorldObjectModel::setPosition(newPosition);
}

