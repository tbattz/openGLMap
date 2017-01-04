/*
 * model.h
 *
 *  Created on: 1Jan.,2017
 *      Author: bcub3d-desktop
 */

#ifndef MODEL_H_
#define MODEL_H_

// Standard Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

GLint TextureFromFile(const char* path, string directory);

class Model {
public:
	/* Functions */
	Model(GLchar* path) {
		this->loadModel(path);
	}

	// Draw the model and its meshes
	void Draw(Shader shader) {
		for(GLuint i = 0; i < this->meshes.size(); i++) {
			this->meshes[i].Draw(shader);
		}
	}

private:
	/* Model Data */
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded; // Store textures once

	/* Functions */
	// Loads model from file using assimp, stores resulting meshes
	void loadModel(string path) {
		// Read using assimp
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		// Check for errors
		if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << '\n';
			return;
		}
		// Get directory from path
		this->directory = path.substr(0,path.find_last_of('/'));

		// Process assimp root node recursively
		this->processNode(scene->mRootNode,scene);
	}

	// Process node using recursion, looking for meshes
	void processNode(aiNode* node, const aiScene* scene) {
		// Process meshes in the current node
		for(GLuint i=0; i < node->mNumMeshes; i++) {
			// Node only has index for data, scene holds the data
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			this->meshes.push_back(this->processMesh(mesh,scene));
		}
		// Recursively process child nodes
		for(GLuint i = 0; i<node->mNumChildren; i++) {
			this->processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		// Mesh Data
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;

		// Loop over each vertex
		for(GLuint i = 0; i<mesh->mNumVertices; i++) {
			// Placeholders
			Vertex vertex;
			glm::vec3 vector;
			// Transfer data from assimp to our vector class
			// Positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// Normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// Texture Coordinates
			if(mesh->mTextureCoords[0]) {
				glm::vec2 vec;
				// Take the first set of texture coordinates
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			} else {
				vertex.TexCoords = glm::vec2(0.0f,0.0f);
			}
			vertices.push_back(vertex);
		}
		// Loop over each face
		for(GLuint i=0; i<mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			// Get all indices of the face, store in indices vector
			for(GLuint j = 0; j<face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		// Process Materials
		if(mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
            // Same applies to other texture as the following list summarizes:
            // Diffuse: texture_diffuseN
            // Specular: texture_specularN
            // Normal: texture_normalN

			// 1. Diffuse maps
			vector<Texture> diffuseMaps = this->loadMaterialTextures(material,aiTextureType_DIFFUSE,"texture_diffuse");
			textures.insert(textures.end(),diffuseMaps.begin(),diffuseMaps.end());
			// 2. Specular maps
			vector<Texture> specularMaps = this->loadMaterialTextures(material,aiTextureType_SPECULAR,"texture_specular");
			textures.insert(textures.end(),specularMaps.begin(),specularMaps.end());
		}

		// Return mesh object
		return Mesh(vertices,indices,textures);
	}

	// Check material textures are a given type and loads them if not already loaded
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
		vector<Texture> textures;
		for(GLuint i=0; i<mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type,i,&str);
			// Check if texture was loaded previously
			GLboolean skip = false;
			for(GLuint j=0; j<textures_loaded.size(); j++) {
				if(textures_loaded[j].path == str) {
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if(!skip) {
				// Load Texture
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(),this->directory);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				this->textures_loaded.push_back(texture);
			}
		}
		return textures;
	}
};


GLint TextureFromFile(const char* path, string directory) {
	// Generate texture ID and load texture data
	string filename = string(path);
	filename = directory + '/' + filename;
	// Create Texture
	GLuint textureID;
	glGenTextures(1,&textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(),&width,&height,0,SOIL_LOAD_RGB);
	// Bind texture and load image data
	glBindTexture(GL_TEXTURE_2D,textureID);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// Unbind texture object and free image memory
	glBindTexture(GL_TEXTURE_2D,0);
	SOIL_free_image_data(image);

	return textureID;
}



#endif /* MODEL_H_ */
