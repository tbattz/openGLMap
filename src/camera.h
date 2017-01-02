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

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Options for camera movement
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;

// Camera class
class Camera {
public:
	// Camera attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	GLfloat Yaw;
	GLfloat Pitch;
	// Camera Options
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;

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
			this->Zoom -= yoffset;
		}
		if(this->Zoom <= 1.0f) {
			this->Zoom = 1.0f;
		}
		if(this->Zoom >= 45.0f) {
			this->Zoom = 45.0f;
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
