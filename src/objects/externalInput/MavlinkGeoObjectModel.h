//
// Created by bcub3d-desktop on 16/3/20.
//

#ifndef OPENGLMAP_MAVLINKGEOOBJECTMODEL_H
#define OPENGLMAP_MAVLINKGEOOBJECTMODEL_H

// Standard Includes
#include <vector>
#include <mutex>

// Boost Includes
#include <boost/circular_buffer.hpp>

// Project Includes
#include <objects/worldObject/WorldGeoObjectModel.h>
#include <utilities/BufferUtil.h>


class MavlinkGeoObjectModel : public WorldGeoObjectModel, public BufferUtil {
public:
    /* Constructors */
    MavlinkGeoObjectModel(std::string name, glm::dvec3 geoPosition, glm::dvec3 origin);
    MavlinkGeoObjectModel(std::string name, glm::dvec3 geoPosition, glm::dvec3 attitude, glm::dvec3 origin);

    /* Functions */
    std::string getName();
    // Position Functions
    void setFirstPositionMsgReceived(bool firstMsgReceived);
    bool getFirstPositionMsgReceived();
    void setStartTimePos(double startTime);
    double getStartTimePos();
    void setStartTimePosMavlink(double mavlinkStartTime);
    double getStartTimePosMavlink();
    void setGeoPosition(glm::dvec3 newGeoPosition);
    void setVelocity(glm::dvec3 newVelocity);
    void setTimePosition(double newTime);

    // Attitude Functions
    void setFirstAttitudeMsgReceived(bool firstMsgReceived);
    bool getFirstAttitudeMsgReceived();
    void setStartTimeAtt(double startTime);
    double getStartTimeAtt();
    void setStartTimeAttMavlink(double mavlinkStartTime);
    double getStartTimeAttMavlink();
    void setAttitude(glm::dvec3 newAttitude);
    void setAttitudeRate(glm::dvec3 newAttitudeRate);
    void setTimeAttitude(double newTime);

    // HUD Functions
    void setAirspeed(float newAirspeed);
    void setHeading(double newHeading);

    // Interpolation
    void interpolatePosition();
    void interpolateAttitude();
    void calculatePositionInterpolationConstants();
    void calculateAttitudeInterpolationConstants();
    void updatePositionAttitude(double glfwTime);

    /* Locks */
    std::mutex                          dataLock;                    // Used to not overwrite circular buffers while reading from them

    // Plotting data
    std::vector<float>                  posTimeLog;
    std::vector<float>                  attTimeLog;
    std::vector<glm::dvec3>             posLog;
    std::vector<glm::dvec3>             attLog;
    std::vector<glm::dvec3>             velLog;

private:
    /* Data */
    std::string                         name;                        // The name of the vehicle
    // Position History Information
    std::vector<glm::dvec3>             geoPositionHistory;          // Vector of Lat (deg), Lon (deg), alt (km)
    std::vector<glm::dvec3>             positionHistory;             // Vector (x,y,z) relative to origin
    std::vector<glm::dvec3>             velocityHistory;             // Vector of (vz, vy, vz)
    std::vector<double>                 timePositionHistory;         // Vector of floats corresponding to times of position history
    bool                                firstPositionMessage = false;// True if the first message has been received
    unsigned int                        currentPosMsgIndex = 0;      // Index of the 'latest' position mavlink message being displayed (this is behind the data)

    // Attitude Information
    std::vector<glm::dvec3>             attitudeHistory;             // Vector of attitudes
    std::vector<glm::dvec3>             attitudeRateHistory;         // Vector of attitude rates
    std::vector<double>                 timeAttitudeHistory;         // Vector of floats corresponding to times of attitude history
    bool                                firstAttitudeMessage = false;// True if the first message has been received
    unsigned int                        currentAttMsgIndex = 0;      // Index of the 'latest' attitude mavlink message being displayed (behind the data)

    // Time Information
    double                              timeStartPos = 0;            // Offset between the autopilot boot time and glfw time (used to sync times)
    double                              timeStartMavlinkPos = 0;     // Boot time of the first mavlink message
    double                              timeStartAtt = 0;
    double                              timeStartMavlinkAtt = 0;
    float                               timeDelay = 0.3;             // Delay between receiving the first mavlink message and displaying it (s)
    float                               timeDelayDelta=0.1;          // The delta to add when increasing the delay
    double                              currTime = 0;                // The current time
    float                               dtPos = 0;                   // Timestep between current frame and last current position mavlink message time
    float                               dtAtt = 0;                   // Timestep between current frame and last current attitude mavlink message time
    double                              minDiff = 10;                // Minimum difference between current sim time and time of latest mavlink message

    // Interpolation Information
    glm::dvec3                          xPosConst;
    glm::dvec3                          yPosConst;
    glm::dvec3                          zPosConst;
    glm::dvec2                          xAttConst;
    glm::dvec2                          yAttConst;
    glm::dvec2                          zAttConst;

    // Airspeed Information
    float                               airspeed = 0;                 // (m/s)
    double                              heading = 0;                  // (rad)

};


#endif //OPENGLMAP_MAVLINKGEOOBJECTMODEL_H
