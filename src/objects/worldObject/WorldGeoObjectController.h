//
// Created by bcub3d-desktop on 22/1/20.
//

#ifndef OPENGLMAP_WORLDGEOOBJECTCONTROLLER_H
#define OPENGLMAP_WORLDGEOOBJECTCONTROLLER_H


// GL Includes
#include <GL/glew.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standard Includes
#include <map>
#include <vector>
#include <memory>

// Project Includes
#include "WorldGeoObjectModel.h"
#include "WorldObjectView.h"


class WorldGeoObjectController {
public:
    /* Constructor */
    WorldGeoObjectController(const GLchar* path, glm::dvec3 origin);

    /* Functions */
    void draw(Shader shader);
    void updateGeoPosition(glm::dvec3 position, glm::dvec3 attitude);
    glm::dvec3 getGeoPosition();
    std::shared_ptr<WorldGeoObjectModel> getWorldGeoObjectModel();

private:
    /* Models */
    WorldGeoObjectModel worldGeoObjectModel;

    /* Views */
    WorldObjectView worldObjectView;

};


#endif //OPENGLMAP_WORLDGEOOBJECTCONTROLLER_H
