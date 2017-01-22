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

		std::cout << "this is the mavlink thread" << "\n";

		try {
			/* Creates the socket to connect to an Mavlink stream */
			boost::asio::io_service io_service;
			udp::endpoint local_endpoint = boost::asio::ip::udp::endpoint(
			boost::asio::ip::address::from_string(this->host), boost::lexical_cast<int>(this->port));
			std::cout << "Local bind " << local_endpoint << std::endl;

			// Create Socket
			udp::socket socket(io_service, udp::endpoint(udp::v4(), boost::lexical_cast<int>(this->port)));

			// Setup Buffers
			boost::array<char, 128> recv_buf;
			udp::endpoint sender_endpoint;

			GLfloat time = glfwGetTime();
			float timeDiff;
			float mavTime = 0;
			float mavTimeDiff;

			// Receive Mavlink
			while (this->socketRunning) {
				// Receive Data
				size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
				// Setup Message Buffers
				mavlink_message_t msg;
				mavlink_status_t status;

				// Parse buffer
				for(size_t i=0; i < len; i++) {
					if(mavlink_parse_char(MAVLINK_COMM_0, recv_buf[i], &msg, &status)) {
						// Message was recieved
						switch(msg.msgid) {
							case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: {
								mavlink_global_position_int_t packet;
								mavlink_msg_global_position_int_decode(&msg,&packet);
								//std::cout << "POSITION: " << packet.lat/1e7 << ", " << packet.lon/1e7 << ", " << packet.relative_alt  /1000.0 << '\n';
								// Update MavAircraft Information
								if(this->mavAircraftPt!=nullptr) {
									mavTimeDiff = packet.time_boot_ms/1000.0 - mavTime;
									mavTime = packet.time_boot_ms/1000.0;
									timeDiff = glfwGetTime() - time;
									time = glfwGetTime();
									std::cout << "MavTimeDiff: " << mavTimeDiff << ", TimeDiff: " << mavTime << '\n';
									(*mavAircraftPt).geoPosition = glm::vec3(packet.lat/1e7,packet.lon/1e7,packet.relative_alt/1000.0);
									(*mavAircraftPt).velocity = glm::vec3(packet.vx/100.0,packet.vy/100.0,packet.vz/100.0);
									(*mavAircraftPt).newMessage = true;
									(*mavAircraftPt).time = glfwGetTime();
								}
								break;
							}
							case MAVLINK_MSG_ID_ATTITUDE: {
								mavlink_attitude_t packet;
								mavlink_msg_attitude_decode(&msg,&packet);
								//printf("ATTITUDE: %f (%f), %f (%f), %f (%f)\n",packet.roll, packet.rollspeed,packet.pitch, packet.pitchspeed, packet.yaw, packet.yawspeed);
								if(mavAircraftPt!=nullptr) {
									// Rotations later
								}
								break;
							}
						}



						//mavlink_msg_command_long_decode(&msg,&msgstruct);
						//std::cout << msgstruct.param1 << '\n';
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
