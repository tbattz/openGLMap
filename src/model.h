/*
 * model.h
 *
 *  Created on: 1Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef MODEL_H_
#define MODEL_H_

// Standard Includes
#include <map>
using std::string;

// Object Loading Includes
#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

// Project Includes
#include "mesh.h"



class Model {
public:
	/* Constructor */
	Model(const GLchar* path);

	/* Functions */
	void Draw(Shader shader);

private:
	/* Model Data */
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded; // Store textures once

	/* Functions */
	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	GLint TextureFromFile(const char* path, string directory);

};



#endif /* MODEL_H_ */
