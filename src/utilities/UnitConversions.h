//
// Created by bcub3d-desktop on 26/1/20.
//

#ifndef OPENGLMAP_UNITCONVERSIONS_H
#define OPENGLMAP_UNITCONVERSIONS_H


// GL Includes
#include <GL/glew.h>

// GLM Mathematics
#include <glm/glm.hpp>


class UnitConversions {
public:
    /* Constructor */
    UnitConversions();

    /* Functions */
    static glm::dvec3 geo2ECEF(glm::dvec3 positionVector);
    static glm::dvec3 ecef2ENU(glm::dvec3 ecefVector, glm::dvec3 ecefOrigin, glm::dvec3 origin);
    static glm::dvec3 geo2ENU(glm::dvec3 geoPosition, glm::dvec3 origin);
    static glm::dvec3 enu2Ecef(glm::dvec3 position, glm::dvec3 origin);
    static glm::dvec3 enu2Geo(glm::dvec3 positionVector, glm::dvec3 origin);

};


#endif //OPENGLMAP_UNITCONVERSIONS_H