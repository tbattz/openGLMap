//
// Created by bcub3d-desktop on 16/3/20.
//

#ifndef OPENGLMAP_MAVLINKGEOOBJECTCONTROLLER_H
#define OPENGLMAP_MAVLINKGEOOBJECTCONTROLLER_H

// Standard Includes
#include <utility>

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Boost Includes
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

// Mavlink Includes
#include <c_library_v2/ardupilotmega/mavlink.h>

// Project Includes
#include "MavlinkGeoObjectModel.h"
#include <renderEngine/shader.h>
#include <objects/worldObject/WorldObjectView.h>
#include <objects/worldObject/IWorldObjectController.h>


class MavlinkGeoObjectController : public IWorldObjectController {
public:
    /* Constructor */
    MavlinkGeoObjectController(std::string name, std::string host, std::string port, const GLchar* path, glm::dvec3 geoPosition, glm::dvec3 origin);

    /* Destructor */
    ~MavlinkGeoObjectController();

    /* Functions */
    glm::dvec3 getPosition();
    glm::dvec3 getGeoPosition();
    void draw(Shader shader);

private:
    /* Functions */
    void startSocket();
    void closeSocket();

    void processGlobalPositionMsg(mavlink_message_t msg);
    void processAttitudeMsg(mavlink_message_t msg);
    void processVFRHudMsg(mavlink_message_t msg);

    /* Models */
    MavlinkGeoObjectModel mavlinkGeoObjectModel;

    /* Views */
    WorldObjectView worldObjectView;

    /* Data */
    bool socketRunning = true;
    std::thread* threadPt = nullptr;
    std::string host;
    std::string port;

};


#endif //OPENGLMAP_MAVLINKGEOOBJECTCONTROLLER_H
