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
    glm::dmat3 A = glm::dmat3(-sin(lon),					cos(lon),					0.0,
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
    glm::dvec3 ecefPosition = UnitConversions::geo2ECEF(geoPosition);
    glm::dvec3 ecefOrigin = UnitConversions::geo2ECEF(origin);

    /* Convert from ECEF to ENU */
    glm::dvec3 position = UnitConversions::ecef2ENU(ecefPosition, ecefOrigin, origin);

    return position;
}


glm::dvec3 UnitConversions::enu2Ecef(glm::dvec3 position, glm::dvec3 origin) {
    // Convert x,y,z position to ECEF
    GLdouble lat = glm::radians(origin[0]);
    GLdouble lon = glm::radians(origin[1]);
    /* Calculate origin in ECEF */
    glm::dvec3 ecefOrigin = UnitConversions::geo2ECEF(origin);

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

glm::dvec3 UnitConversions::ecef2Geo(glm::dvec3 ecefPosition) {
    /* Convert ECEF position to (lat, lon, alt) using Ferrari's solution */
    return UnitConversions::calcFerrariSolution(ecefPosition);
}

glm::dvec3 UnitConversions::enu2Geo(glm::dvec3 positionVector, glm::dvec3 origin) {
    // Convert x,y,z position to geo-position
    /* Convert from ENU to ECEF */
    glm::dvec3 ecefPosition = UnitConversions::enu2Ecef(positionVector, origin);

    /* Convert ECEF to Geodetic */
    glm::dvec3 geoPosition = UnitConversions::ecef2Geo(ecefPosition);

    return geoPosition;
}

glm::dvec3 UnitConversions::calcFerrariSolution(glm::dvec3 ecefPosition) {
    // See https://en.wikipedia.org/wiki/Geographic_coordinate_conversion#Ferrari's_solution
    // Simplify expressions
    GLdouble x = ecefPosition[0];
    GLdouble y = ecefPosition[1];
    GLdouble z = ecefPosition[2];
    GLdouble a2 = pow(a, 2);
    GLdouble b2 = pow(b, 2);
    GLdouble z2 = pow(z, 2);
    GLdouble e4 = pow(e2, 2);
    // Calculate experessions
    GLdouble r = sqrt(pow(x, 2) + pow(y, 2));
    GLdouble r2 = pow(r, 2);
    GLdouble edash2 = (a2 - b2) / b2;
    GLdouble F = 54*b2*z2;
    GLdouble G = r2 + (((1-e2)*z2) - (e2*(a2-b2)));
    GLdouble c = e4*F*r2/pow(G, 3);
    GLdouble s = pow(1 + c + sqrt(pow(c, 2) + (2*c)), 1./3.);
    GLdouble P = F / (3*pow(s + (1/s) + 1, 2)*pow(G, 2));
    GLdouble Q = sqrt(1 + (2*e4*P));
    GLdouble r0_1 = (-P*e2*r)/(1+Q);
    GLdouble r0_2a = 0.5*a2*(1+(1/Q));
    GLdouble r0_2b = (P*(1-e2)*z2)/(Q*(1+Q));
    GLdouble r0_2c = 0.5*P*r2;
    GLdouble r0 = r0_1 + sqrt(r0_2a - r0_2b - r0_2c);
    GLdouble U = sqrt(pow(r - (e2*r0), 2) + z2);
    GLdouble V = sqrt(pow(r - (e2*r0), 2) + ((1 - e2)*z2));
    GLdouble z0 = b2 * z / (a*V);
    GLdouble h = U * (1 - (b2/(a*V)));
    GLdouble lat = glm::degrees(atan2(z + (edash2*z0), r));
    GLdouble lon = glm::degrees(atan2(y, x));

    return glm::dvec3(lat, lon, h);
}
