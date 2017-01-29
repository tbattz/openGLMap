/*
 * mavlinkRecieve.h
 *
 *  Created on: 19Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef MAVLINKRECEIVE_H_
#define MAVLINKRECEIVE_H_

#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <glm/glm.hpp>

#include <c_library_v2/ardupilotmega/mavlink.h>

#include "../src/mavAircraft.h"

using boost::asio::ip::udp;

#define MAV_INCOMING_BUFFER_LENGTH 2041

class MavSocket {
public:
	bool socketRunning = true;
	string host;
	string port;
	MavAircraft* mavAircraftPt;

	MavSocket(string host, string port, MavAircraft* mavAircraftPt = nullptr) {
		this->host = host;
		this->port = port;
		this->mavAircraftPt = mavAircraftPt;
	}

	void startSocket() {

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
				mavAircraftPt->positionLock.lock();

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
										if(this->mavAircraftPt->firstPositionMessage) {
											(this->mavAircraftPt)->timeStart = glfwGetTime() + this->mavAircraftPt->timeDelay;
											(this->mavAircraftPt)->timeStartMavlink = packet.time_boot_ms/1000.0;
											printf("Our Start Time: %f, Mavlink Start Time: %f\n",(this->mavAircraftPt)->timeStart,(this->mavAircraftPt)->timeStartMavlink);
										}

										// Store GeoPosition
										((this->mavAircraftPt)->geoPositionHistory).push_back(geoPos);
										(this->mavAircraftPt)->geoPosition = geoPos;

										/* Convert Geodetic to ECEF */
										glm::dvec3 ecefPosition = (this->mavAircraftPt)->geo2ECEF((this->mavAircraftPt)->geoPosition);
										glm::dvec3 ecefOrigin = (this->mavAircraftPt)->geo2ECEF((this->mavAircraftPt)->origin);

										/* Convert from ECEF to NEU */
										glm::dvec3 pos = (this->mavAircraftPt)->ecef2NEU(ecefPosition, ecefOrigin, (this->mavAircraftPt)->origin);
										((this->mavAircraftPt)->positionHistory).push_back(pos);
										if(this->mavAircraftPt->firstPositionMessage) {
											this->mavAircraftPt->position = this->mavAircraftPt->positionHistory[0];
										}

										// Store velocities to enforce end position
										if(this->mavAircraftPt->positionHistory.size() > 1) {
											(this->mavAircraftPt)->velocityHistory.push_back(glm::dvec3(packet.vx/100.0,packet.vy/100.0,packet.vz/100.0));
										} else {
											// Store first position and time
											(this->mavAircraftPt)->position = (this->mavAircraftPt)->positionHistory[0];
											(this->mavAircraftPt)->currTime = glfwGetTime() - (this->mavAircraftPt)->timeStart;
										}

										// Store Time
										((this->mavAircraftPt)->timePositionHistory).push_back(packet.time_boot_ms/1000.0);

										// Toggle after recieving first message
										if(this->mavAircraftPt->firstPositionMessage) {
											(this->mavAircraftPt)->firstPositionMessage = false;
										}
									} else {
										printf("Waiting for correct data or GPS lock.\r");
									}

									break;
								}
								case MAVLINK_MSG_ID_ATTITUDE: {
									mavlink_attitude_t packet;
									mavlink_msg_attitude_decode(&msg,&packet);

									// Store Rotations
									glm::dvec3 rot = glm::dvec3(packet.roll,packet.pitch,-packet.yaw);
									this->mavAircraftPt->attitude = rot;
									this->mavAircraftPt->attitudeHistory.push_back(rot);
									// Store Rotation Rates
									glm::dvec3 rotRate = glm::dvec3(packet.rollspeed,packet.pitchspeed,packet.yawspeed);
									this->mavAircraftPt->attitudeRateHistory.push_back(rotRate);
									// Store Time
									this->mavAircraftPt->timeAttitudeHistory.push_back(packet.time_boot_ms/1000.0);

									// Reset First Message Switch
									if(this->mavAircraftPt->firstAttitudeMessage) {
										this->mavAircraftPt->firstAttitudeMessage = false;
									}

									break;
								}
								case MAVLINK_MSG_ID_VFR_HUD: {
									mavlink_vfr_hud_t packet;
									mavlink_msg_vfr_hud_decode(&msg,&packet);

									// Store airspeed
									mavAircraftPt->airspeed = packet.airspeed;
								}

							}



						}
					}
				}
				// Release Lock
				mavAircraftPt->positionLock.unlock();

			}
			// Close Socket
			socket.close();


		}  catch (std::exception& e)  {
			// Error
				std::cerr << e.what() << std::endl;
		}
	}

	void closeSocket() {
		this->socketRunning = false;
	}
};

#endif /* MAVLINKRECEIVE_H_ */
