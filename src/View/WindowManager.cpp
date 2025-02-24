#include "IWindowManager.h"
#include <bits/algorithmfwd.h>
namespace ed = ax::NodeEditor;

struct LinkInfo {
    ed::LinkId Id;
    ed::PinId StartPinId;
    ed::PinId EndPinId;
};

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
    ImVector<LinkInfo> m_Links;

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
        // Start ImGui frame (only once per frame)
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

    void RenderImGuiNodesEditor(ed::EditorContext *g_Context) {
        ImGui::Begin("Node Editor");
        ed::SetCurrentEditor(g_Context);
        ed::Begin("My Editor");

        int uniqueId = 1;
        // Node A
        // id's
        ed::NodeId nodeA_Id = uniqueId++;
        ed::PinId nodeA_InputPinId = uniqueId++;
        ed::PinId nodeA_OutputPinId = uniqueId++;

        ed::BeginNode(nodeA_Id);
        ImGui::Text("Node A");
        ed::BeginPin(nodeA_InputPinId, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(nodeA_OutputPinId, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ed::EndNode();


        // Handle link creation
        if (ed::BeginCreate())
        {
            if (ed::QueryNewLink(&nodeA_InputPinId, &nodeA_OutputPinId)) {
                if (nodeA_InputPinId && nodeA_OutputPinId) {
                    ed::AcceptNewItem();
                    // Create a new link
                    ed::Link(ed::LinkId(uniqueId++), nodeA_InputPinId, nodeA_OutputPinId);
                }
            }
        }
        ed::EndCreate();


        // Render existing links
        for (auto& link : m_Links) {
            ed::Link(link.Id, link.StartPinId, link.EndPinId);
        }

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
        ed::DestroyEditor(g_Context);
    }
};
