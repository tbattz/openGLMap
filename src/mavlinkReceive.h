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

#include <c_library_v2/ardupilotmega/mavlink.h>

using boost::asio::ip::udp;

#define MAV_INCOMING_BUFFER_LENGTH 2041

class MavSocket {
public:
	bool socketRunning = true;
	string host;
	string port;

	MavSocket(string host, string port) {
		this->host = host;
		this->port = port;
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
						std::cout << msg.msgid << "\n";


						//mavlink_command_long_t msgstruct;
						//mavlink_msg_command_long_decode(&msg,&msgstruct);
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
