/*
 * volumes.cpp
 *
 *  Created on: 1Jun.,2017
 *      Author: bcub3d-desktop
 */

#include "math.h"

#include "volumes.h"


/* Conversion Geodetic to ECEF */
glm::dvec3 geo2ECEF(glm::dvec3 positionVector) {
	// positionVector: (latitude, longitude, altitude (m))
	// Uses WGS84 defined here https://en.wikipedia.org/wiki/Geodetic_datum#Geodetic_to.2Ffrom_ECEF_coordinates
	const double pi = 3.141592653589793238462643383279502884;
	GLdouble a = 6378137.0;
	GLdouble e2 = 6.6943799901377997e-3;
	GLdouble lat = positionVector[0] * pi /180.0;
	GLdouble lon = positionVector[1] * pi /180.0;
	GLdouble alt = positionVector[2];
	GLdouble N = a / sqrt(1-(e2*sin(lat)*sin(lat)));
	GLdouble ex = (N+alt)*cos(lat)*cos(lon); // m
	GLdouble ey = (N+alt)*cos(lat)*sin(lon); // m
	GLdouble ez = (N*(1-e2) + alt) * sin(lat);    // m

	return glm::dvec3(ex,ey,ez);
}

/* Convert from ECEF to NEU */
glm::dvec3 ecef2NEU(glm::dvec3 ecefVector, glm::dvec3 ecefOrigin, glm::dvec3 origin) {
	GLdouble lat = glm::radians(origin[0]);
	GLdouble lon = glm::radians(origin[1]);
	glm::dmat3 A = glm::dmat3(-sin(lat)*cos(lon),	-sin(lat)*sin(lon),	cos(lat),
							  -sin(lon),			 cos(lon),			0.0,
							  cos(lat)*cos(lon),	 cos(lat)*sin(lon),	sin(lat));
	glm::dvec3 B = glm::dvec3(ecefVector[0]-ecefOrigin[0],ecefVector[1]-ecefOrigin[1],ecefVector[2]-ecefOrigin[2]);

	return B*A; // Flipped due to GLM ordering
}


/* Constructor */
Volume::Volume(glm::vec3 origin, volumeDef volDef) {
	this->origin = origin;
	this->name = volDef.name;
	this->rgb = volDef.rgb;
	this->alpha = volDef.alpha;
	this->pts = volDef.pts;

	/* Convert Origin */
	ecefOrigin = geo2ECEF(origin);

	/* Create Triangles */
	createTriangles();

	/* Create Lines */
	createLines();

	/* Create and Setup Buffers */
	createAndSetupBuffers();
}


void Volume::createTriangles() {
	// Creates the triangles for the given polygon
	// Triangulate Polygon
	std::vector<std::vector<int>> triangles = triangulatePolygon();

	// Section 1: Top Layer
	// Store pts for top layer
	for(unsigned int i=0; i<pts.size(); i++) {
		/* Convert Geodetic to ECEF */
		glm::dvec3 geoPosition = glm::dvec3(pts[i][0],pts[i][1],pts[i][3]/1000.0); // Lat, Lon, High Alt
		glm::dvec3 ecefPosition = geo2ECEF(geoPosition);
		/* Convert from ECEF to NEU */
		glm::dvec3 pos = ecef2NEU(ecefPosition, ecefOrigin, origin);

		/* Store Pts */
		vertices.push_back(pos[0]); // x
		vertices.push_back(pts[i][3]); // z
		vertices.push_back(pos[1]); // y
	}
	// Store indices for top layer
	for(unsigned int i=0; i<triangles.size(); i++) {
		for(unsigned int j=0; j<3; j++) {
			indices.push_back(triangles[i][j]);
		}
	}
	int topLen = vertices.size()/3.0;

	// Section 2: Bottom Layer
	// Store pts for bottom layer
	for(unsigned int i=0; i<pts.size(); i++) {
		/* Convert Geodetic to ECEF */
		glm::dvec3 geoPosition = glm::dvec3(pts[i][0],pts[i][1],pts[i][2]/1000.0); // Lat, Lon, High Alt
		glm::dvec3 ecefPosition = geo2ECEF(geoPosition);
		/* Convert from ECEF to NEU */
		glm::dvec3 pos = ecef2NEU(ecefPosition, ecefOrigin, origin);

		/* Store Pts */
		vertices.push_back(pos[0]); // x
		if (pts[i][2] == 0) {
			vertices.push_back(pts[i][2]+0.1); // z
		} else {
			vertices.push_back(pts[i][2]); // z
		}

		vertices.push_back(pos[1]); // y
	}
	// Store indices for bottom layer
	for(unsigned int i=0; i<triangles.size(); i++) {
		for(unsigned int j=0; j<3; j++) {
			indices.push_back(triangles[i][j]+topLen);
		}
	}

	// Section 3: Walls
	// Store indices based on the previous points
	// Two triangles per squares between the 2 low alt and 2 high alt points
	for(unsigned int i=0; i<pts.size()-1; i++) {
		// Bottom Triangle
		indices.push_back(i);
		indices.push_back(i+1);
		indices.push_back(i+1+topLen);
		// Top Triangle
		indices.push_back(i);
		indices.push_back(i+topLen);
		indices.push_back(i+1+topLen);
	}
	// Do the last square
	int L = pts.size()-1;
	// Bottom Triangle
	indices.push_back(L);
	indices.push_back(0);
	indices.push_back(topLen);
	// Top Triangle
	indices.push_back(L);
	indices.push_back(L+topLen);
	indices.push_back(topLen);
}

