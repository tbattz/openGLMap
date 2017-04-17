/*
 * satTiles.h
 *
 *  Created on: 16Apr.,2017
 *      Author: bcub3d-desktop
 */

#ifndef SATTILES_H_
#define SATTILES_H_

// GL Includes
#include "lodepng.h"

#include "../src/shader.h"

// Boost
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include <string>
#include <cmath>
#include <stdlib.h>
#include <thread>
using std::vector;

/* Useful Functions */
vector<int> latLon2TileNum(float lat, float lon, int zoom) {
	// lat (deg), lon (deg)
	float lat_rad = lat * M_PI / 180.0;
	int n = pow(2,zoom);
	int x = floor(n * (lon + 180.0)/360.0);
	int y = floor(n * (1 - (log(tan(lat_rad) + 1.0/cos(lat_rad)) / M_PI)) / 2.0);

	return {x,y};
}

vector<float> tileNum2LatLon(float x, float y, int zoom) {
	int n = pow(2,zoom);
	float lat = atan(sinh(M_PI - (y*2*M_PI/n)))*180.0/M_PI;
	float lon = (x/n)*360.0 - 180.0;

	return {lat, lon};
}

vector<string> splitStringDelim(string inString, string delim) {
	/* Returns a vector of strings split by a deliminator. */
	unsigned int pos1 = 0;
	unsigned int pos2 = inString.find(delim,pos1);
	vector<string> outVec;
	while (pos2 != string::npos) {
		// Found delim
		outVec.push_back(inString.substr(pos1,pos2-pos1));
		pos1 = pos2 + 1;
		pos2 = inString.find(delim,pos1);
		if (pos2 > inString.length()) {
			// Find . for extension
			pos2 = inString.find(".",pos1);
			outVec.push_back(inString.substr(pos1,pos2-pos1));
			break;
		}
	}

	return outVec;
}

/* Tile Classes */
class SatTile {
public:
	/* Data */
	// Position Information
	glm::vec3 geoPosition;	// Lat (deg), Lon (deg), alt (km)
	glm::vec3 origin;		// Lat (deg), Lon (deg), alt (km)
	glm::vec3 position;		// (x,y,z) relative to origin
	// Tile Information
	GLfloat brightness;		// Multipler for brightness
	vector<GLfloat> vertices;
	vector<GLuint> indices;
	GLfloat widthM, heightM;	// Meters
	int x, y, zoom;
	// Buffers
	GLuint VAO, VBO, EBO;
	// Textures
	GLuint tileTexture;
	int width, height;
	string filename;

	/* Functions */
	SatTile(glm::vec3 origin, int x, int y, int zoom , string filename) {
		this->origin = origin;
		this->x = x;
		this->y = y;
		this->zoom = zoom;
		this->filename = filename;
		this->brightness = 1.0;

		/* Calculate geoPosition from x,y,zoom */
		vector<float> geoPos = tileNum2LatLon(x,y,zoom);
		geoPosition = glm::vec3(geoPos[0],geoPos[1],origin[2]);
		printf("%f,%f\n",geoPos[0],geoPos[1]);
		/* Convert Geodetic to ECEF */
		glm::vec3 ecefPosition = geo2ECEF(geoPosition);
		glm::vec3 ecefOrigin = geo2ECEF(origin);

		/* Convert from ECEF to ENU */
		glm::vec3 position2 = ecef2ENU(ecefPosition, ecefOrigin, origin);
		position[0] = 0.0;
		position[1] = 0.0;
		position[2] = 0.0;
		printf("%f,%f,%f\n",position[0],position[1],position[2]);

		/* Calculate Width */
		calcTileWidthHeight(ecefOrigin);
		printf("%f,%f\n",geoPos[0],geoPos[1]);

		/* Calculate Vertices */
		// Tiles geo position is top left corner
		// Image Texture coords start in bottom left
		vertices = {
				// Positions				// Normals			// Texture Coords
				 0.0f,	0.0f, 0.0f,			0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
				 widthM,0.0f, 0.0f,			0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
				 widthM,0.0f, heightM,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
				 0.0f,	0.0f, heightM,		0.0f, 0.0f, 1.0f,	0.0f, 1.0f
		};

		/* Store Indicies */
		this->indices = { // Indices start from zero
					0, 1, 3, // First triangle
					1, 2, 3  // Second triangle
		};

		/* Create and Setup Buffers */
		createAndSetupBuffers();
		/* Load Texture */
		setupTexture();
	}

