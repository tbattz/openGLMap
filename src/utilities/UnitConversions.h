//
// Created by bcub3d-desktop on 26/1/20.
//

#ifndef OPENGLMAP_UNITCONVERSIONS_H
#define OPENGLMAP_UNITCONVERSIONS_H


// GLM Mathematics
#include <glm/glm.hpp>

// GLAD - Multi Language GL Loader-Generator
#include <glad/glad.h>

class UnitConversions {
public:
    /* Constructor */
    UnitConversions();

    /* Functions */
    static glm::dvec3 geo2ECEF(glm::dvec3 positionVector);
    static glm::dvec3 ecef2NEU(glm::dvec3 ecefVector, glm::dvec3 ecefOrigin, glm::dvec3 origin);
    static glm::dvec3 geo2NEU(glm::dvec3 geoPosition, glm::dvec3 origin);
    static glm::dvec3 neu2Ecef(glm::dvec3 position, glm::dvec3 origin);
    static glm::dvec3 ecef2Geo(glm::dvec3 ecefPosition);
    static glm::dvec3 neu2Geo(glm::dvec3 positionVector, glm::dvec3 origin);

private:
    /* Functions */
    static glm::dvec3 calcFerrariSolution(glm::dvec3 ecefPosition);

    /* Constants */
    constexpr static GLdouble a = 6378137.0;
    constexpr static GLdouble b = 6356752.3142;
    constexpr static GLdouble e2 = 6.69437999014e-3;
};


#endif //OPENGLMAP_UNITCONVERSIONS_H
