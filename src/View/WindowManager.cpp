#include "IWindowManager.h"

#include <bits/algorithmfwd.h>


namespace ed = ax::NodeEditor;

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
    std::string textInput;
    std::vector<LinkInfo> nodeLinkInfos;


public:
    void SetupImGui(GLFWwindow *existingWindow) override {
        window = existingWindow;

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW\n";
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");


        std::cout << "Window initialized successfully\n";
    }

    void RenderUI(ed::EditorContext *g_Context) override {
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render both UI windows
        RenderSingleInstructionWindow();
        RenderImGuiNodesEditor(g_Context);

        // Render the final ImGui frame
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    void RenderSingleInstructionWindow() {
        bool showWindow = true;
        ImGui::Begin("Single instruction", &showWindow);
        ImGui::InputInt("X", &inputX);
        ImGui::InputInt("Y", &inputY);
        ImGui::InputInt("Z", &inputZ);
        ImGui::SliderAngle("X Degrees", &inputXDegrees, -360, 360);
        ImGui::SliderAngle("Y Degrees", &inputYDegrees, -360, 360);
        ImGui::SliderAngle("Z Degrees", &inputZDegrees, -360, 360);
        ImGui::SliderInt("Grip Force", &gripforce, 0, 10);

        if (ImGui::Button("Show values")) {
            textInput = "X = " + std::to_string(inputX) +
                        ", Y = " + std::to_string(inputY) +
                        ", Z = " + std::to_string(inputZ) +
                        " X Degrees = " + std::to_string(inputXDegrees) +
                        " Y Degrees = " + std::to_string(inputYDegrees) +
                        " Z Degrees = " + std::to_string(inputZDegrees);
        }
        ImGui::Text(textInput.c_str());
        ImGui::End();
    }

    static void createNode(int &uniqueId, const std::string& name) {
        ed::BeginNode(uniqueId++);
        ImGui::Text(name.c_str());
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ed::EndNode();
    }

    void RenderLinks() {
        for (const auto &link : nodeLinkInfos) {
            ed::Link(link.id, link.inputPin, link.outputPin);
        }
    }

    static void RenderImGuiNodesEditor(ed::EditorContext *g_Context) {
        if (!g_Context) {
            std::cerr << "Error: g_Context is null!" << std::endl;
            return;
        }

        ImGui::Begin("Node Editor");
        ed::SetCurrentEditor(g_Context);
        ed::Begin("My Editor");

        // Ensure nodes exist before linking
        static int uniqueId = 1;
        createNode(uniqueId, "Move 1");
        createNode(uniqueId, "Move 2");

        ed::End();
        ed::SetCurrentEditor(nullptr);
        ImGui::End();
    }


public:
    void CleanupImGui(ed::EditorContext *g_Context) override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        DestroyEditor(g_Context);
    }
};
