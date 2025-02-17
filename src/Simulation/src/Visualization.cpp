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
    ProjectionMatrix = glm::perspective(glm::radians(fovy), aspectRatio, zNear, zFar);
}

void simulation::lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
    ViewMatrix = glm::lookAt(eye, center, up);
}

void simulation::refresh()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Use our shader
    glUseProgram(programID);
}

void simulation::showFrame()
{
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
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
