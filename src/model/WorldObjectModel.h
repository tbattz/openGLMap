//
// Created by bcub3d-desktop on 22/1/20.
//

#ifndef OPENGLMAP_WORLDOBJECTMODEL_H
#define OPENGLMAP_WORLDOBJECTMODEL_H

// GLM Mathematics
#include <glm/glm.hpp>

class WorldObjectModel {
public:
    /* Constructors */
    WorldObjectModel();
    WorldObjectModel(glm::dvec3 position, glm::dvec3 attitude);

    /* Functions */
    glm::dvec3 getPosition();
    glm::dvec3 getAttitude();
    void setPosition(glm::dvec3 newPosition);
    void setAttitude(glm::dvec3 newAttitude);

private:
    /* Data */
    glm::dvec3 position;    // (x,y,z) relative to origin
    glm::dvec3 attitude;    // roll (rad), pitch (rad), yaw (rad)

};


#endif //OPENGLMAP_WORLDOBJECTMODEL_H
