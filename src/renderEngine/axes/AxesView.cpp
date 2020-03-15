//
// Created by bcub3d-desktop on 14/3/20.
//

#include "AxesView.h"

AxesView::AxesView() {
    /* Calculate points */
    calculatePts();
    /* Create and Setup Buffers */
    createAndSetupBuffers();
}

void AxesView::Draw(Shader shader) {
    // Update Uniforms
    glm::mat4 axesPos = glm::mat4(1.0f);
    axesPos = glm::translate(axesPos, glm::vec3(0.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program,"model"),1,GL_FALSE, glm::value_ptr(axesPos));

    // Update Line Data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, pts.size()*sizeof(GLfloat), &pts[0]);
    glLineWidth(2);
    // Draw axes lines in color
    shader.Use();
    for(int i=0; i<3; i++) {
        glUniform4fv(glGetUniformLocation(shader.Program, "inColor"), 1, glm::value_ptr(colors[i]));
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 2*i, 2);
    }
    glBindVertexArray(0);
}

void AxesView::calculatePts() {
    // x axes
    pts.insert(pts.end(), {0.0f, 0.0f , 0.0f});
    pts.insert(pts.end(), {len*1.0f, 0.0f, 0.0f});
    // y axes
    pts.insert(pts.end(), {0.0f, 0.0f , 0.0f});
    pts.insert(pts.end(), {0.0f, len*1.0f, 0.0f});
    // z axes
    pts.insert(pts.end(), {0.0f, 0.0f , 0.0f});
    pts.insert(pts.end(), {0.0f, 0.0f, len*1.0f});
}

void AxesView::createAndSetupBuffers() {
    /* Create Buffers */
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    /* Setup Buffers */
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    glBufferData(GL_ARRAY_BUFFER, pts.size()*sizeof(GLfloat), &pts[0], GL_STATIC_DRAW);

    /* Position Attributes */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);

    glBindVertexArray(0); // Unbind VAO
}
