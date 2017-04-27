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
	std::vector<std::string> foundNames;
	int lineNum 	= 0;
	const char* settingsFile;

	// Display
	int screenID 	= 1;
	int xRes 		= 1920;
	int yRes		= 1080;
	bool fullscreen = false;

	// Setting Names
	std::vector<std::string> intNames = {"screenID","xRes","yRes"};
	std::vector<std::string> boolNames = {"fullscreen"};

	/* Constructor */
	Settings(const char* settingsFile);


	/* Functions */
	void readFile();
	void parseSetting(std::string line);
	void parseIntSettings(std::string line, std::vector<std::string> lineSplit);
	void parseBoolSettings(std::string line, std::vector<std::string> lineSplit);
	void checkMissingSettings();


};














#endif /* SETTINGS_H_ */
