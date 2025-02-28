//
// Created by stan on 12/02/25.
//

#ifndef SIMULATION_INIT_H
#define SIMULATION_INIT_H

#include <GLFW/glfw3.h>

namespace simulation
{
    inline GLuint programID;
    inline GLFWwindow* window;
    inline GLuint ModelMatrixID;
    inline GLuint ViewMatrixID;
    inline GLuint MvpMatrixID;
    inline GLuint LightPosID;

    int Init(int width = 1280, int height = 720);
    int CompileShaders();
    void Close();

    void checkOpenGLError();
} // simulation

#endif //SIMULATION_INIT_H
