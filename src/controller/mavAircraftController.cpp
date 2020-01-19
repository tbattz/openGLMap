//
// Created by bcub3d-desktop on 19/1/20.
//

#include <view/RenderEngine.h>
#include "mavAircraftController.h"

MavAircraftController::MavAircraftController(Settings* settings, RenderEngine* renderEngine) {
    this->settings = settings;
    this->renderEngine = renderEngine;
    this->worldOrigin = glm::vec3(settings->origin[0], settings->origin[1], settings->origin[2]);

    // Add aircraft from config
    createMavAircraftFromSettings();
}

void MavAircraftController::createMavAircraftFromSettings() {
    /* Create the mav aircraft specified in the config file. */
    for (unsigned int i=0; i < settings->aircraftConList.size(); i++) {
        const GLchar* path = settings->aircraftConList[i].filepath.c_str();
        string name = settings->aircraftConList[i].name;
        MavAircraft* mavAircraft = new MavAircraft(path, worldOrigin, name);
        // Store aircraft
        mavAircraftList.push_back(mavAircraft);
        // Register with the render engine
        renderEngine->registerSixDofModel(mavAircraft);
    }
}

void MavAircraftController::updateAircraftPositions() {
    for(unsigned int i =0; i < mavAircraftList.size(); i++) {
        mavAircraftList[i]->updatePositionAttitude();
    }
}