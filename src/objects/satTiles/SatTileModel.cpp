//
// Created by bcub3d-desktop on 26/1/20.
//

#include "SatTileModel.h"


/* Constructor */
SatTileModel::SatTileModel(glm::vec3 origin, int x, int y, int zoom)  {
    this->origin = origin;
    this->x = x;
    this->y = y;
    this->zoom = zoom;

    /* Calculate geoPosition from x,y,zoom */
    calcGeoPosition();

    /* Convert Geodetic to ECEF */
    glm::dvec3 ecefPosition = geo2ECEF(geoPosition);
    glm::dvec3 ecefOrigin = geo2ECEF(origin);

    /* Convert from ECEF to ENU */
    glm::dvec3 tempPos = ecef2NEU(ecefPosition, ecefOrigin, origin);
    position = glm::dvec3(tempPos[1],tempPos[0],0.0f);

    /* Calculate Width */
    calcTileWidthHeightAll(ecefOrigin);

}

void SatTileModel::calcGeoPosition() {
    std::vector<double> geoPos = tileNum2LatLon((float)x, (float)y, zoom);
    geoPosition = glm::dvec3(geoPos[0],geoPos[1],0.0);
}

std::vector<double> SatTileModel::tileNum2LatLon(float x, float y, int zoom) {
    int n = pow(2,zoom);
    double lat = atan(sinh(M_PI - (y*2*M_PI/n)))*180.0/M_PI;
    double lon = (x/n)*360.0 - 180.0;

    return {lat, lon};
}

std::vector<double> SatTileModel::calcTileWidthHeight(glm::dvec3 ecefOrigin, glm::dvec3 geoPos1, glm::dvec3 geoPos2) {
    // Calculates the width and height offsets between two geo positions in (x m, y m)
    glm::dvec3 ecefPos1 = geo2ECEF(geoPos1);
    glm::dvec3 ecefPos2 = geo2ECEF(geoPos2);
    glm::dvec3 pos1 = ecef2NEU(ecefPos1, ecefOrigin, origin);
    glm::dvec3 pos2 = ecef2NEU(ecefPos2, ecefOrigin, origin);
    glm::dvec3 diff = pos1 - pos2;

    double h = fabs(diff[0])+0.05;
    double w = fabs(diff[1])+0.05;

    return {w, h};
}

void SatTileModel::calcTileWidthHeightAll(glm::dvec3 ecefOrigin) {
    // Calculates the tile width and height in meters
    std::vector<double> geoTL = tileNum2LatLon(x,y,zoom);
    std::vector<double> geoTR = tileNum2LatLon(x+1,y,zoom);
    std::vector<double> geoBL = tileNum2LatLon(x,y+1,zoom);
    std::vector<double> geoBR = tileNum2LatLon(x+1,y+1,zoom);
    // Convert
    glm::dvec3 geoPosTL = glm::dvec3(geoTL[0],geoTL[1],0.0f);
    glm::dvec3 geoPosTR = glm::dvec3(geoTR[0],geoTR[1],0.0f);
    glm::dvec3 geoPosBL = glm::dvec3(geoBL[0],geoBL[1],0.0f);
    glm::dvec3 geoPosBR = glm::dvec3(geoBR[0],geoBR[1],0.0f);
    // Get width height differences
    std::vector<double> diffsTLTR = calcTileWidthHeight(ecefOrigin, geoPosTL, geoPosTR);
    std::vector<double> diffsTLBL = calcTileWidthHeight(ecefOrigin, geoPosTL, geoPosBL);
    std::vector<double> diffsTLBR = calcTileWidthHeight(ecefOrigin, geoPosTL, geoPosBR);
    // Store differences
    xyOff = {};
    xyOff.push_back(diffsTLTR);
    xyOff.push_back(diffsTLBL);
    xyOff.push_back(diffsTLBR);
}

std::vector<std::vector<double>> SatTileModel::getXYOffset() {
    return xyOff;
}

glm::dvec3 SatTileModel::getPosition() {
    return position;
}

GLfloat SatTileModel::getBrightness() {
    return brightness;
}



