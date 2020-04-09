//
// Created by bcub3d-desktop on 16/3/20.
//


// Standard Includes
#include <utility>
#include <thread>

// Project Includes
#include "MavlinkGeoObjectController.h"




/* Constructor */
MavlinkGeoObjectController::MavlinkGeoObjectController(std::string name, std::string host, std::string port, const GLchar* path, glm::dvec3 geoPosition, glm::dvec3 origin) :
    IWorldObjectController(),
    mavlinkGeoObjectModel(std::move(name),geoPosition, origin),
    worldObjectView(path) {
        this->host = std::move(host);
        this->port = std::move(port);

        threadPt = new std::thread(&MavlinkGeoObjectController::startSocket, this);
}

/* Destructor */
MavlinkGeoObjectController::~MavlinkGeoObjectController() {
    MavlinkGeoObjectController::closeSocket();
}

/* Functions */
glm::dvec3 MavlinkGeoObjectController::getPosition() {
    return this->mavlinkGeoObjectModel.getPosition();
}

glm::dvec3 MavlinkGeoObjectController::getGeoPosition() {
    return this->mavlinkGeoObjectModel.getGeoPosition();
}

void MavlinkGeoObjectController::draw(Shader shader) {
    // Update interpolation
    GLfloat currentTime = glfwGetTime();
    this->mavlinkGeoObjectModel.updatePositionAttitude(currentTime);

    // Get Position and Orientation
    glm::vec3 position = this->mavlinkGeoObjectModel.getPosition();
    glm::vec3 attitude = this->mavlinkGeoObjectModel.getAttitude();

    // Do Translation and Rotation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position[0], position[2], position[1])); // Translate first due to GLM ordering, rotations opposite order
    model = glm::rotate(model,(float)attitude[2],glm::vec3(0.0f,1.0f,0.0f)); // Rotate about y, yaw
    model = glm::rotate(model,(float)attitude[1],glm::vec3(0.0f,0.0f,1.0f)); // Rotate about z, pitch
    model = glm::rotate(model,(float)attitude[0],glm::vec3(1.0f,0.0f,0.0f)); // Rotate about x, roll

    // Update Uniforms
    glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE,glm::value_ptr(model));

    // Draw Model
    this->worldObjectView.Draw(shader);

}

void MavlinkGeoObjectController::startSocket() {
    try {
        // Create the socket to connect to a Mavlink stream
        boost::asio::io_service io_service;
        boost::asio::ip::udp::endpoint localEndpoint = boost::asio::ip::udp::endpoint(
                boost::asio::ip::address::from_string(this->host),
                boost::lexical_cast<int>(this->port));
        std::cout << "Bound socket: " << localEndpoint << std::endl;

        boost::asio::ip::udp::socket socket(io_service,
                    boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), boost::lexical_cast<int>(this->port)));

        // Setup Buffers
        boost::array<char, 128> recvBuf{};
        boost::asio::ip::udp::endpoint senderEndpoint;

        // Receive Mavlink
        while(this->socketRunning) {
            // Receive Data
            size_t len = socket.receive_from(boost::asio::buffer(recvBuf), senderEndpoint);
            // Setup Message Buffers
            mavlink_message_t msg;
            mavlink_status_t status;

            // Parse buffer
            for(size_t i=0; i < len; i++) {
                if(mavlink_parse_char(MAVLINK_COMM_0, recvBuf[i], &msg, &status)) {
                    // Message was received
                    // Lock data for writing
                    this->mavlinkGeoObjectModel.dataLock.lock();
                    // Process message
                    switch(msg.msgid) {
                        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: {
                            MavlinkGeoObjectController::processGlobalPositionMsg(msg);
                            break;
                        }
                        case MAVLINK_MSG_ID_ATTITUDE: {
                            MavlinkGeoObjectController::processAttitudeMsg(msg);
                            break;
                        }
                        case MAVLINK_MSG_ID_VFR_HUD: {
                            MavlinkGeoObjectController::processVFRHudMsg(msg);
                            break;
                        }
                        default: {
                            //std::cout << "Unknown mavlink message type, id: " << msg.msgid << std::endl;
                        }
                    }
                    // Unlock data
                    this->mavlinkGeoObjectModel.dataLock.unlock();
                }
            }
        }

        // Close Socket
        socket.close();

    } catch (std::exception& e) {
        // Error
        std::cerr << e.what() << std::endl;
    }
}

