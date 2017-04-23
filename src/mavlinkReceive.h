/*
 * mavlinkRecieve.h
 *
 *  Created on: 19Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef MAVLINKRECEIVE_H_
#define MAVLINKRECEIVE_H_

// Boost Includes
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
using boost::asio::ip::udp;

// Mavlink Includes
#include <c_library_v2/ardupilotmega/mavlink.h>
#define MAV_INCOMING_BUFFER_LENGTH 2041

// Project Includes
#include "mavAircraft.h"


class MavSocket {
public:
	bool socketRunning = true;
	string host;
	string port;
	MavAircraft* mavAircraftPt;

	/* Constructor */
	MavSocket(string host, string port, MavAircraft* mavAircraftPt = nullptr);

	/* Functions */
	void startSocket();
	void closeSocket();

};

#endif /* MAVLINKRECEIVE_H_ */
