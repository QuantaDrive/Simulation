//
// Created by stan on 13/02/25.
//

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Visualization.h"

#include "Init.h"

void simulation::project(float fovy, float aspectRatio, float zNear, float zFar)
{
    if (roundf(aspectRatio) == 0)
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }
    ProjectionMatrix = glm::perspective(glm::radians(fovy), aspectRatio, zNear, zFar);
}

void simulation::lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
    ViewMatrix = glm::lookAt(eye, center, up);
}

void simulation::lightPosition(const glm::vec3& position)
{
    LightPosition = position;
}

void simulation::refresh()
{
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Use our shader
    glUseProgram(programID);

    // Fill matrices
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    glUniform3fv(LightPosID, 1, &LightPosition[0]);
}

bool simulation::needClose()
{
    return glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0;
}

void simulation::setBackgroundColor(float r, float g, float b)
{
    glClearColor(r, g, b, 0.0f);
}