	void calcTileWidthHeight(glm ::vec3 ecefOrigin) {
		// Calculates the tile width and height in meters
		vector<float> geoBL = tileNum2LatLon(x,y,zoom);
		vector<float> geoTR = tileNum2LatLon(x+1,y+1,zoom);
		glm::vec3 geoPosBL = glm::vec3(geoBL[0],geoBL[1],origin[2]);
		glm::vec3 geoPosTR = glm::vec3(geoTR[0],geoTR[1],origin[2]);
		glm::vec3 ecefBL = geo2ECEF(geoPosBL);
		glm::vec3 ecefTR = geo2ECEF(geoPosTR);
		glm::vec3 posBL = ecef2ENU(ecefBL, ecefOrigin, origin);
		glm::vec3 posTR = ecef2ENU(ecefTR, ecefOrigin, origin);
		glm::vec3 diff = posBL - posTR;
		widthM = fabs(diff[0]);
		heightM = fabs(diff[1]);
	}

	/* Conversion Geodetic to ECEF */
	glm::vec3 geo2ECEF(glm::vec3 positionVector) {
		// positionVector: (latitude, longitude, altitude (m))
		// Uses WGS84 defined here https://en.wikipedia.org/wiki/Geodetic_datum#Geodetic_to.2Ffrom_ECEF_coordinates
		GLfloat a = 6378137.0;
		GLfloat e2 = 6.69437999014e-3;
		GLfloat lat = glm::radians(positionVector[0]);
		GLfloat lon = glm::radians(positionVector[1]);
		//GLfloat alt = glm::radians(positionVector[2]);
		GLfloat N = a / glm::sqrt(1-(e2*glm::pow(glm::sin(lat),2)));
		GLfloat h = positionVector[2]; // Convert to m
		GLfloat ex = (N+h)*glm::cos(lat)*glm::cos(lon); // m
		GLfloat ey = (N+h)*glm::cos(lat)*glm::sin(lon); // m
		GLfloat ez = (N*(1-e2) + h) * glm::sin(lat);    // m

		return glm::vec3(ex,ey,ez);
	}

	/* Convert from ECEF to ENU */
	glm::vec3 ecef2ENU(glm::vec3 ecefVector, glm::vec3 ecefOrigin, glm::vec3 origin) {
		GLfloat lat = glm::radians(origin[0]);
		GLfloat lon = glm::radians(origin[1]);
		//GLfloat alt = origin[2];
		glm::mat3 A = glm::mat3(-glm::sin(lon),					glm::cos(lon),					0.0,
								-glm::sin(lat)*glm::cos(lon),	-glm::sin(lat)*glm::sin(lon),	glm::cos(lat),
								glm::cos(lat)*glm::cos(lon),	glm::cos(lat)*glm::sin(lon),	glm::sin(lat));
		glm::vec3 B = glm::vec3(ecefVector[0]-ecefOrigin[0],ecefVector[1]-ecefOrigin[1],ecefVector[2]-ecefOrigin[2]);
		B*A; // Flipped due to GLM ordering

		return B*A;
	}

	/* Draw Function */
	void Draw(Shader shader) {
		// Calculate new position matrix
		glm::mat4 tilePos;
		tilePos = glm::translate(tilePos, glm::vec3(position[0],0.0f,position[1]));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE,glm::value_ptr(tilePos));
		glUniform1f(glGetUniformLocation(shader.Program,"brightness"),brightness);

		// Bind Texture Units
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.Program,"tileTexture"),0);
		glBindTexture(GL_TEXTURE_2D,tileTexture);

		// Draw tile
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);
		glBindVertexArray(0);

		// Set back to defaults
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,0);
	}

private:
	void createAndSetupBuffers() {
		/* Create Buffers */
		glGenVertexArrays(1,&VAO);
		glGenBuffers(1,&VBO);
		glGenBuffers(1,&EBO);

		/* Setup Buffers */
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat),&vertices[0],GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(GLfloat),&indices[0],GL_STATIC_DRAW);

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
			glGenTextures(1,&tileTexture);
			glBindTexture(GL_TEXTURE_2D,tileTexture);
			// Set parameters
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
			// Texture Filtering
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			// Load texture
			printf("%s\n",filename.c_str());
			unsigned char* image = SOIL_load_image(filename.c_str(),&width,&height,0,SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image);
			glGenerateMipmap(GL_TEXTURE_2D);
			SOIL_free_image_data(image);
			glBindTexture(GL_TEXTURE_2D,0);
		}
};

