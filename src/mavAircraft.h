 /*
 * camera.h
 *
 *  Created on: 22Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef MAVAIRCRAFT_H_
#define MAVAIRCRAFT_H_

// GL Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.h"

// Derived Class
class MavAircraft : public Model {
public:
	/* Data */
		// Position Information
		glm::vec3 geoPosition; 	// Lat (deg), Lon (deg), alt (km)
		glm::vec3 origin; 		// Lat (deg), Lon (deg), alt (km)
		glm::vec3 position; 	// (x,y,z) relative to origin
		glm::vec3 velocity;		// (vx,vy,vz) (m/s)
		glm::vec3 attitude;		//
		// Attitude Information

		// Frame Information
		//GLfloat fovX; // Degrees
		//GLfloat fovY; // Degrees
		// Timekeeping
		float time = glfwGetTime();
		bool newMessage = true;

	MavAircraft(GLchar* path, glm::vec3 origin) : Model(path) {
		// Set Geoposition
		geoPosition = glm::vec3(-37.958926f, 145.238343f, 0.0f);
		this->origin = origin;

		/* Convert Geodetic to ECEF */
		glm::vec3 ecefPosition = geo2ECEF(geoPosition);
		glm::vec3 ecefOrigin = geo2ECEF(origin);

		/* Convert from ECEF to ENU */
		ecef2ENU(ecefPosition, ecefOrigin, this->origin);

	}

	void Draw(Shader shader) {
		// Check if actual data or new message
		if(newMessage) {

			/* Convert Geodetic to ECEF */
			glm::vec3 ecefPosition = geo2ECEF(geoPosition);
			glm::vec3 ecefOrigin = geo2ECEF(origin);

			/* Convert from ECEF to ENU */
			ecef2ENU(ecefPosition, ecefOrigin, this->origin);

			newMessage = false;

		}
		// Extrapolate future position
		float oldtime = time;
		time = glfwGetTime();
		float dt = time - oldtime;
		extrapolatePosition(dt);
		//std::cout << dt << '\n';

		//std::cout << position[0] << ", " << position[1] << ", " << position[2] << '\n';

		// Do Translation and Rotation
		glm::mat4 model;
		model = glm::rotate(model,0.0f,glm::vec3(1.0f,0.0f,0.0f)); // Rotate about x
		model = glm::rotate(model,0.0f,glm::vec3(0.0f,1.0f,0.0f)); // Rotate about y
		model = glm::rotate(model,0.0f,glm::vec3(0.0f,0.0f,1.0f)); // Rotate about z
		model = glm::translate(model,glm::vec3(position[0],position[2],position[1]));

		// Update Uniforms
		glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE,glm::value_ptr(model));

		// Draw Model
		Model::Draw(shader);

	}

	void extrapolatePosition(float dt) {
		// Extrapolates position forward in time
		this->position[0] = position[0] + (velocity[0]*dt);
		this->position[1] = position[1] + (velocity[1]*dt);
		this->position[2] = position[2] + (velocity[2]*dt);
	}

	/* Conversion Geodetic to ECEF */
	glm::vec3 geo2ECEF(glm::vec3 positionVector) {
		// positionVector: (latitude, longitude, altitude (m))
		// Uses WGS84 defined here https://en.wikipedia.org/wiki/Geodetic_datum#Geodetic_to.2Ffrom_ECEF_coordinates
		GLfloat a = 6378137.0;
		GLfloat e2 = 6.69437999014e-3;
		GLfloat lat = glm::radians(positionVector[0]);
		GLfloat lon = glm::radians(positionVector[1]);
		GLfloat alt = glm::radians(positionVector[2]);
		GLfloat N = a / glm::sqrt(1-(e2*glm::pow(glm::sin(lat),2)));
		GLfloat h = positionVector[2]; // Convert to m
		GLfloat ex = (N+h)*glm::cos(lat)*glm::cos(lon); // m
		GLfloat ey = (N+h)*glm::cos(lat)*glm::sin(lon); // m
		GLfloat ez = (N*(1-e2) + h) * glm::sin(lat);    // m

		return glm::vec3(ex,ey,ez);
	}

	/* Convert from ECEF to ENU */
	void ecef2ENU(glm::vec3 ecefVector, glm::vec3 ecefOrigin, glm::vec3 origin) {
		GLfloat lat = glm::radians(origin[0]);
		GLfloat lon = glm::radians(origin[1]);
		GLfloat alt = origin[2];
		glm::mat3 A = glm::mat3(-glm::sin(lon),					glm::cos(lon),					0.0,
								-glm::sin(lat)*glm::cos(lon),	-glm::sin(lat)*glm::sin(lon),	glm::cos(lat),
								glm::cos(lat)*glm::cos(lon),	glm::cos(lat)*glm::sin(lon),	glm::sin(lat));
		glm::vec3 B = glm::vec3(ecefVector[0]-ecefOrigin[0],ecefVector[1]-ecefOrigin[1],ecefVector[2]-ecefOrigin[2]);
		this->position = B*A; // Flipped due to GLM ordering
	}

};



#endif /* MAVAIRCRAFT_H_ */
