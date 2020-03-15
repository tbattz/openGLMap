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
	boost::split(lineSplit, line, boost::is_any_of("\t "), boost::token_compress_on);
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
	} else if (lineSplit.size() == 5) {
		if (lineSplit[0]=="origin") {
			parseOriginSettings(line, lineSplit);
		}
		if (lineSplit[0]=="aircraft") {
			// Look through aircraft
			parseAircraftSettings(line, lineSplit);
		}
	} else if (lineSplit.size() > 10) {
		if (lineSplit[0] == "volume") {
			// Look through volume definition
			parseVolumeSettings(line, lineSplit);
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

void Settings::parseOriginSettings(std::string line, std::vector<std::string> lineSplit) {
	// Parses aircraft settings into the class
	double lat = atof(lineSplit[1].c_str());
	double lon = atof(lineSplit[2].c_str());
	double alt = atof(lineSplit[3].c_str());
	double heading = atof(lineSplit[4].c_str());

	origin = {lat,lon,alt,heading};
	originSet = true;
}

void Settings::parseAircraftSettings(std::string line, std::vector<std::string> lineSplit) {
	// Parses aircraft settings into the class
	std::string name = lineSplit[1];
	std::string filepath = lineSplit[2];
	std::string ipString = lineSplit[3];
	std::string	port = lineSplit[4];

	aircraftConnection aircraftCon = {name,filepath,ipString,port};
	aircraftConList.push_back(aircraftCon);
}

void Settings::parseVolumeSettings(std::string line, std::vector<std::string> lineSplit) {
	// Parses volume settings into the class
	std::string name = lineSplit[1];
	std::vector<int> rgb = {stoi(lineSplit[2]),stoi(lineSplit[3]),stoi(lineSplit[4])};
	float alpha = std::stof(lineSplit[5].c_str());
	int nPts = stoi(lineSplit[6]);
	std::vector<std::vector<double>> pts;
	for(int i=0; i<nPts; i++) {
		std::string ptStr = lineSplit[7+i];
		std::vector<std::string> ptSplit;
		boost::split(ptSplit, ptStr, boost::is_any_of(","), boost::token_compress_on);
		double lat = std::atof(ptSplit[0].substr(1).c_str());
		double lon = std::atof(ptSplit[1].c_str());
		double lowAlt = std::atof(ptSplit[2].c_str());
		double highAlt = std::atof(ptSplit[3].substr(0,ptSplit[3].size()-1).c_str());
		std::vector<double> pt = {lat,lon,lowAlt,highAlt};
		pts.push_back(pt);
	}

	volumeDef volume = {name,rgb,alpha,pts};
	volumeList.push_back(volume);
}

void Settings::checkMissingSettings() {
	// Checks if any settings haven't been found, and sets them to their defaults. Only checks required settings.
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
	// Origin
	if (!originSet) {
		printf("Origin not found! Setting to default: lat: %f, lon: %f, alt: %f, heading: %f\n",origin[0],origin[1],origin[2],origin[3]);
	}
}


