//
// Created by bcub3d-desktop on 19/1/20.
//

// Project Includes
#include <view/RenderEngine.h>
#include "worldController.h"
#include "mavAircraftController.h"


WorldController::WorldController(Settings* settings, RenderEngine* renderEngine) {
    this->settings = settings;
    this->renderEngine = renderEngine;

    // Create MavAircraft Controller
    this->mavAircraftController = new MavAircraftController(settings, renderEngine);


}

void WorldController::updatePositions() {
    mavAircraftController->updateAircraftPositions();
}


