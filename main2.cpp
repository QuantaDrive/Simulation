#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_node_editor.h>

#include "common/shader.hpp"
#include "src/Simulation/SimulationInit.h"
#include "src/View/WindowManager.cpp"

namespace ed = ax::NodeEditor;

GLFWwindow *window;
ed::EditorContext *g_Context = nullptr;
WindowManager windowManager = WindowManager();



int main() {
    // Initialize the simulation environment

    SimEnviromentInit(&window);

    // Ensure we can capture the escape key being pressed below
    // Dark blue background
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Create the triangle
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("src/Simulation/VertexShader.vertexshader",
                                   "src/Simulation/FragmentShader.fragmentshader");

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Setup ImGui
    windowManager.SetupImGui(window);

    // Setup Node Editor
    ed::Config config;
    config.SettingsFile = "Simple.json";
    g_Context = ed::CreateEditor(&config);

    do {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);

        // Draw the triangle
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        // Render ImGui UI and Node Editor together
        windowManager.RenderUI(g_Context);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);


    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

    // Cleanup ImGui & Node Editor
    windowManager.CleanupImGui(g_Context);


    SimEnviromentClose();

    return 0;
}
