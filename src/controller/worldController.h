//
// Created by bcub3d-desktop on 19/1/20.
//

#ifndef OPENGLMAP_WORLDCONTROLLER_H
#define OPENGLMAP_WORLDCONTROLLER_H

// Project Includes
#include <settings.h>
#include <view/RenderEngine.h>
#include "mavAircraftController.h"


class WorldController {
public:
    /* Constructor */
    WorldController(Settings* settings, RenderEngine* renderEngine);

    /* Functions */
    void updatePositions();


private:
    /* Settings */
    Settings* settings;
    /* Render Engine */
    RenderEngine* renderEngine;
    /* Controllers */
    MavAircraftController* mavAircraftController;
};


#endif //OPENGLMAP_WORLDCONTROLLER_H
