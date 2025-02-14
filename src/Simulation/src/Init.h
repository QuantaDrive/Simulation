//
// Created by stan on 12/02/25.
//

#ifndef SIMULATIONINIT_H
#define SIMULATIONINIT_H

#include <GLFW/glfw3.h>

namespace simulation
{
    inline GLuint programID;
    inline GLFWwindow* window;

    int Init(int width = 1280, int height = 720);
    int CompileShaders();
    void Close();
} // simulation

#endif //SIMULATIONINIT_H
