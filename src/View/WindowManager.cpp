#include "IWindowManager.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>
#include <bits/algorithmfwd.h>

class WindowManager : public IWindowManager {
private:
    GLFWwindow *window = nullptr;
    int inputX = 0;
    int inputY = 0;
    int inputZ = 0;
    float inputXDegrees = 0;
    float inputYDegrees = 0;
    float inputZDegrees = 0;
    int gripforce = 0;
    std::string textInput = "";

public:
    void init(GLFWwindow *existingWindow) override {
        window = existingWindow;


        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW\n";
            return;
        }

        // Setup ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        std::cout << "Window initialized successfully\n";
    }

    void singleInstructionWindow(bool showWindow) {
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create an ImGui window

        ImGui::Begin("Single instruction", &showWindow);
        // Create an input field for ints
        ImGui::InputInt("X", &inputX);
        ImGui::InputInt("Y", &inputY);
        ImGui::InputInt("Z", &inputZ);
        ImGui::SliderAngle("X Degrees", &inputXDegrees, -360, 360);
        ImGui::SliderAngle("Y Degrees", &inputYDegrees, -360, 360);
        ImGui::SliderAngle("Z Degrees", &inputZDegrees, -360, 360);
        ImGui::SliderInt("Grip Force", &gripforce, 0, 10);



        // Create a button
        if (ImGui::Button("show values")) {
            textInput = "X = " + std::to_string(inputX) +
                        ", Y = " + std::to_string(inputY) +
                        ", Z = " + std::to_string(inputZ) +
                        " X Degrees = " + std::to_string(inputXDegrees) +
                        " Y Degrees = " + std::to_string(inputYDegrees) +
                        " Z Degrees = " + std::to_string(inputZDegrees);
        }
        ImGui::Text(textInput.c_str());
        ImGui::End();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void run() override {
        bool showWindow = true;

        singleInstructionWindow(showWindow);
    }

    void close() override {
        // Cleanup ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }
};
