/*
 * settings.cpp
 *
 *  Created on: 26Apr.,2017
 *
*/

#include "settings.h"


/* Constructor */
Settings::Settings(const char* settingsFile) {
	this->settingsFile = settingsFile;

	/* Read File */
	readFile();

}

/* Functions */
void Settings::readFile() {
	// Reads the config file and parses the settings
	std::ifstream infile(settingsFile);
	std::string line;
	std::map<std::string, int> numbers;
	while (std::getline(infile, line)) {
		lineNum += 1;
		// Don't parse comments
		if (line[0] != '#') {
			if (line.size()>2) {
				// Store variable
				parseSetting(line);
			} else if (line.find_first_not_of(' ') != std::string::npos) {
				printf("Invalid line. %i: %s\n",lineNum,line.c_str());
			}

		}
	}
	// Close file
	infile.close();

	// Check for missing settings
	checkMissingSettings();
}

void Settings::parseSetting(std::string line) {
	// Reads in a line and stores the setting value
	// Split line on spaces
	std::vector<std::string> lineSplit;
	boost::split(lineSplit,line, boost::is_any_of("\t "), boost::token_compress_on);
	// Check setting type
	if (lineSplit.size() == 3) {
		if (lineSplit[1]=="int") {
			// Look through ints
			parseIntSettings(line, lineSplit);
		} else if (lineSplit[1]=="bool") {
			// Look through booleans
			parseBoolSettings(line, lineSplit);
		} else {
			printf("ERROR: Unknown Setting! %s: Line %i\n",lineSplit[0].c_str(),lineNum);
		}
	} else {
		printf("ERROR: Unknown Setting! %s: Line %i\n",lineSplit[0].c_str(),lineNum);
	}
}

void Settings::parseIntSettings(std::string line, std::vector<std::string> lineSplit) {
	// Parses int settings into the class
	if (lineSplit[0] == "screenID") {
		screenID = stoi(lineSplit[2]);
		foundNames.push_back("screenID");
	} else if (lineSplit[0] == "xRes") {
		xRes = stoi(lineSplit[2]);
		foundNames.push_back("xRes");
	} else if (lineSplit[0] == "yRes") {
		yRes = stoi(lineSplit[2]);
		foundNames.push_back("yRes");
	} else {
		printf("Could not find int. %i: %s\n",lineNum,line.c_str());
	}
}

void Settings::parseBoolSettings(std::string line, std::vector<std::string> lineSplit) {
	// Parses bool settings into the class
	if(lineSplit[0] == "fullscreen") {
		fullscreen = stoi(lineSplit[2]) ? true : false;
		foundNames.push_back("fullscreen");
	}
}

void Settings::checkMissingSettings() {
	// Checks if any settings haven't been found, and sets them to their defaults
	// Ints
	for(unsigned int i=0; i<intNames.size(); i++) {
		if(std::find(foundNames.begin(), foundNames.end(), intNames[i]) == foundNames.end()) {
			// Not found
			printf("%s not found! Setting to default.\n",intNames[i].c_str());
		}
	}
	// Booleans
	for(unsigned int i=0; i<boolNames.size(); i++) {
		if(std::find(foundNames.begin(), foundNames.end(), boolNames[i]) == foundNames.end()) {
			// Not found
			printf("%s not found! Setting to default.\n",boolNames[i].c_str());
		}
	}
}


