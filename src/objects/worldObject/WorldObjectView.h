//
// Created by bcub3d-desktop on 22/1/20.
//

#ifndef OPENGLMAP_WORLDOBJECTVIEW_H
#define OPENGLMAP_WORLDOBJECTVIEW_H

// GL Includes
#include <GL/glew.h>

// Object Loading Includes
#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

// Project Includes
#include <renderEngine/objectLoading/mesh.h>


class WorldObjectView {
public:
    /* Constructor */
    WorldObjectView(const GLchar* path);

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


#endif //OPENGLMAP_WORLDOBJECTVIEW_H
