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

									// First Message
									if(this->mavAircraftPt->firstMessage) {
										(this->mavAircraftPt)->timeStart = glfwGetTime() + this->mavAircraftPt->timeDelay;
										(this->mavAircraftPt)->timeStartMavlink = packet.time_boot_ms/1000.0;
										printf("Our Start Time: %f, Mavlink Start Time: %f\n",(this->mavAircraftPt)->timeStart,(this->mavAircraftPt)->timeStartMavlink);
									}

									// Store GeoPosition
									glm::dvec3 geoPos = glm::dvec3(packet.lat/1e7,packet.lon/1e7,packet.relative_alt/1e3);
									((this->mavAircraftPt)->geoPositionHistory).push_back(geoPos);
									(this->mavAircraftPt)->geoPosition = geoPos;

									/* Convert Geodetic to ECEF */
									glm::dvec3 ecefPosition = (this->mavAircraftPt)->geo2ECEF((this->mavAircraftPt)->geoPosition);
									glm::dvec3 ecefOrigin = (this->mavAircraftPt)->geo2ECEF((this->mavAircraftPt)->origin);

									/* Convert from ECEF to NEU */
									glm::dvec3 pos = (this->mavAircraftPt)->ecef2NEU(ecefPosition, ecefOrigin, (this->mavAircraftPt)->origin);
									((this->mavAircraftPt)->positionHistory).push_back(glm::dvec3(pos[0],pos[1],pos[2]));
									if(this->mavAircraftPt->firstMessage) {
										this->mavAircraftPt->position = this->mavAircraftPt->positionHistory[0];
									}

									// Store velocities to enforce end position
									if(this->mavAircraftPt->positionHistory.size() > 1) {
										float vx = packet.vx/100.0;
										float vy = packet.vy/100.0;
										float vz = packet.vz/100.0;

										if(abs(vx)>0.1 || abs(vy)>0.1 || abs(vz)>0.1) {
											(this->mavAircraftPt)->velocityHistory.push_back(glm::dvec3(vx,vy,vz));

										} else {
											(this->mavAircraftPt)->velocityHistory.push_back(glm::dvec3(0,0,0));
										}
									} else {
										// Store first position and time
										(this->mavAircraftPt)->position = (this->mavAircraftPt)->positionHistory[0];
										(this->mavAircraftPt)->currTime = glfwGetTime() - (this->mavAircraftPt)->timeStart;
									}

									// Reset old values
									(this->mavAircraftPt)->oldPosition = (this->mavAircraftPt)->position;
									(this->mavAircraftPt)->oldTime = (this->mavAircraftPt)->currTime;

									// Store Time
									((this->mavAircraftPt)->timeHistory).push_back(packet.time_boot_ms/1000.0);

									// Toggle after recieving first message
									if(this->mavAircraftPt->firstMessage) {
										(this->mavAircraftPt)->firstMessage = false;
									}

									break;
								}
								case MAVLINK_MSG_ID_ATTITUDE: {
									mavlink_attitude_t packet;
									mavlink_msg_attitude_decode(&msg,&packet);

									// Rotations later

									break;
								}

							}



							//mavlink_msg_command_long_decode(&msg,&msgstruct);
							//std::cout << msgstruct.param1 << '\n';
						}
					}
				}
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
