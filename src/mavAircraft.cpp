/*
 * mavAircraft.cpp
 *
 *  Created on: 23Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "mavAircraft.h"


/* Constructor */
MavAircraft::MavAircraft(const GLchar* path, glm::dvec3 origin) : Model(path) {

	// Set Geoposition (temporary)
	this->geoPosition = glm::dvec3(-37.958926f, 145.238343f, 0.0f);
	this->geoPositionHistory.push_back(this->geoPosition);

	// Set Origin
	this->origin = origin;

	// Set Airspeed
	airspeed = 0;
	heading = 0;
}

/* Functions */
void MavAircraft::updatePositionAttitude() {
	// Set new time
	currTime = glfwGetTime() - timeStart;
	if (timePositionHistory.size()>0) {
		minDiff = std::min(minDiff,timePositionHistory.back() - (currTime+timeStartMavlink-timeDelay));

		// Adjust delay if catching up to real messages
		if (minDiff < 0) {
			timeDelay += timeDelay;
			minDiff = timePositionHistory.back() - (currTime+timeStartMavlink-timeDelay);
		}

		// Check to move to next pair of position messages
		std::vector<float>::iterator lowpos = std::lower_bound(timePositionHistory.begin(),timePositionHistory.end(),currTime+timeStartMavlink-timeDelay);
		currentPosMsgIndex = lowpos - timePositionHistory.begin();


		// Check to move to the next pair of attitude messages
		std::vector<float>::iterator lowatt = std::lower_bound(timeAttitudeHistory.begin(),timeAttitudeHistory.end(),currTime+timeStartMavlinkAtt-timeDelay);
		currentAttMsgIndex = lowatt - timeAttitudeHistory.begin();

		// Calculate position offset
		if(!firstPositionMessage) {
			dtPos = currTime - (timePositionHistory[currentPosMsgIndex]-timeStartMavlink) - timeDelay;
			interpolatePosition();
		}

		// Calculate attitude offset
		if(!firstAttitudeMessage) {
			dtAtt = currTime - (timeAttitudeHistory[currentAttMsgIndex]-timeStartMavlinkAtt) - timeDelay;
			interpolateAttitude();
		}
	}
}

void MavAircraft::Draw(Shader shader) {
	if(currentPosMsgIndex>1) {
		// Do Translation and Rotation
		glm::mat4 model;
		model = glm::translate(model,glm::vec3(position[0],position[2],position[1]));// Translate first due to GLM ordering, rotations opposite order
		model = glm::rotate(model,(float)attitude[2],glm::vec3(0.0f,1.0f,0.0f)); // Rotate about y, yaw
		model = glm::rotate(model,(float)attitude[1],glm::vec3(0.0f,0.0f,1.0f)); // Rotate about z, pitch
		model = glm::rotate(model,(float)attitude[0],glm::vec3(1.0f,0.0f,0.0f)); // Rotate about x, roll

		// Update Uniforms
		glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE,glm::value_ptr(model));

		// Draw Model
		Model::Draw(shader);

		// Unlock
		//positionLock.unlock();
	}
}

// Calculate position at next frame
void MavAircraft::interpolatePosition() {
	if(currentPosMsgIndex>1) {
		// Recalculate Interpolation Constants
		calculatePositionInterpolationConstants();

		// Store Past Position
		glm::dvec3 oldPosition = position;

		// Calculate Positions
		this->position[0] = (0.5*xPosConst[0]*dtPos*dtPos) + (xPosConst[1]*dtPos) + xPosConst[2];
		this->position[1] = (0.5*yPosConst[0]*dtPos*dtPos) + (yPosConst[1]*dtPos) + yPosConst[2];
		this->position[2] = (0.5*zPosConst[0]*dtPos*dtPos) + (zPosConst[1]*dtPos) + zPosConst[2];

		// Calculate Velocity
		this->velocity[0] = (position[0] - oldPosition[0])/(-dtPos);
		this->velocity[1] = (position[1] - oldPosition[1])/(-dtPos);
		this->velocity[2] = (position[2] - oldPosition[2])/(-dtPos);

		tempTime.push_back(currTime+timeStartMavlink-timeDelay);
		tempPos.push_back(position);
		tempVel.push_back(velocity);

	}
}

void MavAircraft::interpolateAttitude() {
	if(currentAttMsgIndex>1) {
		// Recalculate Interpolation Constants
		calculateAttitudeInterpolationConstants();

		// Calculate Attitude
		this->attitude[0] = (xAttConst[0]*dtAtt) + xAttConst[1];
		this->attitude[1] = (yAttConst[0]*dtAtt) + yAttConst[1];
		this->attitude[2] = (zAttConst[0]*dtAtt) + zAttConst[1];

		tempTime2.push_back(currTime+timeStartMavlinkAtt-timeDelay);
		tempAtt.push_back(attitude);
	}
}

