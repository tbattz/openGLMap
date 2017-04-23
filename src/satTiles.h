/*
 * satTiles.h
 *
 *  Created on: 16Apr.,2017
 *      Author: bcub3d-desktop
 */

#ifndef SATTILES_H_
#define SATTILES_H_

// Boost
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

// Standard Includes
#include <thread>
#include <curl/curl.h>
using std::vector;

// Project Includes
#include "mavAircraft.h"


/* Structures */
struct weightVector {
	vector<int> tile;
	float 		weight;
};

/* Functions */
vector<float> latLon2TileNum(float lat, float lon, int zoom);
vector<double> tileNum2LatLon(float x, float y, int zoom);
vector<string> splitStringDelim(string inString, string delim);
vector<float> latLonOffsetHeading(float lat1, float lon1, float distance, float bearing, float sphereRadius = 6378.137);


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

	/* Constructor */
	SatTile(glm::vec3 origin, int x, int y, int zoom, string filename);

	/* Functions */
	vector<double> calcTileWidthHeight(glm::dvec3 ecefOrigin, glm::dvec3 geoPos1, glm::dvec3 geoPos2);
	void calcTileWidthHeightAll(glm::dvec3 ecefOrigin);
	glm::dvec3 geo2ECEF(glm::dvec3 positionVector);
	glm::dvec3 ecef2ENU(glm::dvec3 ecefVector, glm::dvec3 ecefOrigin, glm::dvec3 origin);
	void Draw(Shader shader);


private:
	/* Functions */
	void createAndSetupBuffers();
	void setupTexture();

};

class SatTileList {
public:
	/* Data */
	glm::vec3			origin;				// lat (deg), lon (deg)
	int 				zoom = 18;
	float				aircraftRadius = 1000; // m
	const char* 		folderPath = "../SatTiles/";
	bool				initFin = false;

	/* Tiles */
	vector<SatTile>		tiles;
	vector<vector<int>>	requiredTiles;
	vector<vector<int>> toDownloadTiles;
	vector<vector<int>> downloadedTiles;
	vector<vector<int>> toLoadTiles;
	vector<vector<int>>	loadedTiles;
	bool 				loadAll = true;
	CURL*				curlPt;
	std::mutex			threadLock;

	/* Aircraft */
	MavAircraft* mavAircraftPt;
	glm::dvec3 aircraftGeoPos;

	/* Threads */
	std::thread satTileDownloadThread;
	bool		threadRunning = true;

	/* Constructor */
	SatTileList(glm::vec3 origin, MavAircraft* mavAicraftPt);

	/* Functions */
	void updateTiles();
	void satTileDownloader();
	void stopThreads();
	void downloadTile(vector<int> tileVec);
	void loadTile(vector<int> tileVec);
	void getDiskTiles();
	void updateRequiredTiles();
	void loadRequiredTiles();
	void getDownloadListTiles();
	void downloadTiles();
	void Draw(Shader shader);
};





#endif /* SATTILES_H_ */
