//
// Created by bcub3d-desktop on 26/1/20.
//

#include "SatTileGroupController.h"


/* Constructor */
SatTileGroupController::SatTileGroupController(glm::vec3 origin, std::shared_ptr<WorldObjectController> worldObjectController) {
    this->origin = origin;

    // Set current model used to update position to search for new tiles
    setWorldObjectController(worldObjectController);

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

    // Create thread
    satTileDownloadThread = std::thread(&SatTileGroupController::satTileDownloader, this);
}

/* Destructor */
SatTileGroupController::~SatTileGroupController() {
    stopThreads();
}

std::vector<float> SatTileGroupController::latLon2TileNum(float lat, float lon, int zoom) {
    // lat (deg), lon (deg)
    float lat_rad = lat * M_PI / 180.0;
    int n = pow(2,zoom);
    float x = n * (lon + 180.0) / 360.0;
    float y = n * (1 - (log(tan(lat_rad) + 1.0/cos(lat_rad)) / M_PI)) / 2.0;

    return {x,y};
}

std::vector<float> SatTileGroupController::latLonOffsetHeading(float lat1, float lon1, float distance, float bearing, float sphereRadius) {
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

void SatTileGroupController::satTileDownloader() {
    // The thread to load tiles from disk into the world
    while (!initFin) {
        // Delay until initialisation finished
        std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0)));
    }

    while(threadRunning) {
        if(!toDownloadTiles.empty()) {
            // Download the tiles
            downloadTiles();
        } else {
            // Delay next run
            std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0)));
        }
    }
}

void SatTileGroupController::stopThreads() {
    // Stops the threads
    threadRunning = false;
    satTileDownloadThread.join();
    std::cout << "Satellite Tile Download Thread joined." << std::endl;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    // Writes data to the file from the curl stream
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int SatTileGroupController::getMSDigit(int x, int y) {
    return (((y & 3) << 1) + (x & 1));
}

std::string SatTileGroupController::getQuadCode(int x, int y, int zoom) {
    std::ostringstream quadcode;
    //std::string quadcode(zoom, '*');
    //std::vector<int> digits;
    int pos = -1;
    for (int i=zoom-1; i > -1; i--) {
        pos += 1;
        quadcode << ((((y >> i) & 1) << 1) + ((x >> i) & 1));
    }

    return quadcode.str();
}

/* Get and Load Functions */
void SatTileGroupController::downloadTile(std::vector<int> tileVec) {
    // Downloads the tile specified by tileVec
    int x = tileVec[0];
    int y = tileVec[1];
    //int x = 0;
    //int y = 0;
    int tileZoom = tileVec[2];
    //int tileZoom = 2;
    // Prepare paths
    std::string msDigit = std::to_string(getMSDigit(x, y));
    std::string quadCode = getQuadCode(x, y, tileZoom);
    string outName = folderPath + std::to_string(tileZoom) + "-" + std::to_string(x) + "-" + std::to_string(y) + ".jpeg";
    ///string url = "http://maptile.maps.svc.ovi.com/maptiler/v2/maptile/newest/hybrid.day/" + std::to_string(tileZoom) +"/" + std::to_string(x) + "/" + std::to_string(y) +"/256/png8";
    // TODO - Allow selector of different microsoft maps. Store each in a different folder.
    string url = "http://ecn.t" + msDigit + ".tiles.virtualearth.net/tiles/h" + quadCode + ".png?g=441&mkt=en-us&n=z";
    std::cout << url << std::endl;
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

void SatTileGroupController::loadTile(std::vector<int> tileVec) {
    // Loads the tile specified by tileVec
    int x = tileVec[0];
    int y = tileVec[1];
    int tileZoom = tileVec[2];
    string mypath = folderPath + std::to_string(tileZoom) + "-" + std::to_string(x) + "-" + std::to_string(y) + ".png";
    // Create tile
    tileControllers.push_back(SatTileController(origin, x, y, tileZoom, mypath));
    loadedTiles.push_back(tileVec);
    //printf("Loaded %s\n",mypath.c_str());
}

/* String Splitter */
std::vector<string> SatTileGroupController::splitStringDelim(string inString, string delim) {
    /* Returns a vector of strings split by a deliminator. */
    unsigned int pos1 = 0;
    unsigned int pos2 = inString.find(delim,pos1);
    std::vector<string> outVec;
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

/* Update vector functions */
void SatTileGroupController::getDiskTiles() {
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
                    std::vector<string> split = splitStringDelim(newFilename, "-");
                    // Create New Tile
                    int tileZoom = atoi(split[0].c_str());
                    int x = atoi(split[1].c_str());
                    int y = atoi(split[2].c_str());
                    // Add to disk tiles
                    downloadedTiles.push_back({x,y,tileZoom});
                } else {
                    /* File is probably still being downloaded. */
                    std::cout << "Satellite Tile " << newFilename << "is in use. Not adding.\n";
                }
            }
        }
    }
}

