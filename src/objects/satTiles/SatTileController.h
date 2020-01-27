//
// Created by bcub3d-desktop on 26/1/20.
//

#ifndef OPENGLMAP_SATTILECONTROLLER_H
#define OPENGLMAP_SATTILECONTROLLER_H


// Standard Includes
#include <string>

// GLM Mathematics
#include <glm/glm.hpp>

// Project Includes
#include "SatTileModel.h"
#include "SatTileView.h"


class SatTileController {
public:
    /* Constructor */
    SatTileController(glm::vec3 origin, int x, int y, int zoom, std::string filename);

    /* Functions */
    void draw(Shader shader);

private:
    /* Model */
    SatTileModel model;

    /* View */
    SatTileView view;
};


#endif //OPENGLMAP_SATTILECONTROLLER_H
