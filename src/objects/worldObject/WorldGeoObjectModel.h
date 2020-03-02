//
// Created by bcub3d-desktop on 16/2/20.
//

#ifndef OPENGLMAP_WORLDGEOOBJECTMODEL_H
#define OPENGLMAP_WORLDGEOOBJECTMODEL_H


#include <utilities/UnitConversions.h>
#include "WorldObjectModel.h"

class WorldGeoObjectModel : public WorldObjectModel, public UnitConversions {
public:
    /* Constructors */
    WorldGeoObjectModel(glm::dvec3 origin);
    WorldGeoObjectModel(glm::dvec3 geoPosition, glm::dvec3 attitude, glm::dvec3 origin);

    /* Functions */
    glm::dvec3 getGeoPosition();
    void setPosition(glm::dvec3 newPosition);
    void setGeoPosition(glm::dvec3 newGeoPosition);

private:
    /* Data */
    glm::dvec3 origin;         // (lat (deg), lon (deg), alt (m))
    glm::dvec3 geoPosition;    // (lat (deg), lon (deg), alt (m))

};




#endif //OPENGLMAP_WORLDGEOOBJECTMODEL_H
