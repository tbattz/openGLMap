//
// Created by bcub3d-desktop on 26/1/20.
//

#ifndef OPENGLMAP_SATTILEGROUPCONTROLLER_H
#define OPENGLMAP_SATTILEGROUPCONTROLLER_H


// Standard Includes
#include <memory>
#include <vector>
#include <thread>
#include <curl/curl.h>
#include <mutex>
#include <cstdio>

// Boost
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

// GLM Mathematics
#include <glm/glm.hpp>

// GL Includes
#include <GL/glew.h>

// GLFW (Multi-platform library for OpenGL)
#include <GLFW/glfw3.h>

// Project Includes
#include <renderEngine/shader.h>
#include <objects/worldObject/WorldObjectController.h>
#include "SatTileController.h"



/* Structures */
struct weightVector {
    std::vector<int> tile;
    float 		weight;
};


class SatTileGroupController {
public:
    /* Constructor */
    SatTileGroupController(glm::vec3 origin, std::shared_ptr<WorldObjectController> worldObjectController);
    /* Destructor */
    ~SatTileGroupController();


    /* Functions */
    void setWorldObjectController(std::shared_ptr<WorldObjectController> worldObjectController);
    std::vector<float> latLonOffsetHeading(float lat1, float lon1, float distance, float bearing, float sphereRadius = 6378.137);
    std::vector<float> latLon2TileNum(float lat, float lon, int zoom);
    void updateTiles();
    void draw(Shader shader);

private:
    /* Data */
    glm::dvec3      origin;
    //int             zoom = 18;
    int             zoom = 14;
    float           aircraftRadius = 1000; // m
    // TODO - Fix relative paths
    const char*     folderPath = "../../SatTiles/";
    bool            initFin = false;
    GLfloat         fileChecklast = 0.0f;

    /* Aircraft */
    std::shared_ptr<WorldObjectController> currWorldObjectController;

    /* Tiles */
    std::vector<SatTileController>  tileControllers;
    std::vector<std::vector<int>>	requiredTiles;
    std::vector<std::vector<int>>   toDownloadTiles;
    std::vector<std::vector<int>>   downloadedTiles;
    std::vector<std::vector<int>>   toLoadTiles;
    std::vector<std::vector<int>>	loadedTiles;
    bool 				loadAll = true;
    CURL*				curlPt;
    std::mutex			threadLock;

    /* Threads */
    std::thread satTileDownloadThread;
    bool		threadRunning = true;

    /* Functions */
    void satTileDownloader();
    void stopThreads();
    void downloadTile(std::vector<int> tileVec);
    void loadTile(std::vector<int> tileVec);
    std::vector<string> splitStringDelim(string inString, string delim);
    void getDiskTiles();
    void updateRequiredTiles();
    void loadRequiredTiles();
    void getDownloadListTiles();
    int getMSDigit(int x, int y);
    std::string getQuadCode(int x, int y, int zoom);
    void downloadTiles();

};


#endif //OPENGLMAP_SATTILEGROUPCONTROLLER_H
