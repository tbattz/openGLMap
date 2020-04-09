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

// Project Includes
#include "mavAircraft.h"
#include <objects/worldObject/WorldObjectController.h>


// Camera Views
#define FREE_CAM 			0
#define TRACKING_CAM		1
#define ONBOARD_FREE		2
#define ONBOARD_CHASE		3

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
const GLfloat SPEED = 70.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;


/* Classes */
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
	// Aircraft
	////std::vector<MavAircraft>* mavAircraftListPt;
	////int aircraftID = 0;
	////int otherAircraftID = 0; // ID to track another aircraft
	std::shared_ptr<IWorldObjectController> currentAircraftController;

	/* Constructors */
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH);
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX,GLfloat upY, GLfloat upZ,GLfloat yaw, GLfloat pitch);

	/* Functions */
	glm::mat4 GetViewMatrix();
	void setCurrentAircraft(std::shared_ptr<IWorldObjectController> currentAircraftController);
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(GLfloat yoffset);
	//void setupView(std::vector<MavAircraft>* mavAircraftList);
    void setupView();

private:
	/* Functions */
	void updateCameraVectors();
};

#endif /* CAMERA_H_ */
