/*
 * camera.cpp
 *
 *  Created on: 22Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "camera.h"


// Camera Constructor (vectors)
Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
	this->Position = position;
	this->WorldUp = up;
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

// Camera Constructor (scalars)
Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX,GLfloat upY, GLfloat upZ,GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
	this->Position = glm::vec3(posX,posY,posZ);
	this->WorldUp = glm::vec3(upX,upY,upZ);
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

// Calculate View Matrix
glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

// Process Keyboard input
void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
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
void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch) {

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
void Camera::ProcessMouseScroll(GLfloat yoffset) {
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

void Camera::setupView(std::vector<MavAircraft>* mavAircraftList) {
	// Select Aircraft
	MavAircraft* mavAircraftPt = &(*mavAircraftList)[aircraftID];
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
			Pitch = atan2(diffy,dist)*180.0/M_PI;
			Yaw = atan2(diffz,diffx)*180.0/M_PI;

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
			Pitch = atan2(diffy,dist)*180.0/M_PI;
			Yaw = atan2(diffz,diffx)*180.0/M_PI;

			// Update Camera Vectors
			updateCameraVectors();
			break;
		}
		default: {
			printf("Incorrect view selection\n");
		}
	}
}

// Calculate front vector from cameras Euler angles
void Camera::updateCameraVectors() {
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


