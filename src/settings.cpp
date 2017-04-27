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

	// Check Settings
	checkSettings();
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
			for(unsigned int i=0; i<intNames.size(); i++) {
				if (lineSplit[0]==intNames[i]) {
					intSets[intNames[i]] = stoi(lineSplit[2]);
					break;
				}
			}
		} else if (lineSplit[1]=="bool") {
			// Look through booleans
			for(unsigned int i=0; i<boolNames.size(); i++) {
				if (lineSplit[0]==boolNames[i]) {
					boolSets[boolNames[i]] = stoi(lineSplit[2]);
				}
			}
		} else {
			printf("ERROR: Unknown Setting! %s: Line %i\n",lineSplit[0].c_str(),lineNum);
		}
	} else {
		printf("ERROR: Unknown Setting! %s: Line %i\n",lineSplit[0].c_str(),lineNum);
	}
}

void Settings::checkSettings() {
	// Checks if all required settings have been set
	// Ints
	for(unsigned int i=0; i<intNames.size(); i++) {
		if(intSets.count(intNames[i])==0) {
			printf("ERROR: %s missing. Setting to default: %i\n",intNames[i].c_str(),intDefaults[i]);
			intSets["screenID"] = intDefaults[i];
		}
	}
	// Booleans
	for(unsigned int i=0; i<boolNames.size(); i++) {
		if(boolSets.count(boolNames[i])==0) {
			printf("ERROR: %s missing. Setting to default: %s\n",boolNames[i].c_str(),boolDefaults[i] ? "true" : "false");
			intSets["screenID"] = intDefaults[i];
		}
	}
}




