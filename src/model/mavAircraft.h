 /*
 *	mavAircraft.h
 *
 *  Created on: 22Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef MAVAIRCRAFT_H_
#define MAVAIRCRAFT_H_

// GL Includes
#include <GL/glew.h>

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// Standard Includes
#include <mutex>

// Project Includes
#include "view/fonts.h"
#include "sixDofModel.h"

// Derived Class
class MavAircraft : public SixDofModel {
public:
	/* Data */
	// Name
	std::string name;

	// Position Information
	glm::dvec3 			origin; 						// Lat (deg), Lon (deg), alt (km)
	glm::dvec3 			geoPosition; 					// Lat (deg), Lon (deg), alt (km)
	glm::dvec3 			velocity;						// (vx,vy,vz) (m/s)

	// Position History Information
	vector<glm::dvec3>	geoPositionHistory;				// Vector of Lat (deg), Lon (deg), alt (km)
	vector<glm::dvec3>	positionHistory; 				// Vector of (x,y,z) relative to origin
	vector<glm::dvec3>	velocityHistory;				// Vector of (vx,vy,vz)
	vector<float>		timePositionHistory;			// Vector of floats corresponding to times of position history
	bool				firstPositionMessage = true;	// True if the first message has been received
	unsigned int		currentPosMsgIndex = 0;			// Index of the 'latest' position mavlink message being displayed (this is behind the data)

	// Attitude Information
	vector<glm::dvec3>	attitudeHistory; 				// Vector if attitudes
	vector<glm::dvec3>	attitudeRateHistory;			// Vector of attitude rates
	vector<float>		timeAttitudeHistory;			// Vector of floats corresponding to times of attitude history
	bool				firstAttitudeMessage = true;	// True if the first message has been recieved
	unsigned int		currentAttMsgIndex = 0; 		// Index of the 'latest' attitude mavlink message being displayed (behind the data)

	// Time Information
	float				timeStart=0;					// Offset between autopilot boot time and glfw time (used to sync times)
	float				timeStartMavlink=0; 			// Boot time of the first mavlink message (s)
	float				timeStartAtt=0;
	float				timeStartMavlinkAtt=0;
	float				timeDelay=0.3;  				// Delay between receiving the first mavlink message and displaying it (s)
	float				currTime=0;						// The current time
	float				dtPos=0;						// Timestep between current frame and last current position mavlink message time
	float				dtAtt=0;						// Timestep between current frame and last current attitude mavlink message time
	float				minDiff=10;						// Minimum difference between current sim time and time of latest mavlink message

	// Interpolation Information
	glm::dvec3 			xPosConst;
	glm::dvec3 			yPosConst;
	glm::dvec3 			zPosConst;
	glm::dvec2			xAttConst;
	glm::dvec2			yAttConst;
	glm::dvec2			zAttConst;

	// Airpseed Information
	float 				airspeed;						// (m/s)
	float				heading;						// (rad)

	// Temp stuff
	vector<float> tempTime;
	vector<float> tempTime2;
	vector<glm::dvec3> tempPos;
	vector<glm::dvec3> tempAtt;
	vector<glm::dvec3> tempVel;

	/* Constructor */
	MavAircraft(const GLchar* path, glm::dvec3 origin, string name);

	/* Functions */
	void updatePositionAttitude();
	void interpolatePosition();
	void interpolateAttitude();
	void calculatePositionInterpolationConstants();
	void calculateAttitudeInterpolationConstants();

	/* Conversions */
	glm::dvec3 geo2ECEF(glm::dvec3 positionVector);
	glm::dvec3 ecef2NEU(glm::dvec3 ecefVector, glm::dvec3 ecefOrigin, glm::dvec3 origin);

};



#endif /* MAVAIRCRAFT_H_ */
