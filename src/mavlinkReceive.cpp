/*
 * mavlinkReceive.cpp
 *
 *  Created on: 23Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "mavlinkReceive.h"

/* Constructor */
MavSocket::MavSocket(string host, string port, MavAircraft* mavAircraftPt) {
	this->host = host;
	this->port = port;
	this->mavAircraftPt = mavAircraftPt;
}

/* Functions */
void MavSocket::startSocket() {
	try {
		/* Creates the socket to connect to an Mavlink stream */
		boost::asio::io_service io_service;
		udp::endpoint local_endpoint = boost::asio::ip::udp::endpoint(
		boost::asio::ip::address::from_string(this->host), boost::lexical_cast<int>(this->port));
		std::cout << "Bound socket: " << local_endpoint << std::endl;

		// Create Socket
		udp::socket socket(io_service, udp::endpoint(udp::v4(), boost::lexical_cast<int>(this->port)));

		// Setup Buffers
		boost::array<char, 128> recv_buf;
		udp::endpoint sender_endpoint;

		// Receive Mavlink
		while (this->socketRunning) {
			// Receive Data
			size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
			// Setup Message Buffers
			mavlink_message_t msg;
			mavlink_status_t status;

			// Grab Lock
			//mavAircraftPt->positionLock.lock();

			// Parse buffer
			if(this->mavAircraftPt!=nullptr) {
				for(size_t i=0; i < len; i++) {
					if(mavlink_parse_char(MAVLINK_COMM_0, recv_buf[i], &msg, &status)) {
						// Message was recieved
						switch(msg.msgid) {
							case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: {
								// Setup Decoding Packet
								mavlink_global_position_int_t packet;
								mavlink_msg_global_position_int_decode(&msg,&packet);

								// Check for correct data
								glm::dvec3 geoPos = glm::dvec3(packet.lat/1e7,packet.lon/1e7,packet.relative_alt/1e3);
								if(geoPos[0]>=-90 && geoPos[0]<=90 && geoPos[1]>=-180 && geoPos[1]<=180 && geoPos[0]!=0 && geoPos[1]!=0) {

									// First Message
									if(mavAircraftPt->firstPositionMessage) {
										mavAircraftPt->timeStart = glfwGetTime();
										mavAircraftPt->timeStartMavlink = packet.time_boot_ms/1000.0;
										printf("Our Position Start Time: %f, Mavlink Start Time: %f\n",mavAircraftPt->timeStart,mavAircraftPt->timeStartMavlink);
									}

									// Store GeoPosition
									(mavAircraftPt->geoPositionHistory).push_back(geoPos);
									(mavAircraftPt)->geoPosition = geoPos;

									/* Convert Geodetic to ECEF */
									glm::dvec3 ecefPosition = mavAircraftPt->geo2ECEF(mavAircraftPt->geoPosition);
									glm::dvec3 ecefOrigin = mavAircraftPt->geo2ECEF(mavAircraftPt->origin);

									/* Convert from ECEF to NEU */
									glm::dvec3 pos = mavAircraftPt->ecef2NEU(ecefPosition, ecefOrigin, mavAircraftPt->origin);
									mavAircraftPt->positionHistory.push_back(pos);
									if(mavAircraftPt->firstPositionMessage) {
										mavAircraftPt->position = mavAircraftPt->positionHistory[0];
									}

									// Store velocities to enforce end position
									if(mavAircraftPt->positionHistory.size() > 1) {
										glm::dvec3 vel = glm::dvec3(packet.vx/100.0,packet.vy/100.0,packet.vz/100.0);
										mavAircraftPt->velocity = vel;
										mavAircraftPt->velocityHistory.push_back(vel);

									} else {
										// Store first position and time
										mavAircraftPt->position = mavAircraftPt->positionHistory[0];
										mavAircraftPt->currTime = glfwGetTime() - mavAircraftPt->timeStart;
									}

									// Store Time
									mavAircraftPt->timePositionHistory.push_back(packet.time_boot_ms/1000.0);

									// Toggle after recieving first message
									if(mavAircraftPt->firstPositionMessage) {
										(mavAircraftPt)->firstPositionMessage = false;
									}
								} else {
									printf("Waiting for correct data or GPS lock.\r");
								}

								break;
							}
							case MAVLINK_MSG_ID_ATTITUDE: {
								mavlink_attitude_t packet;
								mavlink_msg_attitude_decode(&msg,&packet);

								// First Message
								if(mavAircraftPt->firstAttitudeMessage) {
									(mavAircraftPt)->timeStartAtt = glfwGetTime();
									(mavAircraftPt)->timeStartMavlinkAtt = packet.time_boot_ms/1000.0;
									printf("Our Attitude Start Time: %f, Mavlink Start Time: %f\n",mavAircraftPt->timeStartAtt,mavAircraftPt->timeStartMavlinkAtt);
								}

								// Store Rotations
								glm::vec3 rot = glm::vec3(packet.roll,packet.pitch,-packet.yaw);
								mavAircraftPt->attitude = rot;
								mavAircraftPt->attitudeHistory.push_back(rot);

								// Store Rotation Rates
								glm::dvec3 rotRate = glm::dvec3(packet.rollspeed,packet.pitchspeed,-packet.yawspeed);
								mavAircraftPt->attitudeRateHistory.push_back(rotRate);
								// Store Time
								mavAircraftPt->timeAttitudeHistory.push_back(packet.time_boot_ms/1000.0);

								// Reset First Message Switch
								if(mavAircraftPt->firstAttitudeMessage) {
									mavAircraftPt->firstAttitudeMessage = false;
								}

								break;
							}
							case MAVLINK_MSG_ID_VFR_HUD: {
								mavlink_vfr_hud_t packet;
								mavlink_msg_vfr_hud_decode(&msg,&packet);

								// Store airspeed
								mavAircraftPt->airspeed = packet.airspeed;
								mavAircraftPt->heading = packet.heading * M_PI / 180.0;
							}

						}



					}
				}
			}
			// Release Lock
			//mavAircraftPt->positionLock.unlock();

		}
		// Close Socket
		socket.close();


	}  catch (std::exception& e)  {
		// Error
			std::cerr << e.what() << std::endl;
	}
}

void MavSocket::closeSocket() {
	this->socketRunning = false;
}
