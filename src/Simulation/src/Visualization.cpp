//
// Created by stan on 13/02/25.
//

#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include "Visualization.h"

#include "Init.h"

void simulation::setBackgroundColor(float r, float g, float b)
{
    glClearColor(r, g, b, 0.0f);
}

void simulation::refresh()
{
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}
