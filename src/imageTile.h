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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL.h>

#include "../src/shader.h"
#include "../src/loadingScreen.h"

// Standard Includes
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <stdlib.h>

// Boost
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

class ImageTile;

struct tileTelem {
	string 	timeStr;
	double 	altitude;	// m
	double	longitude;	// Degrees
	double	latitude;	// Degrees
	double	roll; 		// Radians
	double	pitch; 		// Radians
	double	yaw;		// Radians
};


class ImageTile {
public:
	/* Data */
	// Position Information
	glm::vec3 geoPosition; 	// Lat (deg), Lon (deg), alt (km)
	glm::vec3 origin; 		// Lat (deg), Lon (deg), alt (km)
	glm::vec3 position; 	// (x,y,z) relative to origin
	// Frame Information
	GLfloat fovX; // Degrees
	GLfloat fovY; // Degrees
	GLfloat brightness; // Multiplier for brightness
	// Tile Information
	vector<GLfloat> vertices;
	vector<GLuint> indices;
	// Buffers
	GLuint VAO, VBO, EBO;
	// Textures
	GLuint tileTexture;
	int width, height;
	string filename;


	/* Functions */
	ImageTile(glm::vec3 origin, glm::vec3 geoPosition, GLfloat fovX, GLfloat fovY,string filename) {
		/* Instantiates the Image Tile */
		this->origin	= origin;			// Lat (deg), Lon (deg), alt (km)
		this->geoPosition = geoPosition; 	// Lat (deg), Lon (deg), alt (km)
		this->fovX		= fovX;		   		// Degrees
		this->fovY		= fovY;		   		// Degrees
		this->filename 	= filename;			// Name of image file
		this->brightness = 2.0;

		/* Convert Geodetic to ECEF */
		glm::vec3 ecefPosition = geo2ECEF(geoPosition);
		glm::vec3 ecefOrigin = geo2ECEF(origin);

		/* Convert from ECEF to ENU */
		ecef2ENU(ecefPosition, ecefOrigin, this->origin);

		/* Calculate Vertices */
		GLfloat xdiff = (this->geoPosition[2]) * glm::tan(glm::radians(this->fovX/2.0));
		GLfloat ydiff = (this->geoPosition[2]) * glm::tan(glm::radians(this->fovY/2.0));
		vector<GLfloat> vertices = {
			// Positions			 // Normals			// Texture Coords
			-xdiff, 0.0f,	-ydiff, 	 0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
			-xdiff, 0.0f,	+ydiff, 	 0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
			 xdiff, 0.0f,	+ydiff, 	 0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
			 xdiff, 0.0f,	-ydiff, 	 0.0f, 0.0f, 1.0f,	1.0f, 0.0f
		};
		this->vertices = vertices;

		/* Store Indices */
		vector<GLuint> indices = { // Indices start from zero
				0, 1, 3,  // First triangle
				1, 2, 3   // Second triangle
		};
		this->indices = indices;

		/* Create and Setup Buffers */
		createAndSetupBuffers();

		/* Load Texture */
		setupTexture();

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

	/* Draw Function */
	void Draw(Shader shader) {
		// Calculate new position matrix
		glm::mat4 tilePos;
		tilePos = glm::translate(tilePos, glm::vec3(this->position[0],0.0f,this->position[1]));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE,glm::value_ptr(tilePos));
		glUniform1f(glGetUniformLocation(shader.Program,"brightness"),this->brightness);

		// Bind Texture Units
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.Program,"tileTexture"),0);
		glBindTexture(GL_TEXTURE_2D,this->tileTexture);

		// Draw tile
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES,this->indices.size(),GL_UNSIGNED_INT,0);
		glBindVertexArray(0);

		// Set back to defaults
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,0);
	}

	/* Update Origin */
	void updateOrigin(glm::vec3 newOrigin) {

	}

	/* Prints */
	void printNEUPosition() {
		std::cout << this->position[0] << ", " << this->position[1] << ", " << this->position[2] << "\n";
	}

	void printVertices() {
		for(int i=0; i<4; i++) {
			std::cout << "(" << this->vertices[0+(8*i)] << "," << this->vertices[1+(8*i)] << ")\n";
		}
	}


private:
	void createAndSetupBuffers() {
		/* Create Buffers */
		glGenVertexArrays(1,&this->VAO);
		glGenBuffers(1,&this->VBO);
		glGenBuffers(1,&this->EBO);

		/* Setup Buffers */
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER,this->VBO);

		glBufferData(GL_ARRAY_BUFFER, (this->vertices).size()*sizeof(GLfloat),&this->vertices[0],GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,(this->indices).size()*sizeof(GLfloat),&this->indices[0],GL_STATIC_DRAW);

		/* Position Attributes */
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(GLfloat),(GLvoid*)0);
		/* Normal Attributes */
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(GLfloat),(GLvoid*)(3*sizeof(GLfloat)));
		/* TexCoord Attributes */
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(GLfloat),(GLvoid*)(6*sizeof(GLfloat)));


		glBindVertexArray(0); // Unbind VAO
	}

	void setupTexture() {
		// Create Texture
		glGenTextures(1,&(this->tileTexture));
		glBindTexture(GL_TEXTURE_2D,this->tileTexture);
		// Set parameters
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		// Texture Filtering
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		// Load texture
		unsigned char* image = SOIL_load_image(this->filename.c_str(),&this->width,&this->height,0,SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,this->width,this->height,0,GL_RGB,GL_UNSIGNED_BYTE,image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D,0);
	}

};

