//
// Created by bcub3d-desktop on 26/1/20.
//

#include "UnitConversions.h"

/* Constructor */
UnitConversions::UnitConversions() {

}

/* Conversion Geodetic to ECEF */
glm::dvec3 UnitConversions::geo2ECEF(glm::dvec3 positionVector) {
    // positionVector: (latitude, longitude, altitude (m))
    // Uses WGS84 defined here https://en.wikipedia.org/wiki/Geodetic_datum#Geodetic_to.2Ffrom_ECEF_coordinates
    GLdouble a = 6378137.0;
    GLdouble e2 = 6.69437999014e-3;
    GLdouble lat = glm::radians(positionVector[0]);
    GLdouble lon = glm::radians(positionVector[1]);
    //GLdouble alt = glm::radians(positionVector[2]);
    GLdouble N = a / sqrt(1-(e2*pow(sin(lat),2)));
    GLdouble h = positionVector[2]; // Convert to m
    GLdouble ex = (N+h)*cos(lat)*cos(lon); // m
    GLdouble ey = (N+h)*cos(lat)*sin(lon); // m
    GLdouble ez = (N*(1-e2) + h) * sin(lat);    // m

    return glm::dvec3(ex,ey,ez);
}

/* Convert from ECEF to ENU */
glm::dvec3 UnitConversions::ecef2ENU(glm::dvec3 ecefVector, glm::dvec3 ecefOrigin, glm::dvec3 origin) {
    GLdouble lat = glm::radians(origin[0]);
    GLdouble lon = glm::radians(origin[1]);
    //GLfloat alt = origin[2];
    glm::mat3 A = glm::mat3(-sin(lon),					cos(lon),					0.0,
                            -sin(lat)*cos(lon),	-sin(lat)*sin(lon),	cos(lat),
                            cos(lat)*cos(lon),	cos(lat)*sin(lon),	sin(lat));
    glm::dvec3 B = glm::dvec3(ecefVector[0]-ecefOrigin[0],ecefVector[1]-ecefOrigin[1],ecefVector[2]-ecefOrigin[2]);
    B*A; // Flipped due to GLM ordering

    return B*A;
}
