//
// Created by bcub3d-desktop on 19/1/20.
//

#ifndef OPENGLMAP_MAVAIRCRAFTCONTROLLER_H
#define OPENGLMAP_MAVAIRCRAFTCONTROLLER_H


// Project Includes
#include <settings.h>
#include <model/mavAircraft.h>
#include <view/RenderEngine.h>


class MavAircraftController {
public:
    /* Constructor */
    explicit MavAircraftController(Settings* settings, RenderEngine* renderEngine);

    /* Functions */
    void createMavAircraftFromSettings();
    void updateAircraftPositions();

private:
    /* Settings */
    Settings* settings;
    RenderEngine* renderEngine;
    glm::vec3 worldOrigin;
    /* Aircraft */
    std::vector<MavAircraft*> mavAircraftList;
};

#endif //OPENGLMAP_MAVAIRCRAFTCONTROLLER_H