void MavAircraft::calculatePositionInterpolationConstants() {
	// Get Index
	int pos = currentPosMsgIndex;

	// x(t) = 0.5*a*t^2+b*t+c
	// (t1,x1), (t2,x2), (t3,x3), t2 is current
	float t1 = timePositionHistory[pos-2]-timePositionHistory[pos];
	float t2 = timePositionHistory[pos-1]-timePositionHistory[pos];
	float t3 = 0;

	// Find inverse matrix of [x1,x2,v2]=[BLAH][a,b,c]
	glm::mat3x3 A = glm::mat3x3(0.5*t1*t1, t1, 1,0.5*t2*t2,t2,1,0.5*t3*t3, t3, 1);
	glm::mat3x3 inv = glm::inverse(A);

	// Constants
	glm::dvec3 xvec = glm::dvec3(positionHistory[pos-2][0],positionHistory[pos-1][0],positionHistory[pos][0]);
	glm::dvec3 yvec = glm::dvec3(positionHistory[pos-2][1],positionHistory[pos-1][1],positionHistory[pos][1]);
	glm::dvec3 zvec = glm::dvec3(positionHistory[pos-2][2],positionHistory[pos-1][2],positionHistory[pos][2]);

	xPosConst = xvec*inv;		// Flipped due to GLM ordering
	yPosConst = yvec*inv;		// Flipped due to GLM ordering
	zPosConst = zvec*inv;		// Flipped due to GLM ordering
}

void MavAircraft::calculateAttitudeInterpolationConstants() {
	// Get Index
	int pos = currentAttMsgIndex;

	// x(t) = at+b
	// v(t) = a
	// (t1,x1), (t2,x2), t2 is current
	float t1 = timeAttitudeHistory[pos-1]-timeAttitudeHistory[pos];
	float t2 = 0;

	// Find inverse matrix of [x1,x2]=[BLAH][a,b]
	glm::mat2x2 A = glm::mat2x2(t1,1, t2,1);
	glm::mat2x2 inv = glm::inverse(A);

	// Constants
	glm::dvec2 xvec = glm::dvec2(attitudeHistory[pos-1][0],attitudeHistory[pos][0]);
	glm::dvec2 yvec = glm::dvec2(attitudeHistory[pos-1][1],attitudeHistory[pos][1]);
	glm::dvec2 zvec;
	float PI = 3.1415926535;
	// Yaw Correction (-pi to pi flippy plane)
	if(attitudeHistory[pos-1][2]>0 && attitudeHistory[pos][2]<0) {
		// Positive to negative
		if(attitudeHistory[pos-1][2]>3 && attitudeHistory[pos][2]<-3) {
			// pi to -pi flips
			zvec = glm::dvec2(attitudeHistory[pos-1][2]-(2*PI),attitudeHistory[pos][2]);
		} else {
			// 0 to -0
			zvec = glm::dvec2(attitudeHistory[pos-1][2],attitudeHistory[pos][2]);
		}
	} else if (attitudeHistory[pos-1][2]<0 && attitudeHistory[pos][2]>0) {
		// Negative to positive
		if(attitudeHistory[pos-1][2]<-3 && attitudeHistory[pos][2]>3) {
			// -pi to pi flips
			zvec = glm::dvec2(attitudeHistory[pos-1][2]+(2*PI),attitudeHistory[pos][2]);
		} else {
			// -0 to 0
			zvec = glm::dvec2(attitudeHistory[pos-1][2],attitudeHistory[pos][2]);
		}
	} else {
		// Same sign
		zvec = glm::dvec2(attitudeHistory[pos-1][2],attitudeHistory[pos][2]);
	}

	xAttConst = xvec*inv;		// Flipped due to GLM ordering
	yAttConst = yvec*inv;		// Flipped due to GLM ordering
	zAttConst = zvec*inv;		// Flipped due to GLM ordering
}

/* Conversion Geodetic to ECEF */
glm::dvec3 MavAircraft::geo2ECEF(glm::dvec3 positionVector) {
	// positionVector: (latitude, longitude, altitude (m))
	// Uses WGS84 defined here https://en.wikipedia.org/wiki/Geodetic_datum#Geodetic_to.2Ffrom_ECEF_coordinates
	const double pi = 3.141592653589793238462643383279502884;
	GLdouble a = 6378137.0;
	GLdouble e2 = 6.6943799901377997e-3;
	GLdouble lat = positionVector[0] * pi /180.0;
	GLdouble lon = positionVector[1] * pi /180.0;
	GLdouble alt = positionVector[2];
	GLdouble N = a / sqrt(1-(e2*sin(lat)*sin(lat)));
	GLdouble ex = (N+alt)*cos(lat)*cos(lon); // m
	GLdouble ey = (N+alt)*cos(lat)*sin(lon); // m
	GLdouble ez = (N*(1-e2) + alt) * sin(lat);    // m

	return glm::dvec3(ex,ey,ez);
}

/* Convert from ECEF to NEU */
glm::dvec3 MavAircraft::ecef2NEU(glm::dvec3 ecefVector, glm::dvec3 ecefOrigin, glm::dvec3 origin) {
	GLdouble lat = glm::radians(origin[0]);
	GLdouble lon = glm::radians(origin[1]);
	glm::dmat3 A = glm::dmat3(-sin(lat)*cos(lon),	-sin(lat)*sin(lon),	cos(lat),
							  -sin(lon),			 cos(lon),			0.0,
							  cos(lat)*cos(lon),	 cos(lat)*sin(lon),	sin(lat));
	glm::dvec3 B = glm::dvec3(ecefVector[0]-ecefOrigin[0],ecefVector[1]-ecefOrigin[1],ecefVector[2]-ecefOrigin[2]);

	return B*A; // Flipped due to GLM ordering
}
