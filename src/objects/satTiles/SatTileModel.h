//
// Created by bcub3d-desktop on 26/1/20.
//

#ifndef OPENGLMAP_SATTILEMODEL_H
#define OPENGLMAP_SATTILEMODEL_H

// Standard Includes
#include <vector>

// GLM Mathematics
#include <glm/glm.hpp>

// Project Includes
#include <utilities/UnitConversions.h>
#include <renderEngine/shader.h>


class SatTileModel : public UnitConversions {
public:
    /* Constructor */
    SatTileModel(glm::vec3 origin, int x, int y, int zoom);

    /* Functions */
    std::vector<std::vector<double>> getXYOffset();
    glm::dvec3 getPosition();
    GLfloat getBrightness();

private:
    /* Data */
    // Position Information
    glm::vec3 origin;           // Lat (deg), Lon (deg), alt (km)
    glm::dvec3 position;        // (x, y, z) relative to origin
    glm::dvec3 geoPosition;     // Lat (deg), Lon (deg), alt (km)
    // Tile Information
    GLfloat brightness = 1.0;          // Multipler for brightness
    int x;
    int y;
    int zoom;
    std::vector<std::vector<double>> xyOff;	// Meters, (wTR,hTR,wBL,hBL,wBR,hBR)

    /* Functions */
    void calcGeoPosition();
    std::vector<double> tileNum2LatLon(float x, float y, int zoom);
    std::vector<double> calcTileWidthHeight(glm::dvec3 ecefOrigin, glm::dvec3 geoPos1, glm::dvec3 geoPos2);
    void calcTileWidthHeightAll(glm::dvec3 ecefOrigin);



};


#endif //OPENGLMAP_SATTILEMODEL_H