class TileList {
public:
	/* Data */
	vector<ImageTile> 	tiles;
	vector<string>		tilePaths;
	GLfloat				fovX;
	GLfloat				fovY;
	glm::vec3			origin;
	bool				firstLoad = true;
	GLFWwindow*			window;

	/* Functions */
	// Initialiser
	TileList(glm::vec3 origin, GLfloat fovX, GLfloat fovY, GLFWwindow* window) {
		this->origin = origin;
		this->fovX = fovX;
		this->fovY = fovY;
		this->window = window;

	}

	// Update List
	void updateTileList(const char* folderPath, LoadingScreen* loadingScreenPt) {
		/* Checks the folder for any new tiles */
		// Get current file list
		path p(folderPath);
		string newFilename; // Filename of image file
		string mypath;		// Path of image file relative to src directory
		if (firstLoad) {
			loadingScreenPt->appendLoadingMessage("Starting loading of Image Tiles...");
		}
		for (auto i = directory_iterator(p); i != directory_iterator(); i++) {
			if (!is_directory(i->path())) { // Ignore subdirectories
				newFilename = i->path().filename().string();
				// Check if Image file
				string ext = extension(newFilename);
				mypath = "";
				mypath.append(folderPath);
				mypath.append("/");
				mypath.append(newFilename);
				if ((ext.compare(".png"))*(ext.compare(".jpg")) == 0) {
					// Check if we already have loaded this filename
					if(std::find((this->tilePaths).begin(), (this->tilePaths).end(), mypath) == (this->tilePaths).end()) {
						std::cout << "Found: " << newFilename << ".\n";
						// Check if file is not in use
						std::fstream myfile;
						myfile.open(mypath);
						if (myfile.is_open()) {
							// Not in previous list
							// Close file
							myfile.close();
							// Check if telem file exists
							string mypathTelem;
							mypathTelem.append(folderPath);
							mypathTelem.append("/");
							int lastindex = newFilename.find_last_of(".");
							string objname = newFilename.substr(0,lastindex);
							mypathTelem.append(objname);
							mypathTelem.append("_telem");
							mypathTelem.append(".txt");
							std::fstream mytelemfile;
							std::fstream* mytelemfilePt = &mytelemfile;
							mytelemfile.open(mypathTelem);
							if (mytelemfile.is_open()) {
								// Telem file is safe
								// Add image file
								(this->tilePaths).push_back(mypath);
								std::cout << "Added " << mypath << " to list.\n";

								// Parse Telemetry Data
								tileTelem tiletelem;
								tileTelem* tiletelemPt = &tiletelem;
								this->parseTelemFile(mytelemfilePt,tiletelemPt);

								// Create Position Data
								glm::tvec3<double> geoPosition = {tiletelem.latitude,tiletelem.longitude,tiletelem.altitude};

								// Create New Tile
								this->tiles.push_back(ImageTile(this->origin, geoPosition, this->fovX, this->fovY,mypath.c_str()));

								// Update Loading Screen
								if (firstLoad) {
									loadingScreenPt->appendLoadingMessage("Loaded image Tile " + mypath);
								}

							} else {
								/* File is probably still being downloaded. */
								std::cout << objname << ".txt" << " is in use. Not adding.\n";
							}
						} else {
							/* File is probably still being downloaded. */
							std::cout << newFilename << " is in use. Not adding.\n";
						}
					}
				}
			}
		}
		firstLoad = false;
	}

private:
	void parseTelemFile(std::fstream* myfilePt, tileTelem* tiletelemPt) {
		/* Parses information from the telemetry file */
		string line;
		getline(*myfilePt,line);
		// Time String
		int endPos = line.find(",");
		(*tiletelemPt).timeStr = line.substr(2,endPos-3);
		line = line.substr(endPos+1);
		// Altitude
		endPos = line.find(",");
		(*tiletelemPt).altitude = atof(line.substr(1,endPos-1).c_str());
		line = line.substr(endPos+1);
		// Longitude
		endPos = line.find(",");
		(*tiletelemPt).longitude = atof(line.substr(1,endPos-1).c_str());
		line = line.substr(endPos+1);
		// Latitude
		endPos = line.find(",");
		(*tiletelemPt).latitude = atof(line.substr(1,endPos-1).c_str());
		line = line.substr(endPos+1);
		// Roll
		int brackPos = line.find("[");
		line = line.substr(brackPos);
		endPos = line.find(",");
		(*tiletelemPt).roll = atof(line.substr(1,endPos-1).c_str());
		line = line.substr(endPos+1);
		// Pitch
		endPos = line.find(",");
		(*tiletelemPt).pitch = atof(line.substr(1,endPos-1).c_str());
		line = line.substr(endPos+1);
		// Yaw
		brackPos = line.find("]");
		(*tiletelemPt).yaw = atof(line.substr(1,brackPos-1).c_str());
	}

};


#endif /* IMAGETILE_H_ */
