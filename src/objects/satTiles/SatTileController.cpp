//
// Created by bcub3d-desktop on 26/1/20.
//

#include "SatTileController.h"

/* Constructor */
SatTileController::SatTileController(glm::vec3 origin, int x, int y, int zoom, std::string filename) : model(origin, x, y, zoom ) {
    view = SatTileView();
    view.setupTile(model.getXYOffset(), filename);

}

void SatTileController::draw(Shader shader) {
    // Calculate new position matrix
    glm::mat4 tilePos = glm::mat4(1.0f);
    glm::dvec3 position = model.getPosition();
    GLfloat brightness = model.getBrightness();
    tilePos = glm::translate(tilePos, glm::vec3(position[0],0.0f,position[1]));

    // Update Uniforms
    glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE,glm::value_ptr(tilePos));
    glUniform1f(glGetUniformLocation(shader.Program,"brightness"),brightness);

    // Draw Model
    this->view.Draw(shader);
}
