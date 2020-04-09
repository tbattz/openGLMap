//
// Created by bcub3d-desktop on 22/1/20.
//

#ifndef OPENGLMAP_WORLDOBJECTCONTROLLER_H
#define OPENGLMAP_WORLDOBJECTCONTROLLER_H


// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standard Includes
#include <map>
#include <vector>
#include <memory>

// Project Includes
#include <objects/worldObject/WorldGeoObjectModel.h>
#include <objects/worldObject/WorldObjectView.h>
#include "IWorldObjectController.h"


class WorldObjectController : public IWorldObjectController {
public:
    /* Constructor */
    WorldObjectController(const GLchar* path);

    /* Functions */
    void draw(Shader shader);
    void updatePosition(glm::dvec3 position, glm::dvec3 attitude);
    glm::dvec3 getPosition();
    std::shared_ptr<WorldObjectModel> getWorldObjectModel();

    void incrementPosition();


private:
    /* Models */
    WorldObjectModel worldObjectModel;

    /* Views */
    WorldObjectView worldObjectView;

};


#endif //OPENGLMAP_WORLDOBJECTCONTROLLER_H
