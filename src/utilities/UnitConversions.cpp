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

    // Flipped due to GLM ordering
    glm::dvec3 tempPos = B*A;
    glm::dvec3 position = glm::dvec3(tempPos[1], tempPos[0], tempPos[2]);

    return position;
}

glm::dvec3 UnitConversions::geo2ENU(glm::dvec3 geoPosition, glm::dvec3 origin) {
    // Convert geo-position to x,y,z position
    /* Convert Geodetic to ECEF */
    glm::dvec3 ecefPosition = geo2ECEF(geoPosition);
    glm::dvec3 ecefOrigin = geo2ECEF(origin);

    /* Convert from ECEF to ENU */
    glm::dvec3 position = ecef2ENU(ecefPosition, ecefOrigin, origin);

    return position;
}


glm::dvec3 UnitConversions::enu2Geo(glm::dvec3 positionVector, glm::dvec3 origin) {
    // Convert x,y,z position to geo-position


    return glm::dvec3();
}

glm::dvec3 UnitConversions::enu2Ecef(glm::dvec3 position, glm::dvec3 origin) {
    // Convert x,y,z position to ECEF
    GLdouble lat = glm::radians(origin[0]);
    GLdouble lon = glm::radians(origin[1]);
    /* Calculate origin in ECEF */
    glm::dvec3 ecefOrigin = geo2ECEF(origin);

    glm::dvec3 tempPos = glm::dvec3(position[1], position[0], position[2]);

    /* Create Matrices */
    glm::mat3 A = glm::mat3(-sin(lon), -sin(lat)*cos(lon), cos(lat)*cos(lon),
                            cos(lon),  -sin(lat)*sin(lon), cos(lat)*sin(lon),
                            0,     cos(lat),               sin(lat));
    glm::dvec3 B = tempPos;

    // Flipped due to GLM ordering
    glm::dvec3 ecefTemp = B*A;
    glm::dvec3 ecefPosition = ecefTemp + ecefOrigin;

    return ecefPosition;
}


