/*
 * satTiles.h
 *
 *  Created on: 16Apr.,2017
 *      Author: bcub3d-desktop
 */

#ifndef SATTILES_H_
#define SATTILES_H_


#include "../src/shader.h"

// Boost
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include <string>
#include <cmath>
#include <stdlib.h>
#include <thread>
#include <algorithm>
using std::vector;

/* Useful Functions */
vector<float> latLon2TileNum(float lat, float lon, int zoom) {
	// lat (deg), lon (deg)
	float lat_rad = lat * M_PI / 180.0;
	int n = pow(2,zoom);
	float x = n * (lon + 180.0) / 360.0;
	float y = n * (1 - (log(tan(lat_rad) + 1.0/cos(lat_rad)) / M_PI)) / 2.0;

	return {x,y};
}

vector<double> tileNum2LatLon(float x, float y, int zoom) {
	int n = pow(2,zoom);
	double lat = atan(sinh(M_PI - (y*2*M_PI/n)))*180.0/M_PI;
	double lon = (x/n)*360.0 - 180.0;

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

vector<float> latLonOffsetHeading(float lat1, float lon1, float distance, float bearing, float sphereRadius = 6378.137) {
	// Gives the lat, lon of a point at a given distance and bearing from another lat, lon point.
	// lat1:		deg
	// lon1:		deg
	// distance:	km
	// bearing:		deg
	// sphereRadius: km (Default Earth Radius)

	// Convert to radius
	float latR = lat1 * M_PI / 180.0;
	float lonR = lon1 * M_PI / 180.0;
	float bearingR = bearing * M_PI / 180.0;

	float d_r = distance/sphereRadius; // d/r
	float lat2R = asin((sin(latR)*cos(d_r)) + (cos(latR)*sin(d_r)*cos(bearingR)));
	float lon2R = lonR + atan2(sin(bearingR)*sin(d_r)*cos(latR),cos(d_r)-(sin(latR)*sin(lat2R)));

	// Convert back to degrees
	float lat2 = lat2R * 180.0 / M_PI;
	float lon2 = lon2R * 180.0 / M_PI;

	return {lat2,lon2};
}

/* Tile Classes */
class SatTile {
public:
	/* Data */
	// Position Information
	glm::dvec3 geoPosition;	// Lat (deg), Lon (deg), alt (km)
	glm::dvec3 origin;		// Lat (deg), Lon (deg), alt (km)
	glm::dvec3 position;		// (x,y,z) relative to origin
	// Tile Information
	GLfloat brightness;		// Multipler for brightness
	vector<GLfloat> vertices;
	vector<GLuint> indices;
	vector<vector<double>> xyOff;	// Meters, (wTR,hTR,wBL,hBL,wBR,hBR)
	int x, y, zoom;
	// Buffers
	GLuint VAO, VBO, EBO;
	// Textures
	GLuint tileTexture;
	int width, height;
	string filename;

	/* Functions */
	SatTile(glm::vec3 origin, int x, int y, int zoom, string filename) {
		this->origin = origin;
		this->x = x;
		this->y = y;
		this->zoom = zoom;
		this->filename = filename;
		this->brightness = 1.0;

		/* Calculate geoPosition from x,y,zoom */
		vector<double> geoPos = tileNum2LatLon(x,y,zoom);
		geoPosition = glm::dvec3(geoPos[0],geoPos[1],0.0);
		/* Convert Geodetic to ECEF */
		glm::dvec3 ecefPosition = geo2ECEF(geoPosition);
		glm::dvec3 ecefOrigin = geo2ECEF(origin);

		/* Convert from ECEF to ENU */
		glm::dvec3 tempPos = ecef2ENU(ecefPosition, ecefOrigin, origin);
		position = glm::dvec3(tempPos[1],tempPos[0],tempPos[2]);

		/* Calculate Width */
		calcTileWidthHeightAll(ecefOrigin);

		/* Calculate Vertices */
		// Tiles geo position is top left corner
		// Image Texture coords start in bottom left, then rotated by 90 deg (for png)
		vertices = {
				// Positions				// Normals			// Texture Coords
				 -xyOff[1][0],	0.0f, xyOff[1][1],			0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
				 -xyOff[2][0],	0.0f, xyOff[2][1],			0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
				 -xyOff[0][0],	0.0f, xyOff[0][1],			0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
				  0.0f,			0.0f, 0.0f,					0.0f, 0.0f, 1.0f,	0.0f, 0.0f
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

	vector<double> calcTileWidthHeight(glm::dvec3 ecefOrigin, glm::dvec3 geoPos1, glm::dvec3 geoPos2) {
		// Calculates the width and height offsets between two geo positions in (x m, y m)
		glm::dvec3 ecefPos1 = geo2ECEF(geoPos1);
		glm::dvec3 ecefPos2 = geo2ECEF(geoPos2);
		glm::dvec3 pos1 = ecef2ENU(ecefPos1, ecefOrigin, origin);
		glm::dvec3 pos2 = ecef2ENU(ecefPos2, ecefOrigin, origin);
		glm::dvec3 diff = pos1 - pos2;

		double h = fabs(diff[0])+0.05;
		double w = fabs(diff[1])+0.05;

		return {w, h};
	}

	void calcTileWidthHeightAll(glm::dvec3 ecefOrigin) {
		// Calculates the tile width and height in meters
		vector<double> geoTL = tileNum2LatLon(x,y,zoom);
		vector<double> geoTR = tileNum2LatLon(x+1,y,zoom);
		vector<double> geoBL = tileNum2LatLon(x,y+1,zoom);
		vector<double> geoBR = tileNum2LatLon(x+1,y+1,zoom);
		// Convert
		glm::dvec3 geoPosTL = glm::dvec3(geoTL[0],geoTL[1],0.0f);
		glm::dvec3 geoPosTR = glm::dvec3(geoTR[0],geoTR[1],0.0f);
		glm::dvec3 geoPosBL = glm::dvec3(geoBL[0],geoBL[1],0.0f);
		glm::dvec3 geoPosBR = glm::dvec3(geoBR[0],geoBR[1],0.0f);
		// Get width height differences
		vector<double> diffsTLTR = calcTileWidthHeight(ecefOrigin, geoPosTL, geoPosTR);
		vector<double> diffsTLBL = calcTileWidthHeight(ecefOrigin, geoPosTL, geoPosBL);
		vector<double> diffsTLBR = calcTileWidthHeight(ecefOrigin, geoPosTL, geoPosBR);
		// Store differences
		xyOff = {};
		xyOff.push_back(diffsTLTR);
		xyOff.push_back(diffsTLBL);
		xyOff.push_back(diffsTLBR);
	}

	/* Conversion Geodetic to ECEF */
	glm::dvec3 geo2ECEF(glm::dvec3 positionVector) {
		// positionVector: (latitude, longitude, altitude (m))
		// Uses WGS84 defined here https://en.wikipedia.org/wiki/Geodetic_datum#Geodetic_to.2Ffrom_ECEF_coordinates
		GLdouble a = 6378137.0;
		GLdouble e2 = 6.69437999014e-3;
		GLdouble lat = glm::radians(positionVector[0]);
		GLdouble lon = glm::radians(positionVector[1]);
		//GLdouble alt = glm::radians(positionVector[2]);
		GLdouble N = a / glm::sqrt(1-(e2*glm::pow(glm::sin(lat),2)));
		GLdouble h = positionVector[2]; // Convert to m
		GLdouble ex = (N+h)*glm::cos(lat)*glm::cos(lon); // m
		GLdouble ey = (N+h)*glm::cos(lat)*glm::sin(lon); // m
		GLdouble ez = (N*(1-e2) + h) * glm::sin(lat);    // m

		return glm::dvec3(ex,ey,ez);
	}

	/* Convert from ECEF to ENU */
	glm::dvec3 ecef2ENU(glm::dvec3 ecefVector, glm::dvec3 ecefOrigin, glm::dvec3 origin) {
		GLdouble lat = glm::radians(origin[0]);
		GLdouble lon = glm::radians(origin[1]);
		//GLfloat alt = origin[2];
		glm::mat3 A = glm::mat3(-glm::sin(lon),					glm::cos(lon),					0.0,
								-glm::sin(lat)*glm::cos(lon),	-glm::sin(lat)*glm::sin(lon),	glm::cos(lat),
								glm::cos(lat)*glm::cos(lon),	glm::cos(lat)*glm::sin(lon),	glm::sin(lat));
		glm::dvec3 B = glm::dvec3(ecefVector[0]-ecefOrigin[0],ecefVector[1]-ecefOrigin[1],ecefVector[2]-ecefOrigin[2]);
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
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			// Texture Filtering
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			// Load texture
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
	int					zoom = 18;			// Zoom level
	vector<vector<int>> requiredTiles;
	vector<vector<int>> diskTiles;
	vector<vector<int>> loadedTiles;
	const char* 		folderPath = "../SatTiles/";
	bool				initFin = false;

	/* Aircraft Data */
	MavAircraft* mavAircraftPt;
	glm::dvec3 aircraftGeoPos = glm::dvec3(0.0f,0.0f,0.0f);
	float aircraftRadius = 500;		// metres

	/* Threads */
	std::thread satTileLoadThread;
	std::thread satTileDownloaderThread;
	bool		threadRunning = true;

	/* Functions */
	// Initialiser
	SatTileList(glm::vec3 origin, MavAircraft* mavAircraftPt = NULL) : satTileLoadThread(&SatTileList::satTileLoader,this), satTileDownloaderThread(&SatTileList::satTileDownloader,this) {
		this->origin = origin;
		this->mavAircraftPt = mavAircraftPt;

		// Start Threads
		initFin = true;


	}


	void satTileLoader() {
		// The thread to load tiles from disk into the world
		while (!initFin) {
			// Delay until initialisation finished
			std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0)));
		}

		while(threadRunning) {
			// Update set of disk tiles
			updateDiskTiles(folderPath);

			// Get new set of required tiles
			updateRequiredTiles();

			// Load required tiles from disk
			//loadRequiredTilesFromDisk();

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
			// Download required tiles
			for(unsigned int i=0; i<requiredTiles.size(); i++) {
				string outName = "../SatTiles/" + std::to_string(zoom) + "-" + std::to_string(requiredTiles[i][0]) + "-" + std::to_string(requiredTiles[i][1]) + ".png";
				//system(("wget http://maptile.maps.svc.ovi.com/maptiler/v2/maptile/newest/hybrid.day/" + std::to_string(zoom) +"/" + std::to_string(requiredTiles[i][0]) + "/" + std::to_string(requiredTiles[i][1]) +"/256/png8 -O " + outName.c_str()).c_str());
				std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0)));
			}



			// Delay next run
			std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0)));
		}
	}

	void loadRequiredTilesFromDisk() {
		// Loads the tiles that from the disk that are required
		if(requiredTiles.size()>0) {
			for(unsigned int i=0; i<requiredTiles.size(); i++) {
				// If tile is not loaded
				if (!(std::find(loadedTiles.begin(), loadedTiles.end(), requiredTiles[i]) != loadedTiles.end())) {
					// If tile is on disk
					if(std::find(diskTiles.begin(), diskTiles.end(), requiredTiles[i]) != diskTiles.end()) {
						// Vector not in vector
						int x = requiredTiles[i][0];
						int y = requiredTiles[i][1];
						string xStr = std::to_string(x);
						string yStr = std::to_string(y);
						string zoomStr = std::to_string(zoom);
						string mypath;
						mypath = "";
						mypath.append(folderPath);
						//mypath.append(zoomStr+"-"+xStr+"-"+yStr+".png");
						mypath.append("18-236831-160989.png");
						printf("%s\n",mypath.c_str());
						// Load Tile
						tiles.push_back(SatTile(origin, x, y, zoom, mypath));
						loadedTiles.push_back({x,y,zoom});
						printf("Loaded Tile %s.\n",(zoom+"-"+xStr+"-"+yStr).c_str());
					}
				}
			}
		}
	}

	// Update List
	void updateTileList(const char* folderPath, LoadingScreen* loadingScreenPt) {
		/* Checks the folder for any new tiles */
		// Get current file list
		path p(folderPath);
		string newFilename; // Filename of image file
		string mypath;		// Path of image file relative to src directory
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
					if(std::find(tilePaths.begin(), tilePaths.end(), mypath) == tilePaths.end()) {
						std::cout << "Found: " << newFilename << ".\n";
						// Check if file is not in use
						std::fstream myfile;
						myfile.open(mypath);
						if (myfile.is_open()) {
							// Not in previous list
							// Close file
							myfile.close();
							vector<string> split = splitStringDelim(newFilename, "-");
							int zoom = atoi(split[0].c_str());
							int x = atoi(split[1].c_str());
							int y = atoi(split[2].c_str());
							string xStr = std::to_string(x);
							string yStr = std::to_string(y);
							string zoomStr = std::to_string(zoom);
							string mypath;
							mypath = "";
							mypath.append(folderPath);
							mypath.append(zoomStr+"-"+xStr+"-"+yStr+".png");
							printf("%s\n",mypath.c_str());
							// Create New Tile
							this->tiles.push_back(SatTile(origin, x, y, zoom, mypath.c_str()));


						} else {
							/* File is probably still being downloaded. */
							std::cout << newFilename << " is in use. Not adding.\n";
						}
					}
				}
			}
		}
	}


	void updateRequiredTiles() {
		// Calculates the tiles required at the current point in time
		// Get current tile below aircraft
		if (mavAircraftPt != NULL) {
			aircraftGeoPos = mavAircraftPt->geoPosition;
		} else {
			aircraftGeoPos = origin;
		}
		vector<float> currTiles = latLon2TileNum(aircraftGeoPos[0], aircraftGeoPos[1], zoom);
		// Find tiles within distance based on aircraft position
		vector<float> latLonT = latLonOffsetHeading(aircraftGeoPos[0], aircraftGeoPos[1], aircraftRadius/1000.0, 0);
		vector<float> latLonR = latLonOffsetHeading(aircraftGeoPos[0], aircraftGeoPos[1], aircraftRadius/1000.0, 90);
		vector<float> latLonB = latLonOffsetHeading(aircraftGeoPos[0], aircraftGeoPos[1], aircraftRadius/1000.0, 180);
		vector<float> latLonL = latLonOffsetHeading(aircraftGeoPos[0], aircraftGeoPos[1], aircraftRadius/1000.0, 270);
		vector<float> tilesT = latLon2TileNum(latLonT[0], latLonT[1], zoom);
		vector<float> tilesR = latLon2TileNum(latLonR[0], latLonR[1], zoom);
		vector<float> tilesB = latLon2TileNum(latLonB[0], latLonB[1], zoom);
		vector<float> tilesL = latLon2TileNum(latLonL[0], latLonL[1], zoom);
		int xmin = std::min(tilesL[0],tilesR[0]);
		int xmax = std::max(tilesL[0],tilesR[0]);
		int ymin = std::min(tilesT[1],tilesB[1]);
		int ymax = std::max(tilesT[1],tilesB[1]);
		// Spiral outwards
		int edge = std::max(xmax-xmin,ymax-ymin);
		if (edge % 2 == 0) {
			edge += 1;
		}
		// Generate in a spiral pattern
		int maxSideLen = edge - 1; // maxSideLen x maxSideLen grid
		vector<vector<int>> tileRowCol;
		vector<int> centerTiles = {(int)currTiles[0],(int)currTiles[1],zoom};
		vector<int> oldTile = centerTiles;
		tileRowCol.push_back(centerTiles);
		int sign = 1;
		for(int i=1; i<maxSideLen+1; i++) {
			// Get new tiles - x leg
			for(int j=1; j<i+1; j++) {
				vector<int> newTile = {oldTile[0]+sign,oldTile[1],zoom};
				oldTile = newTile;
				tileRowCol.push_back(newTile);
			}
			// Get new tiles - y leg
			for(int j=1; j<i+1; j++) {
				vector<int> newTile = {oldTile[0],oldTile[1]+sign,zoom};
				oldTile = newTile;
				tileRowCol.push_back(newTile);
			}
			// Flip sign
			sign = sign * -1;

			// Remove elements from list that are already loaded
			for(unsigned int i=0; i<loadedTiles.size(); i++) {
				tileRowCol.erase(std::remove(tileRowCol.begin(), tileRowCol.end(), loadedTiles[i]), tileRowCol.end());
			}

			// Update Required Tiles
			requiredTiles = tileRowCol;
		};
	}

	void stopThreads() {
		// Stops the threads
		threadRunning = false;
		satTileLoadThread.join();
		satTileDownloaderThread.join();
	}

	// Update List
	void updateDiskTiles(const char* folderPath) {
		/* Checks the folder for any new tiles */
		// Get current file list
		path p(folderPath);
		string newFilename;	// Filename of image file
		string mypath;		// Path of image file relative to src directory

		// Loop over files in directory
		for(auto i = directory_iterator(p); i != directory_iterator(); i++) {
			if (!is_directory(i->path())) { // Ignore subdirectories
				newFilename = i->path().filename().string();
				// Check if file is not in use
				std::fstream myfile;
				mypath = "";
				mypath.append(folderPath);
				mypath.append("/");
				mypath.append(newFilename);
				myfile.open(mypath);
				if (myfile.is_open()) {
					myfile.close();
					// File has finished downloading
					// Parse filename for zoom, row, col
					vector<string> split = splitStringDelim(newFilename, "-");
					int zoom = atoi(split[0].c_str());
					int x = atoi(split[1].c_str());
					int y = atoi(split[2].c_str());
					vector<int> tileVec = {x,y,zoom};
					// Add Tiles to disk tiles list
					if (!(std::find(diskTiles.begin(), diskTiles.end(), tileVec) != diskTiles.end())) {
						// Vector not in vector
						diskTiles.push_back(tileVec);
					}
					// Load Tile
					tiles.push_back(SatTile(origin, x, y, zoom, mypath));
					loadedTiles.push_back({x,y,zoom});
					string xStr = std::to_string(x);
					string yStr = std::to_string(y);
					printf("Loaded Tile %s.\n",(zoom+"-"+xStr+"-"+yStr).c_str());
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
