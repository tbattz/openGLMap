/*
 * imageTile.h
 *
 *  Created on: 15Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef IMAGETILE_H_
#define IMAGETILE_H_

// GL Includes
#include <GL/glew.h>
#include <SOIL.h>

#include "renderEngine/loadingScreen/loadingScreen.h"

// Standard Includes
#include <iomanip>

// Boost
#include <boost/filesystem.hpp>
using namespace boost::filesystem;


/* Structures */
struct tileTelem {
	string 	timeStr;
	double 	altitude;	// m
	double	longitude;	// Degrees
	double	latitude;	// Degrees
	double	roll; 		// Radians
	double	pitch; 		// Radians
	double	yaw;		// Radians
};

/* Classes */
class ImageTile {
public:
	/* Data */
	// Position Information
	glm::vec3 geoPosition; 	// Lat (deg), Lon (deg), alt (km)
	glm::vec3 origin; 		// Lat (deg), Lon (deg), alt (km)
	glm::vec3 position; 	// (x,y,z) relative to origin
	float altOffset;		// (m) Offset from 0 to stop overlapping tiles flickering
	// Frame Information
	GLfloat fovX; 			// Degrees
	GLfloat fovY; 			// Degrees
	GLfloat brightness; 	// Multiplier for brightness
	// Tile Information
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	// Buffers
	GLuint VAO, VBO, EBO;
	// Textures
	GLuint tileTexture;
	int width, height;
	string filename;

	/* Constructor */
	ImageTile(glm::vec3 origin, glm::vec3 geoPosition, GLfloat fovX, GLfloat fovY, float altOffset, string filename);

	/* Functions */
	glm::vec3 geo2ECEF(glm::vec3 positionVector);
	void ecef2ENU(glm::vec3 ecefVector, glm::vec3 ecefOrigin, glm::vec3 origin);
	void Draw(Shader shader);
	void updateOrigin(glm::vec3 newOrigin);
	void printNEUPosition();
	void printVertices();

private:
	void createAndSetupBuffers();
	void setupTexture();
};

class TileList {
public:
	/* Data */
	std::vector<ImageTile> 	tiles;
	std::vector<string>		tilePaths;
	GLfloat				fovX;
	GLfloat				fovY;
	glm::vec3			origin;
	bool				firstLoad = true;
	GLFWwindow*			window;
	float				currentOffset = 0.0;

	/* Constructor */
	TileList(glm::vec3 origin, GLfloat fovX, GLfloat fovY, GLFWwindow* window);

	/* Functions */
	void updateTileList(const char* folderPath, LoadingScreen* loadingScreenPt);
	void Draw(Shader shader);

private:
	/* Functions */
	void parseTelemFile(std::fstream* myfilePt, tileTelem* tiletelemPt);
};


#endif /* IMAGETILE_H_ */