void Volume::createLines() {
	// Creates the lines that outline the polygon
	// Top Polygon
	for(unsigned int i=0; i<pts.size(); i++) {
		lineVerts.push_back(vertices[3*i]);   		// x
		lineVerts.push_back(vertices[3*i+1]); 		// z
		lineVerts.push_back(vertices[3*i+2]); 		// y
	}

	// Bottom Polygon
	int topLen = vertices.size()/2;
	for(unsigned int i=0; i<pts.size(); i++) {
		lineVerts.push_back(vertices[3*i+topLen]);  // x
		lineVerts.push_back(vertices[3*i+1+topLen]);// z
		lineVerts.push_back(vertices[3*i+2+topLen]);// y
	}

	// Side Lines
	for(unsigned int i=0; i<pts.size(); i++) {
		lineVerts.push_back(vertices[3*i]);   		// Top x
		lineVerts.push_back(vertices[3*i+1]); 		// Top z
		lineVerts.push_back(vertices[3*i+2]); 		// Top y
		lineVerts.push_back(vertices[3*i+topLen]);  // Bottom x
		lineVerts.push_back(vertices[3*i+1+topLen]);// Bottom z
		lineVerts.push_back(vertices[3*i+2+topLen]);// Bottom y
	}
}

bool Volume::linesCross(std::vector<double> pt1, std::vector<double> pt2, std::vector<double> pt3, std::vector<double> pt4) {
	// Checks if the line joining x1 with x2 crosses the lines joining x3 with x4 in the given domain
	// Order point pairs on x values
	double x1, y1, x2, y2, x3, y3, x4, y4;
	double m1, m2;
	double x;
	bool mCheck1 = false;
	bool mCheck2 = false;
	// Pair 1
	if (pt1[0]<pt2[0]) {
		x1 = pt1[0];
		y1 = pt1[1];
		x2 = pt2[0];
		y2 = pt2[1];
	} else {
		x1 = pt2[0];
		y1 = pt2[1];
		x2 = pt1[0];
		y2 = pt1[1];
	}
	// Pair 2
	if (pt3[0]<pt3[1]) {
		x3 = pt3[0];
		y3 = pt3[1];
		x4 = pt4[0];
		y4 = pt4[1];
	} else {
		x3 = pt4[0];
		y3 = pt4[1];
		x4 = pt3[0];
		y4 = pt3[1];
	}
	// Check if ranges intersect
	if ((x2<x3) || (x1>x4)) {
		// X range doesn't intersect
		return false;
	}
	if ((std::max(y1,y2)<std::min(y3,y4)) || (std::min(y1,y2)>std::max(y3,y4))) {
		// Y Range doesn't intersect
		return false;
	}
	// Calculate Gradients
	if (x2 != x1) {
		m1 = (y2-y1)/(x2-x1);
	} else {
		mCheck1 = true;
	}
	if (x3 != x4) {
		m2 = (y4-y3)/(x4-x3);
	} else {
		mCheck2 = true;
	}
	// Find Intersection
	if ((!mCheck1) && (!mCheck2)) {
		if (m1==m2) {
			// Lines are parallel, no intersection
			return false;
		} else {
			// Lines intersect, calculate Intersection Point
			x = (y3-y1)/((m1-m2)*((m2*x3)-(m1*x1)));
		}
	} else if (mCheck1 && mCheck2) {
		// Both lines are vertical, no intersection
		return false;
	} else if (mCheck1) {
		// Line 1 is vertical
		x = x1;
	} else if (mCheck2) {
		// Line 2 is vertical
		x = x3;
	}

	// Check if intersection is in domain
	double xmin = std::max(x1,x3);
	double xmax = std::min(x2,x4);
	if (x>xmin && x<xmax) {
		// Lines intersect in domain
		return true;
	} else {
		// Lines intersect outside domain
		return false;
	}
}

