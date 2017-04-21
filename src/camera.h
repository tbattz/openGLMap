 /*
 * camera.h
 *
 *  Created on: 1Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef CAMERA_H_
#define CAMERA_H_

// Standard Includes
#include <vector>
#include <iostream>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../src/mavAircraft.h"

// Options for camera movement
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Camera Views
#define FREE_CAM 			0
#define TRACKING_CAM		1
#define ONBOARD_FREE		2
#define ONBOARD_CHASE		3

// PI
#define PI 3.14159265

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 70.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;

// Camera class
class Camera {
public:
	// Camera attributes
	glm::vec3 Position; // y Up, x North, z, East
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	GLfloat Yaw; // Deg
	GLfloat Pitch; // Deg
	// Camera Options
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;
	// Camera Number
	int view = 0;

	// Camera Constructor (vectors)
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}

	// Camera Constructor (scalars)
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX,GLfloat upY, GLfloat upZ,GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
		this->Position = glm::vec3(posX,posY,posZ);
		this->WorldUp = glm::vec3(upX,upY,upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}

	// Calculate View Matrix
	glm::mat4 GetViewMatrix() {
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}

	// Process Keyboard input
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
		GLfloat velocity = this->MovementSpeed * deltaTime;
		if(direction == FORWARD) {
			this->Position += this->Front * velocity;
		}
		if(direction == BACKWARD) {
			this->Position -= this->Front * velocity;
		}
		if(direction == LEFT) {
			this->Position -= this->Right * velocity;
		}
		if(direction == RIGHT) {
			this->Position += this->Right * velocity;
		}
	}

	// Process Mouse Input
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) {

		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw += xoffset;
		this->Pitch += yoffset;

		// Fix out of bound pitch
		if(constrainPitch) {
			if(this->Pitch > 89.9f) {
				this->Pitch = 89.9f;
			}
			if(this->Pitch < -89.9f) {
				this->Pitch = -89.9f;
			}
		}

		// Update Front, Right, Up Vectors using Euler Angles
		this->updateCameraVectors();
	}

	// Process Scroll Input
	void ProcessMouseScroll(GLfloat yoffset) {
		if(this->Zoom >= 1.0f && this->Zoom <= 45.0f) {
			this->Zoom -= yoffset/10.0;
		}
		if(this->Zoom <= 1.0f) {
			this->Zoom = 1.0f;
		}
		if(this->Zoom >= 45.0f) {
			this->Zoom = 45.0f;
		}
	}

	void setupView(MavAircraft* mavAircraftPt) {
		// Sets up the selected view
		switch(view) {
		case FREE_CAM: {
			// Do nothing
			break;
		};
		case TRACKING_CAM: {
			// Change view angles
			// Get vector
			float diffx = mavAircraftPt->position[0] - Position.x;
			float diffy = mavAircraftPt->position[2] - Position.y;
			float diffz = mavAircraftPt->position[1] - Position.z;

			// Update Angle
			float dist = sqrt((diffx*diffx)+(diffz*diffz));
			Pitch = atan2(diffy,dist)*180.0/PI;
			Yaw = atan2(diffz,diffx)*180.0/PI;

			// Update Camera Vectors
			updateCameraVectors();
			break;
		}
		case ONBOARD_FREE: {
			// Offset position below aircraft
			Position = glm::vec3(mavAircraftPt->position[0],mavAircraftPt->position[2]-0.5,mavAircraftPt->position[1]);
			break;
		}
		case ONBOARD_CHASE: {
			// Calculate velocity vector
			glm::vec3 v2 = mavAircraftPt->velocity;
			glm::vec3 unitv = v2/glm::length(v2);

			// Get offset position
			float xpos = mavAircraftPt->position[0] - (unitv[0]*5.0);
			float ypos = mavAircraftPt->position[2] - (unitv[2]*5.0) + 2.0;
			float zpos = mavAircraftPt->position[1] - (unitv[1]*5.0);

			// Set Position
			Position = glm::vec3(xpos,ypos,zpos);

			// Get difference vector
			float diffx = mavAircraftPt->position[0] - Position.x;
			float diffy = mavAircraftPt->position[2] - Position.y;
			float diffz = mavAircraftPt->position[1] - Position.z;

			// Update Angle
			float dist = sqrt((diffx*diffx)+(diffz*diffz));
			Pitch = atan2(diffy,dist)*180.0/PI;
			Yaw = atan2(diffz,diffx)*180.0/PI;

			// Update Camera Vectors
			updateCameraVectors();
			break;
		}
		default: {
			printf("Incorrect view selection\n");
		}
		}
	}

private:
	// Calculate front vector from cameras Euler angles
	void updateCameraVectors() {
		// Update Front Vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);
		// Recalculate Right and Up Vectors
		this->Right = glm::normalize(glm::cross(this->Front,this->WorldUp));
		this->Up = glm::normalize(glm::cross(this->Right,this->Front));
	}
};

#endif /* CAMERA_H_ */
