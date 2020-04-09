//
// Created by bcub3d-desktop on 16/3/20.
//


// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

#include <utility>
#include <iostream>

// Project Includes
#include "MavlinkGeoObjectModel.h"



MavlinkGeoObjectModel::MavlinkGeoObjectModel(std::string name, glm::dvec3 geoPosition, glm::dvec3 origin) : WorldGeoObjectModel(geoPosition, origin) {
    // Set name
    this->name = std::move(name);

    // Set geoPosition
    MavlinkGeoObjectModel::setGeoPosition(geoPosition);

    // Set attitude
    MavlinkGeoObjectModel::setAttitude(glm::dvec3(0.0f, 0.0f, 0.0f));
}

MavlinkGeoObjectModel::MavlinkGeoObjectModel(std::string name, glm::dvec3 geoPosition, glm::dvec3 attitude, glm::dvec3 origin) : WorldGeoObjectModel(geoPosition, attitude, origin) {
    // Set name
    this->name = std::move(name);

    // Set geoPosition
    MavlinkGeoObjectModel::setGeoPosition(geoPosition);

    // Set attitude
    MavlinkGeoObjectModel::setAttitude(attitude);
}

/* Functions */
std::string MavlinkGeoObjectModel::getName() {
    return this->name;
}

// Position Functions
void MavlinkGeoObjectModel::setFirstPositionMsgReceived(bool firstMsgReceived) {
    this->firstPositionMessage = firstMsgReceived;
}

bool MavlinkGeoObjectModel::getFirstPositionMsgReceived() {
    return this->firstPositionMessage;
}

void MavlinkGeoObjectModel::setStartTimePos(double startTime) {
    this->timeStartPos = startTime;
}

double MavlinkGeoObjectModel::getStartTimePos() {
    return this->timeStartPos;
}

void MavlinkGeoObjectModel::setStartTimePosMavlink(double mavlinkStartTime) {
    this->timeStartMavlinkPos = mavlinkStartTime;
}

double MavlinkGeoObjectModel::getStartTimePosMavlink() {
    return this->timeStartMavlinkPos;
}

void MavlinkGeoObjectModel::setGeoPosition(glm::dvec3 newGeoPosition) {
    // Add to geoPosition history
    this->geoPositionHistory.push_back(newGeoPosition);
    WorldGeoObjectModel::setGeoPosition(newGeoPosition);
    // Add to position history
    this->positionHistory.push_back(this->getPosition());
}

void MavlinkGeoObjectModel::setVelocity(glm::dvec3 newVelocity) {
    if(positionHistory.size() > 1) {
        newVelocity = glm::dvec3(0.0);
    }
    this->velocityHistory.push_back(newVelocity);
}

void MavlinkGeoObjectModel::setTimePosition(double newTime) {
    this->timePositionHistory.push_back(newTime);
}

// Attitude Functions
void MavlinkGeoObjectModel::setFirstAttitudeMsgReceived(bool firstMsgReceived) {
    this->firstAttitudeMessage = firstMsgReceived;
}

bool MavlinkGeoObjectModel::getFirstAttitudeMsgReceived() {
    return this->firstAttitudeMessage;
}

void MavlinkGeoObjectModel::setStartTimeAtt(double startTime) {
    this->timeStartAtt = startTime;
}

double MavlinkGeoObjectModel::getStartTimeAtt() {
    return this->timeStartAtt;
}

void MavlinkGeoObjectModel::setStartTimeAttMavlink(double mavlinkStartTime) {
    this->timeStartMavlinkAtt = mavlinkStartTime;
}

double MavlinkGeoObjectModel::getStartTimeAttMavlink() {
    return this->timeStartMavlinkAtt;
}

void MavlinkGeoObjectModel::setAttitude(glm::dvec3 newAttitude) {
    // Add to attitude history
    this->attitudeHistory.push_back(newAttitude);
    WorldObjectModel::setAttitude(newAttitude);
}

void MavlinkGeoObjectModel::setAttitudeRate(glm::dvec3 newAttitudeRate) {
    // Add to attitude rate history
    this->attitudeRateHistory.push_back(newAttitudeRate);
}