void MavlinkGeoObjectController::closeSocket() {
    this->socketRunning = false;
}

void MavlinkGeoObjectController::processGlobalPositionMsg(mavlink_message_t msg) {
    // Decode packet
    mavlink_global_position_int_t packet;
    mavlink_msg_global_position_int_decode(&msg, &packet);

    // Check for correct data
    glm::dvec3 geoPos = glm::dvec3(packet.lat/1e7, packet.lon/1e7, packet.alt/1e3);
    glm::dvec3 vel = glm::dvec3(packet.vx/100.0, packet.vy/100.0, packet.vz/100.0);
    if(geoPos[0]>=-90 && geoPos[0]<=90 && geoPos[1]>=-180 && geoPos[1]<=180 && geoPos[0]!=0 && geoPos[1]!=0) {
        // Check if first message
        if(!mavlinkGeoObjectModel.getFirstPositionMsgReceived()) {
            mavlinkGeoObjectModel.setStartTimePos(glfwGetTime());
            mavlinkGeoObjectModel.setStartTimePosMavlink(packet.time_boot_ms / 1000.0);
            printf("%s: Our Position Start Time: %f, Mavlink Start Time: %f\n",
                   mavlinkGeoObjectModel.getName().c_str(),
                   mavlinkGeoObjectModel.getStartTimePos(),
                   mavlinkGeoObjectModel.getStartTimePosMavlink());
        }

        // Store GeoPosition
        mavlinkGeoObjectModel.setGeoPosition(geoPos);

        // Store velocities to enforce end position
        mavlinkGeoObjectModel.setVelocity(vel);

        // Store Time
        mavlinkGeoObjectModel.setTimePosition(packet.time_boot_ms/1000.0);

        // Toggle after receiving the first message
        if(!mavlinkGeoObjectModel.getFirstPositionMsgReceived()) {
            mavlinkGeoObjectModel.setFirstPositionMsgReceived(true);
        }
    } else {
        printf("Waiting for correct data or GPS lock.\r");
    }
}

void MavlinkGeoObjectController::processAttitudeMsg(mavlink_message_t msg) {
    // Decode packet
    mavlink_attitude_t packet;
    mavlink_msg_attitude_decode(&msg, &packet);

    // First Message
    if(!mavlinkGeoObjectModel.getFirstAttitudeMsgReceived()) {
        mavlinkGeoObjectModel.setStartTimeAtt(glfwGetTime());
        mavlinkGeoObjectModel.setStartTimeAttMavlink(packet.time_boot_ms/1000.0);
        printf("%s: Our Attitude Start Time: %f, Mavlink Start Time: %f\n",
                mavlinkGeoObjectModel.getName().c_str(),
                mavlinkGeoObjectModel.getStartTimeAtt(),
                mavlinkGeoObjectModel.getStartTimeAttMavlink());
    }

    // Parse data
    glm::dvec3 rot = glm::dvec3(packet.roll, packet.pitch, -packet.yaw);
    glm::dvec3 rotRate = glm::dvec3(packet.rollspeed, packet.pitchspeed, -packet.yawspeed);

    // Store Rotations
    mavlinkGeoObjectModel.setAttitude(rot);

    // Store Rotation Rates
    mavlinkGeoObjectModel.setAttitudeRate(rotRate);

    // Store Time
    mavlinkGeoObjectModel.setTimeAttitude(packet.time_boot_ms/1000.0);

    // Toggle after receiving the first message
    if(!mavlinkGeoObjectModel.getFirstAttitudeMsgReceived()) {
        mavlinkGeoObjectModel.setFirstAttitudeMsgReceived(true);
    }
}

void MavlinkGeoObjectController::processVFRHudMsg(mavlink_message_t msg) {
    // Decode packet
    mavlink_vfr_hud_t packet;
    mavlink_msg_vfr_hud_decode(&msg, &packet);

    // Store airspeed
    mavlinkGeoObjectModel.setAirspeed(packet.airspeed);
    mavlinkGeoObjectModel.setHeading(packet.heading * M_PI / 180.0);

}


