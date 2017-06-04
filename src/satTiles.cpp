/*
 * satTiles.cpp
 *
 *  Created on: 23Apr.,2017
 *      Author: bcub3d-desktop
 */

#include "satTiles.h"



/* Functions */
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

vector<float> latLonOffsetHeading(float lat1, float lon1, float distance, float bearing, float sphereRadius) {
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

/* Constructor */
SatTile::SatTile(glm::vec3 origin, int x, int y, int zoom, string filename) {
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
			// Positions								// Normals			// Texture Coords
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

/* Class Member Functions */
vector<double> SatTile::calcTileWidthHeight(glm::dvec3 ecefOrigin, glm::dvec3 geoPos1, glm::dvec3 geoPos2) {
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

void SatTile::calcTileWidthHeightAll(glm::dvec3 ecefOrigin) {
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
glm::dvec3 SatTile::geo2ECEF(glm::dvec3 positionVector) {
	// positionVector: (latitude, longitude, altitude (m))
	// Uses WGS84 defined here https://en.wikipedia.org/wiki/Geodetic_datum#Geodetic_to.2Ffrom_ECEF_coordinates
	GLdouble a = 6378137.0;
	GLdouble e2 = 6.69437999014e-3;
	GLdouble lat = glm::radians(positionVector[0]);
	GLdouble lon = glm::radians(positionVector[1]);
	//GLdouble alt = glm::radians(positionVector[2]);
	GLdouble N = a / sqrt(1-(e2*pow(sin(lat),2)));
	GLdouble h = positionVector[2]; // Convert to m
	GLdouble ex = (N+h)*cos(lat)*cos(lon); // m
	GLdouble ey = (N+h)*cos(lat)*sin(lon); // m
	GLdouble ez = (N*(1-e2) + h) * sin(lat);    // m

	return glm::dvec3(ex,ey,ez);
}

/* Convert from ECEF to ENU */
glm::dvec3 SatTile::ecef2ENU(glm::dvec3 ecefVector, glm::dvec3 ecefOrigin, glm::dvec3 origin) {
	GLdouble lat = glm::radians(origin[0]);
	GLdouble lon = glm::radians(origin[1]);
	//GLfloat alt = origin[2];
	glm::mat3 A = glm::mat3(-sin(lon),					cos(lon),					0.0,
							-sin(lat)*cos(lon),	-sin(lat)*sin(lon),	cos(lat),
							cos(lat)*cos(lon),	cos(lat)*sin(lon),	sin(lat));
	glm::dvec3 B = glm::dvec3(ecefVector[0]-ecefOrigin[0],ecefVector[1]-ecefOrigin[1],ecefVector[2]-ecefOrigin[2]);
	B*A; // Flipped due to GLM ordering

	return B*A;
}

/* Draw Function */
void SatTile::Draw(Shader shader) {
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

void SatTile::createAndSetupBuffers() {
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

void SatTile::setupTexture() {
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

/* Constructor */
SatTileList::SatTileList(glm::vec3 origin, MavAircraft* mavAicraftPt) : satTileDownloadThread(&SatTileList::satTileDownloader,this) {
	this->origin = origin;
	this->mavAircraftPt = mavAicraftPt;

	// Setup Curl
	curlPt = curl_easy_init();

	// Update Set of Disk Tiles
	getDiskTiles();

	// Update Required Tiles
	updateRequiredTiles();

	// Load First set of Tiles
	loadRequiredTiles();

	// Update tiles to be downloaded
	getDownloadListTiles();

	initFin = true;
}

/* Functions */
void SatTileList::updateTiles() {
	// Loads tiles that have been downloaded and are required
	// Update Required Tiles
	updateRequiredTiles();

	// Load Required Tiles
	loadRequiredTiles();

	// Update tiles to be downloaded
	getDownloadListTiles();
}

void SatTileList::satTileDownloader() {
	// The thread to load tiles from disk into the world
	while (!initFin) {
		// Delay until initialisation finished
		std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0)));
	}

	while(threadRunning) {
		if(toDownloadTiles.size() > 0) {
			// Download the tiles
			downloadTiles();
		} else {
			// Delay next run
			std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0)));
		}
	}
}

void SatTileList::stopThreads() {
	// Stops the threads
	threadRunning = false;
	satTileDownloadThread.join();
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	// Writes data to the file from the curl stream
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

/* Get and Load Functions */
void SatTileList::downloadTile(vector<int> tileVec) {
	// Downloads the tile specified by tileVec
	int x = tileVec[0];
	int y = tileVec[1];
	int zoom = tileVec[2];
	string outName = "../SatTiles/" + std::to_string(zoom) + "-" + std::to_string(x) + "-" + std::to_string(y) + ".png";
	string url = "http://maptile.maps.svc.ovi.com/maptiler/v2/maptile/newest/hybrid.day/" + std::to_string(zoom) +"/" + std::to_string(x) + "/" + std::to_string(y) +"/256/png8";
	if(curlPt) {
		CURLcode result;
		FILE *outfile;
		curl_easy_setopt(curlPt, CURLOPT_URL,url.c_str());
		// Full Debugging
		curl_easy_setopt(curlPt, CURLOPT_VERBOSE, 1L);
		// Disable progress display
		curl_easy_setopt(curlPt, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curlPt, CURLOPT_VERBOSE, 0L);
		// Send data here
		curl_easy_setopt(curlPt, CURLOPT_WRITEFUNCTION, write_data);
		// Open File
		outfile = fopen(outName.c_str(), "wb");
		if(outfile) {
			// Setup output file
			curl_easy_setopt(curlPt, CURLOPT_WRITEDATA, outfile);
			// Get file and write data
			result = curl_easy_perform(curlPt);
			// Close
			fclose(outfile);
			if(result == CURLE_OK) {
				printf("Download successful %s\n",outName.c_str());
				threadLock.lock();
				downloadedTiles.push_back(tileVec);
				toDownloadTiles.erase(std::remove(toDownloadTiles.begin(), toDownloadTiles.end(), tileVec), toDownloadTiles.end());
				threadLock.unlock();
			}
		}
	}
}

void SatTileList::loadTile(vector<int> tileVec) {
	// Loads the tile specified by tileVec
	int x = tileVec[0];
	int y = tileVec[1];
	int zoom = tileVec[2];
	string mypath = folderPath + std::to_string(zoom) + "-" + std::to_string(x) + "-" + std::to_string(y) + ".png";
	tiles.push_back(SatTile(origin, x, y, zoom, mypath));
	loadedTiles.push_back(tileVec);
	//printf("Loaded %s\n",mypath.c_str());
}

/* Update vector functions */
void SatTileList::getDiskTiles() {
	// Gets the list of tiles on disk
	path p(folderPath);
	string newFilename;	// Filename of image file
	string mypath;		// Path of image file relative to src directory

	// Loop over files in directory
	// Clear disk tiles
	for (auto i = directory_iterator(p); i != directory_iterator(); i++) {
		if (!is_directory(i->path())) { // Ignore subdirectories
			newFilename = i->path().filename().string();
			// Check if this file is already loaded
			string ext = extension(newFilename);
			mypath = "";
			mypath.append(folderPath);
			mypath.append("/");
			mypath.append(newFilename);
			if((ext.compare(".png"))*(ext.compare(".jpg")) == 0) {
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
					// Add to disk tiles
					downloadedTiles.push_back({x,y,zoom});
				} else {
					/* File is probably still being downloaded. */
					std::cout << "Satellite Tile " << newFilename << "is in use. Not adding.\n";
				}
			}
		}
	}
}

void SatTileList::updateRequiredTiles() {
	// Calculates the tiles required at the current point in time
	// Get current tile below aircraft
	if ((mavAircraftPt != NULL) || (mavAircraftPt->positionHistory.size() < 0)) {
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

	// Generate square around main tile
	vector<weightVector> tileRowCol;
	float dmax = sqrt(pow(xmax-xmin,2)+pow(ymax-ymax,2));
	for(int i=xmin; i<xmax+1; i++) {
		for(int j=ymin; j<ymax+1; j++) {
			// Calculate weighting for priority
			float d = sqrt(pow(i-currTiles[0],2)+pow(j-currTiles[1],2));
			float theta = atan2(i-currTiles[0],j-currTiles[1]) - (mavAircraftPt->heading) + M_PI; //atan2(y,x) but we want tan(theta)=x/y so atan2(x,y)
			float w = (dmax-d)*fabs(cos(theta/2.0));
			// Store struct
			tileRowCol.push_back({{i,j,zoom},w});
		}
	}

	// Sort based on weightings
	std::sort(tileRowCol.begin(), tileRowCol.end(), [](weightVector& i, weightVector& j) {return i.weight > j.weight;});

	// Update Required Tiles
	vector<vector<int>> tempReqTiles;
	for(unsigned int i = 0; i<tileRowCol.size(); i++) {
		tempReqTiles.push_back(tileRowCol[i].tile);
	}
	requiredTiles = tempReqTiles;
}


void SatTileList::loadRequiredTiles() {
	// Loads required tiles after they've completed downloading
	threadLock.lock();
	toLoadTiles = downloadedTiles;
	threadLock.unlock();
	unsigned int loadedCount = 0;
	for(unsigned int i=0; i<toLoadTiles.size(); i++) {
		// Load tiles if still required
		if(loadAll || std::find(requiredTiles.begin(), requiredTiles.end(), toLoadTiles[i]) != requiredTiles.end()) {
			// Load tiles from disk if not already loaded
			if(!(std::find(loadedTiles.begin(), loadedTiles.end(), toLoadTiles[i]) != loadedTiles.end())) {
				// Load tile
				loadTile(toLoadTiles[i]);
				loadedCount += 1;
			}
		}
	}
	if (loadedCount>0) {
		printf("Loaded %u Satellite Tiles.\n",loadedCount);
	}

}

void SatTileList::getDownloadListTiles() {
	// Creates a list of the tiles that need to be downloaded
	threadLock.lock();
	toDownloadTiles = {};
	for(unsigned int i=0; i<requiredTiles.size(); i++) {
		if (!(std::find(downloadedTiles.begin(), downloadedTiles.end(), requiredTiles[i]) != downloadedTiles.end())) {
			// Tiles required but not on disk
			toDownloadTiles.push_back(requiredTiles[i]);
		}
	}
	threadLock.unlock();
}

void SatTileList::downloadTiles() {
	// Downloads the tiles in the toDownloadTiles vector
	threadLock.lock();
	vector<vector<int>> downloadList = toDownloadTiles;
	threadLock.unlock();
	for(unsigned int i=0; i<downloadList.size(); i++) {
		if(threadRunning) {
			downloadTile(downloadList[i]);
		}
	}
}

/* Draw Function */
void SatTileList::Draw(Shader shader) {
	// Draw tiles
	for(unsigned int i=0; i != tiles.size(); i++) {
		tiles[i].Draw(shader);
	}
}