void MavlinkGeoObjectModel::setTimeAttitude(double newTime) {
    this->timeAttitudeHistory.push_back(newTime);
}

// HUD Functions
void MavlinkGeoObjectModel::setAirspeed(float newAirspeed) {
    this->airspeed = newAirspeed;
}

void MavlinkGeoObjectModel::setHeading(double newHeading) {
    this->heading = newHeading;
}

void MavlinkGeoObjectModel::interpolatePosition() {
    if(currentPosMsgIndex > 1) {
        // Recalculate Interpolation Constants
        MavlinkGeoObjectModel::calculatePositionInterpolationConstants();

        // Store Past Position
        glm::dvec3 oldPosition = MavlinkGeoObjectModel::getPosition();

        // Calculate Positions
        glm::dvec3 newPosition = glm::dvec3(0.0f, 0.0f, 0.0f);
        newPosition[0] = (0.5 * xPosConst[0] * dtPos * dtPos) + (xPosConst[1] * dtPos) + xPosConst[2];
        newPosition[1] = (0.5 * yPosConst[0] * dtPos * dtPos) + (yPosConst[1] * dtPos) + yPosConst[2];
        newPosition[2] = (0.5 * zPosConst[0] * dtPos * dtPos) + (zPosConst[1] * dtPos) + zPosConst[2];
        setPosition(newPosition);

        // Calculate Velocity
        glm::dvec3 newVelocity = glm::dvec3(0.0f, 0.0f, 0.0f);
        newVelocity[0] = (newPosition[0] - oldPosition[0]) / (-dtPos);
        newVelocity[1] = (newPosition[1] - oldPosition[1]) / (-dtPos);
        newVelocity[2] = (newPosition[2] - oldPosition[2]) / (-dtPos);
        MavlinkGeoObjectModel::setVelocity(newVelocity);

        // TODO - Cleanup
        posTimeLog.push_back(currTime + timeStartMavlinkPos - timeDelay);
        posLog.push_back(newPosition);
        velLog.push_back(newVelocity);
    }
}

void MavlinkGeoObjectModel::interpolateAttitude() {
    if (currentAttMsgIndex > 1) {
        // Recalculate Interpolation Constants
        MavlinkGeoObjectModel::calculateAttitudeInterpolationConstants();

        // Calculate attitude
        glm::dvec3 newAttitude = glm::dvec3(0.0f, 0.0f, 0.0f);
        newAttitude[0] = (xAttConst[0]*dtAtt) + xAttConst[1];
        newAttitude[1] = (yAttConst[0]*dtAtt) + yAttConst[1];
        newAttitude[2] = (zAttConst[0]*dtAtt) + zAttConst[1];
        MavlinkGeoObjectModel::setAttitude(newAttitude);

        // TODO - Cleanup
        attTimeLog.push_back(currTime + timeStartMavlinkAtt - timeDelay);
        attLog.push_back(newAttitude);
    }
}

void MavlinkGeoObjectModel::calculatePositionInterpolationConstants() {
    // Get Index
    unsigned int pos = currentPosMsgIndex;

    // x(t) = 0.5*a*t^2 + b*t + c
    // (t1,x1), (t2,x2), (t3,x3), t2 is current
    float t1 = timePositionHistory[pos-2] - timePositionHistory[pos];
    float t2 = timePositionHistory[pos-1] - timePositionHistory[pos];
    float t3 = 0;

    // Find inverse matrix of [x1,x2,v2]=[BLAH][a,b,c]
    glm::mat3x3 A = glm::mat3x3(0.5*t1*t1, t1, 1, 0.5*t2*t2,t2, 1, 0.5*t3*t3, t3, 1);
    glm::mat3x3 inv = glm::inverse(A);

    // Constants
    glm::dvec3 xvec = glm::dvec3(positionHistory[pos-2][0], positionHistory[pos-1][0], positionHistory[pos][0]);
    glm::dvec3 yvec = glm::dvec3(positionHistory[pos-2][1], positionHistory[pos-1][1], positionHistory[pos][1]);
    glm::dvec3 zvec = glm::dvec3(positionHistory[pos-2][2], positionHistory[pos-1][2], positionHistory[pos][2]);

    xPosConst = xvec*inv;       // Flipped due to GLM ordering
    yPosConst = yvec*inv;       // Flipped due to GLM ordering
    zPosConst = zvec*inv;       // Flipped due to GLM ordering
}

