/*
 * settings.h
 *
 *  Created on: 26Apr.,2017
 *      Author: bcub3d-desktop
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

// Standard Includes
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

// Boost includes
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

/* Classes */
class Settings {
public:
	/* Data */
	int lineNum 	= 0;
	const char* settingsFile;
	// Settings Name Vectors
	std::vector<std::string> intNames = {"screenID","xRes","yRes"};
	std::vector<int>		 intDefaults = {1,	    1920,   1080};
	std::vector<std::string> boolNames = {"fullscreen"};
	std::vector<bool>		 boolDefaults = {false};
	// Setting Maps
	std::map<std::string, int> intSets;
	std::map<std::string, bool> boolSets;



	// Display
	int screenID 	= 0;
	int xRes 		= 1920;
	int yRes		= 1080;
	bool fullscreen = false;


	/* Constructor */
	Settings(const char* settingsFile);


	/* Functions */
	void readFile();
	void parseSetting(std::string line);
	void checkSettings();




};














#endif /* SETTINGS_H_ */