std::vector<std::vector<int>> Volume::triangulatePolygon() {
	// Triangulates the given polygon by providing three indices per triangle
	// Uses the Ear Clipping Method
	// Creating shrinking polygon
	std::vector<std::vector<double>> currPoly;
	std::vector<std::vector<int>> triangles;
	for (unsigned int i=0; i<pts.size(); i++) {
		std::vector<double> vec = {pts[i][0],pts[i][1],pts[i][2],pts[i][3],i};
		currPoly.push_back(vec);
	}

	unsigned int i = 1; // Current Point
	std::vector<double> pt1, pt2, pt3, pt4, ptc;
	while (currPoly.size()>3) {
		// Form triangle from neighbouring points
		if (i==0) {
			pt1 = currPoly[currPoly.size()-1];
			ptc = currPoly[i];
			pt2 = currPoly[i+1];
		} else if (i==currPoly.size()-1) {
			pt1 = currPoly[i-1];
			ptc = currPoly[i];
			pt2 = currPoly[0];
		} else {
			pt1 = currPoly[i-1];
			ptc = currPoly[i];
			pt2 = currPoly[i+1];
		}
		// Create line joining neighbouring points
		// Check if this line crosses any other lines in the current polygon
		int count = 0;
		for(unsigned int j=0; j<currPoly.size()-2; j++) {
			pt3 = currPoly[j];
			pt4 = currPoly[j+1];
			if (linesCross(pt1, pt2, pt3, pt4)) {
				count += 1;
			}
		}
		if (count == 0) {
			// Lines don't intersect, these three points form an ear
			// Store triangle
			std::vector<int> tri = {pt1[4],ptc[4],pt2[4]};
			triangles.push_back(tri);
			// Remove point
			currPoly.erase(currPoly.begin() + i);
		}
		i += 1;
		if (i>currPoly.size()-1) {
			i = 0;
		}
	}
	// Add last triangle
	pt1 = currPoly[0];
	ptc = currPoly[1];
	pt2 = currPoly[2];
	std::vector<int> tri = {pt1[4],ptc[4],pt2[4]};
	triangles.push_back(tri);

	return triangles;
}

void Volume::Draw(Shader shader) {
	// Set colour
	glUniform3f(glGetUniformLocation(shader.Program,"color"),rgb[0],rgb[1],rgb[2]);

	// Draw Triangles
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);
	glBindVertexArray(0);
}

void Volume::DrawLines(Shader lineShader) {
	// Draw Lines
	glm::vec4 inColor = glm::vec4(0.0,0.0,0.0,1.0);
	glUniform4fv(glGetUniformLocation(lineShader.Program,"inColor"),1,glm::value_ptr(inColor));
	glBindVertexArray(lVAO);
	glDrawArrays(GL_LINE_LOOP,0,pts.size());
	glDrawArrays(GL_LINE_LOOP,pts.size(),pts.size());
	glDrawArrays(GL_LINES,2*pts.size(),2*pts.size());
	glBindVertexArray(0);
}


void Volume::createAndSetupBuffers() {
	/* Triangles */
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
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(GLfloat),(GLvoid*)0);
	glBindVertexArray(0); // Unbind lVAO

	/* Lines */
	/* Create Buffers */
	glGenVertexArrays(1,&lVAO);
	glGenBuffers(1,&lVBO);

	/* Setup Buffers */
	glBindVertexArray(lVAO);
	glBindBuffer(GL_ARRAY_BUFFER,lVBO);
	glBufferData(GL_ARRAY_BUFFER, lineVerts.size()*sizeof(GLfloat),&lineVerts[0],GL_STATIC_DRAW);

	/* Position Attributes */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindVertexArray(0); // Unbind lVAO
}

/* Constructor */
VolumeList::VolumeList(Camera* cameraPt) {
	this->cameraPt = cameraPt;
}

void VolumeList::addVolume(Volume volume) {
	// Adds volume to list
	volumeList.push_back(volume);
}

void VolumeList::sortByMaxDistance() {
	// Sorts the volumes based on the distance to the furthest point in each volume
	// Get distances
	std::vector<float> dist;
	for(unsigned int i=0; i<volumeList.size(); i++) {
		std::vector<float> ptDist;
		for(unsigned int j=0; j<volumeList[i].vertices.size()/3.0; j++) {
			float dist = sqrt(pow(cameraPt->Position[0]-volumeList[i].vertices[3*j],2)+
							  pow(cameraPt->Position[1]-volumeList[i].vertices[3*j+1],2)+
							  pow(cameraPt->Position[2]-volumeList[i].vertices[3*j+2],2));
			ptDist.push_back(dist);
		}
		double max = *max_element(ptDist.begin(), ptDist.end());
		dist.push_back(max);
	}
	// Sort by distance
	std::vector<Volume> tempVol = volumeList;
	volumeList = {};
	while(tempVol.size()>0) {
		int maxInd = 0;
		for(unsigned int i=0; i<tempVol.size(); i++) {
			if (dist[i]>dist[maxInd]) {
				maxInd = i;
			}
		}
		// Store maximum
		volumeList.push_back(tempVol[maxInd]);
		tempVol.erase(tempVol.begin()+maxInd);
		dist.erase(dist.begin()+maxInd);
	}
}

void VolumeList::Draw(Shader shader) {
	// Draws all volumes
	// Sort volumes by distance
	sortByMaxDistance();
	for(unsigned int i=0; i<volumeList.size(); i++) {
		volumeList[i].Draw(shader);
	}
}

void VolumeList::DrawLines(Shader shader) {
	// Draws all volumes lines
	for(unsigned int i=0; i<volumeList.size(); i++) {
		volumeList[i].DrawLines(shader);
	}
}

