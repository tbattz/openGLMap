//
// Created by bcub3d-desktop on 22/1/20.
//

#ifndef OPENGLMAP_WORLDOBJECTCONTROLLER_H
#define OPENGLMAP_WORLDOBJECTCONTROLLER_H


// GL Includes
#include <GL/glew.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standard Includes
#include <map>
#include <vector>

// Project Includes
#include <model/WorldObjectModel.h>
#include <view/WorldObjectView.h>


class WorldObjectController {
public:
    /* Constructor */
    WorldObjectController(const GLchar* path);

    /* Functions */
    void draw(Shader shader);
    void updatePosition(glm::dvec3 position, glm::dvec3 attitude);

    void incrementPosition();


private:
    /* Models */
    WorldObjectModel worldObjectModel;

    /* Views */
    WorldObjectView worldObjectView;

};


#endif //OPENGLMAP_WORLDOBJECTCONTROLLER_H
