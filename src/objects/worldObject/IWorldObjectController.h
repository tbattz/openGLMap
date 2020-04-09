//
// Created by bcub3d-desktop on 22/3/20.
//

#ifndef OPENGLMAP_IWORLDOBJECTCONTROLLER_H
#define OPENGLMAP_IWORLDOBJECTCONTROLLER_H

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Project Includes
#include <renderEngine/shader.h>


class IWorldObjectController {
public:
    IWorldObjectController();

    virtual void draw(Shader shader) = 0;
    virtual glm::dvec3 getPosition() = 0;
};


#endif //OPENGLMAP_IWORLDOBJECTCONTROLLER_H
