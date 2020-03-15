//
// Created by bcub3d-desktop on 13/3/20.
//

#ifndef OPENGLMAP_SIMPLEOBJECTCONTROLLER_H
#define OPENGLMAP_SIMPLEOBJECTCONTROLLER_H

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
#include <objects/worldObject/WorldGeoObjectModel.h>
#include <objects/worldObject/SimpleSquareView.h>


class SimpleObjectController {
public:
    /* Constructor */
    SimpleObjectController(glm::dvec3 position, const char* filename);

    /* Functions */
    void draw(Shader shader);
    void updatePosition(glm::dvec3 position, glm::dvec3 attitude);
    glm::dvec3 getPosition();
    std::shared_ptr<WorldObjectModel> getWorldObjectModel();

private:
    /* Models */
    WorldObjectModel worldObjectModel;

    /* Views */
    SimpleSquareView simpleSquareView;

};


#endif //OPENGLMAP_SIMPLEOBJECTCONTROLLER_H