class SatTileList {
public:
	/* Data */
	vector<SatTile>		tiles;
	vector<string>		tilePaths;
	glm::vec3			origin;				// lat (deg), lon (deg)
	bool				initFin = false;

	/* Threads */
	std::thread satTileLoadThread;
	std::thread satTileDownloaderThread;
	bool		threadRunning = true;

	/* Functions */
	// Initialiser
	SatTileList(glm::vec3 origin) : satTileLoadThread(&SatTileList::satTileLoader,this), satTileDownloaderThread(&SatTileList::satTileDownloader,this) {
		this->origin = origin;

		// Update Tile List
		updateSatTileList("../SatTiles");

		initFin = true;
	}


	void satTileLoader() {
		// The thread to load tiles from disk into the world
		while (!initFin) {
			// Delay until initialisation finished
			std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0)));
		}

		while(threadRunning) {

			// Delay next run
			std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0)));
		}
	}

	void satTileDownloader() {
		// The thread to download the required tiles to the disk
		while (!initFin) {
			// Delay until initialisation finished
			std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0)));
		}

		while (threadRunning) {
			// Delay next run
			std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0)));
		}
	}

	void stopThreads() {
		// Stops the threads
		threadRunning = false;
		satTileLoadThread.join();
		satTileDownloaderThread.join();
	}

	// Update List
	void updateSatTileList(const char* folderPath) {
		/* Checks the folder for any new tiles */
		// Get current file list
		path p(folderPath);
		string newFilename;	// Filename of image file
		string mypath;		// Path of image file relative to src directory

		// Loop over files in directory
		for (auto i = directory_iterator(p); i != directory_iterator(); i++) {
			if (!is_directory(i->path())) { // Ignore subdirectories
				newFilename = i->path().filename().string();
				// Check if this file is already loaded
				string ext = extension(newFilename);
				mypath = "";
				mypath.append(folderPath);
				mypath.append("/");
				mypath.append(newFilename);
				//printf("%s\n",mypath.c_str());
				if((ext.compare(".png"))*(ext.compare(".jpg")) == 0) {
					// Check if file already loaded
					if(std::find(tilePaths.begin(), tilePaths.end(), mypath) == tilePaths.end()) {
						std::cout << "Found unloaded Satellite Tile: " << newFilename << ".\n";
						// Check if file is not in use
						std::fstream myfile;
						myfile.open(mypath);
						if (myfile.is_open()) {
							// Not in previous list, close file
							// Close File
							myfile.close();
							// Parse filename for zoom, row, col, type
							vector<string> split = splitStringDelim(newFilename, "-");
							// Create New Tile
							int zoom = atoi(split[0].c_str());
							int x = atoi(split[1].c_str());
							int y = atoi(split[2].c_str());
							tiles.push_back(SatTile(origin, x, y, zoom, mypath));


							// Add to tile paths
							tilePaths.push_back(mypath);

						} else {
							/* File is probably still being downloaded. */
							std::cout << "Satellite Tile " << newFilename << "is in use. Not adding.\n";
						}
					}
				}
			}
		}
	}

	/* Draw Function */
	void Draw(Shader shader) {
		// Draw tiles
		for(unsigned int i=0; i != tiles.size(); i++) {
			tiles[i].Draw(shader);
		}
	}


	/*
	 *
	 * 	// Global Position
	float lat = -37.955494;	// Deg
	float lon = 145.239152;	// Deg
	int z = 17; 				// Zoom Level

	// Conversion
	float latRad = lat * M_PI / 180;
	int n = pow(2,z);
	float col = n * ((lon + 180)/360);
	float row = n * (1-(log(tan(latRad) + 1/cos(latRad))/M_PI))/2.0;

	printf("n: %i col: %f, row: %f\n",n,col,row);

	//ss << "http://maptile.maps.svc.ovi.com/maptiler/v2/maptile/newest/hybrid.day/" << n << "/" << col << "/" << row << "/256/png8";
//"http://maptile.maps.svc.ovi.com/maptiler/v2/maptile/newest/hybrid.day/17/118415/80493/256/png8"
	 */

};




#endif /* SATTILES_H_ */