void SatTileGroupController::updateRequiredTiles() {
    // Calculates the tiles required at the current point in time
    // Get current tile below aircraft
    glm::dvec3 aircraftGeoPos;
    if (currWorldObjectController != nullptr) {
        aircraftGeoPos = currWorldObjectController->getPosition();
    } else {
        aircraftGeoPos = origin;
    }
    std::vector<float> currTiles = latLon2TileNum(aircraftGeoPos[0], aircraftGeoPos[1], zoom);
    // Find tiles within distance based on aircraft position
    std::vector<float> latLonT = latLonOffsetHeading(aircraftGeoPos[0], aircraftGeoPos[1], aircraftRadius/1000.0, 0);
    std::vector<float> latLonR = latLonOffsetHeading(aircraftGeoPos[0], aircraftGeoPos[1], aircraftRadius/1000.0, 90);
    std::vector<float> latLonB = latLonOffsetHeading(aircraftGeoPos[0], aircraftGeoPos[1], aircraftRadius/1000.0, 180);
    std::vector<float> latLonL = latLonOffsetHeading(aircraftGeoPos[0], aircraftGeoPos[1], aircraftRadius/1000.0, 270);
    std::vector<float> tilesT = latLon2TileNum(latLonT[0], latLonT[1], zoom);
    std::vector<float> tilesR = latLon2TileNum(latLonR[0], latLonR[1], zoom);
    std::vector<float> tilesB = latLon2TileNum(latLonB[0], latLonB[1], zoom);
    std::vector<float> tilesL = latLon2TileNum(latLonL[0], latLonL[1], zoom);
    int xmin = std::min(tilesL[0],tilesR[0]);
    int xmax = std::max(tilesL[0],tilesR[0]);
    int ymin = std::min(tilesT[1],tilesB[1]);
    int ymax = std::max(tilesT[1],tilesB[1]);

    // Generate square around main tile
    std::vector<weightVector> tileRowCol;
    float dmax = sqrt(pow(xmax-xmin,2)+pow(ymax-ymax,2));
    for(int i=xmin; i<xmax+1; i++) {
        for(int j=ymin; j<ymax+1; j++) {
            // Calculate weighting for priority
            float d = sqrt(pow(i-currTiles[0],2)+pow(j-currTiles[1],2));
            // TODO - Use real heading
            //float theta = atan2(i-currTiles[0],j-currTiles[1]) - (mavAircraftPt->heading) + M_PI; //atan2(y,x) but we want tan(theta)=x/y so atan2(x,y)
            float theta = atan2(i-currTiles[0],j-currTiles[1]) - (0.0) + M_PI; //atan2(y,x) but we want tan(theta)=x/y so atan2(x,y)
            float w = (dmax-d)*fabs(cos(theta/2.0));
            // Store struct
            tileRowCol.push_back({{i,j,zoom},w});
        }
    }

    // Sort based on weightings
    std::sort(tileRowCol.begin(), tileRowCol.end(), [](weightVector& i, weightVector& j) {return i.weight > j.weight;});

    // Update Required Tiles
    std::vector<std::vector<int>> tempReqTiles;
    for(unsigned int i = 0; i<tileRowCol.size(); i++) {
        tempReqTiles.push_back(tileRowCol[i].tile);
    }
    requiredTiles = tempReqTiles;
}


void SatTileGroupController::loadRequiredTiles() {
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
                std::cout << "Loaded tile." << std::endl;
            }
        }
    }
    if (loadedCount>0) {
        printf("Loaded %u Satellite Tiles.\n",loadedCount);
    }

}

void SatTileGroupController::getDownloadListTiles() {
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

void SatTileGroupController::downloadTiles() {
    // Downloads the tiles in the toDownloadTiles vector
    threadLock.lock();
    std::vector<std::vector<int>> downloadList = toDownloadTiles;
    threadLock.unlock();
    for(unsigned int i=0; i<downloadList.size(); i++) {
        if(threadRunning) {
            downloadTile(downloadList[i]);
        }
    }
}

/* Functions */
void SatTileGroupController::updateTiles() {
    // Loads tiles that have been downloaded and are required
    GLfloat currentFrame = glfwGetTime();
    if ((currentFrame - fileChecklast) > 0.3) {
        // Update Required Tiles
        updateRequiredTiles();

        // Load Required Tiles
        loadRequiredTiles();

        // Update tiles to be downloaded
        getDownloadListTiles();

        // Update file check time
        fileChecklast = currentFrame;
    }
}



void SatTileGroupController::draw(Shader shader) {
    for(unsigned int i=0; i != tileControllers.size(); i++) {
        // Draw tile
        tileControllers[i].draw(shader);
    }

}

void SatTileGroupController::setWorldObjectController(std::shared_ptr<WorldObjectController> worldObjectController) {
    currWorldObjectController = worldObjectController;
}
