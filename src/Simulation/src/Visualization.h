//
// Created by stan on 13/02/25.
//

#ifndef SIMULATION_VISUALIZATION_H
#define SIMULATION_VISUALIZATION_H

#include <glm/glm.hpp>

namespace simulation {
    inline glm::mat4 ProjectionMatrix;
    inline glm::mat4 ViewMatrix;
    inline glm::vec3 LightPosition;

    void project(float fovy = 45.0f, float aspectRatio = 0, float zNear = 0.1f, float zFar = 2500.0f);
    void lookAt(glm::vec3 eye, glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
    void lightPosition(const glm::vec3& position);

    void refresh();
    bool needClose();
    void setBackgroundColor(float r, float g, float b);
} // simulation

#endif //SIMULATION_VISUALIZATION_H