void MavlinkGeoObjectModel::calculateAttitudeInterpolationConstants() {
    // Get Index
    int pos = currentAttMsgIndex;

    // x(t) = at + b
    // v(t) = a
    // (t1,x1), (t2,x2), t2 is current
    float t1 = timeAttitudeHistory[pos-1] - timeAttitudeHistory[pos];
    float t2 = 0;

    // Find inverse matrix of [x1,x2]=[BLAH][a,b]
    glm::mat2x2 A = glm::mat2x2(t1, 1, t2, 1);
    glm::mat2x2 inv = glm::inverse(A);

    // Constants
    glm::dvec2 xvec = glm::dvec2(attitudeHistory[pos-1][0], attitudeHistory[pos][0]);
    glm::dvec2 yvec = glm::dvec2(attitudeHistory[pos-1][1], attitudeHistory[pos][1]);
    glm::dvec2 zvec;

    // Yaw Correction (-pi to pi flippy plane)
    if(attitudeHistory[pos-1][2]>0 && attitudeHistory[pos][2] < 0) {
        // Positive to negative
        if(attitudeHistory[pos-1][2]>3 && attitudeHistory[pos][2]<-3) {
            // pi to -pi flips
            zvec = glm::dvec2(attitudeHistory[pos-1][2] - (2*M_PI), attitudeHistory[pos][2]);
        } else {
            zvec = glm::dvec2(attitudeHistory[pos-1][2], attitudeHistory[pos][2]);
        }
    } else if (attitudeHistory[pos-1][2]<0 && attitudeHistory[pos][2]>0) {
        // Negative to positive
        if(attitudeHistory[pos-1][2]<-3 && attitudeHistory[pos][2]>3) {
            // -pi to pi flips
            zvec = glm::dvec2(attitudeHistory[pos-1][2] + (2*M_PI), attitudeHistory[pos][2]);
        } else {
            // -0 to 0
            zvec = glm::dvec2(attitudeHistory[pos-1][2], attitudeHistory[pos][2]);
        }
    } else {
        // Same sign
        zvec = glm::dvec2(attitudeHistory[pos-1][2], attitudeHistory[pos][2]);
    }

    xAttConst = xvec*inv;       // Flipped due to GLM ordering
    yAttConst = yvec*inv;       // Flipped due to GLM ordering
    zAttConst = zvec*inv;       // Flipped due to GLM ordering
}

void MavlinkGeoObjectModel::updatePositionAttitude(double glfwTime) {
    // Set new time
    currTime = glfwTime - timeStartPos;

    if(!timePositionHistory.empty()) {
        // Lock data writing
        dataLock.lock();

        // Get minimum difference
        minDiff = std::min(minDiff, timePositionHistory.back() - (currTime + timeStartMavlinkPos - timeDelay));

        // Adjust delay if catching up to real messages
        if(minDiff < 0) {
            timeDelay += timeDelayDelta;
            printf("Incremented time delay. Current Delay: %f\n", timeDelay);
            minDiff = timePositionHistory.back() - (currTime + timeStartMavlinkPos - timeDelay);
        }

        // Check to move to next pair of position messages
        currentPosMsgIndex =
                BufferUtil::findFirstIndexGreaterThan(timePositionHistory, currTime + timeStartMavlinkPos - timeDelay);

        // Check to move to the next pair of attitude messages
        currentAttMsgIndex =
                BufferUtil::findFirstIndexGreaterThan(timeAttitudeHistory, currTime + timeStartMavlinkAtt - timeDelay);

        // Calculate position offset
        if(firstPositionMessage) {
            dtPos = currTime - (timePositionHistory[currentPosMsgIndex] - timeStartMavlinkPos) - timeDelay;
            interpolatePosition();
        }

        // Calculate attitude offset
        if(firstAttitudeMessage) {
            dtAtt = currTime - (timeAttitudeHistory[currentAttMsgIndex] - timeStartMavlinkAtt) - timeDelay;
            interpolateAttitude();
        }

        // Unlock data writing
        dataLock.unlock();
    }
}








