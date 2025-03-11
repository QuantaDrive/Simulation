#include "IWindowManager.h"

#include "../Domain/Node.h"
#include "../Domain/NodeActivation.h"
#include "../Domain/Instruction.h"
#include "helper/HelperFunctions.h"
namespace ed = ax::NodeEditor;

using namespace domain;


class WindowManager : public IWindowManager {
    GLFWwindow *window = nullptr;
    // Single instruction Window
    int gripforce = 0;
    std::string textInput;

    // Render Nodes Window
    ImVector<LinkInfo> m_Links;
    std::vector<domain::Node> m_Nodes;
    bool m_FirstFrame = true;
    int m_NextLinkId = 100;
    ImVec2 m_NextNodePosition = ImVec2(0, 0);
    int m_NextNodeId = 1;

    // Window states
    bool m_ShowNodeSelector = true;
    bool m_ShowSingleInstruction = true;
    bool m_ShowNodeEditor = true;

    //Simulationmanager
    SimulationManager *localSimulationManager;


   void RenderNodeSelectorWindow() {
    ImGui::Begin("Node Selector", &m_ShowNodeSelector, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar()) {
        ImGui::EndMenuBar();
    }

    if (!ImGui::IsWindowCollapsed()) {
        // Calculate the widest label to set consistent spacing
        float maxLabelWidth = 0.0f;
        for (int i = 0; i < static_cast<int>(RobotActions::NodeActivation::COUNT); ++i) {
            auto action = static_cast<RobotActions::NodeActivation>(i);
            std::string label = RobotActions::toString(action).data();
            maxLabelWidth = std::max(maxLabelWidth, ImGui::CalcTextSize(label.c_str()).x);
        }

        maxLabelWidth += 20.0f;

        for (int i = 0; i < static_cast<int>(RobotActions::NodeActivation::COUNT); ++i) {
            auto action = static_cast<RobotActions::NodeActivation>(i);
            std::string label = RobotActions::toString(action).data();

            ImGui::Text("%s", label.c_str());
            ImGui::SameLine(maxLabelWidth);

            if (ImGui::Button(("+##" + label).c_str())) {
                domain::Node newNode(label.c_str(), action);
                int currentId = m_NextNodeId;
                newNode.InitializeNodeIds(currentId);
                m_NextNodeId = currentId;

                switch (action) {
                    case RobotActions::NodeActivation::LoopStart: {
                        int loopCount = 1;
                        ImGui::InputInt("Loop Count", &loopCount);
                        newNode.SetLoopCount(loopCount);
                        break;
                    }
                    case RobotActions::NodeActivation::Wait: {
                        int waitTimer = 1;
                        ImGui::InputInt("Wait Timer", &waitTimer);
                        newNode.SetWaitTimer(waitTimer);
                        break;
                    }
                    default:
                        break;
                }

                m_Nodes.push_back(newNode);

                float randomXNumber = (rand() % 100) - 30;
                float randomYNumber = (rand() % 100) - 30;
                m_NextNodePosition.x += randomXNumber;
                m_NextNodePosition.y += randomYNumber;
            }
            ImGui::SameLine();

            if (ImGui::Button(("-##" + label).c_str())) {
                for (auto it = m_Nodes.rbegin(); it != m_Nodes.rend(); ++it) {
                    if (it->GetActivation() == action) {
                        m_Nodes.erase((++it).base());
                        break;
                    }
                }
            }
        }
    }

    ImGui::End();
}

    void RenderSingleInstructionWindow() {
        ImGui::Begin("Position Window", &m_ShowSingleInstruction, ImGuiWindowFlags_MenuBar);

        // Add minimize/maximize in the menu bar
        if (ImGui::BeginMenuBar()) {
            ImGui::EndMenuBar();
        }

        // Only show content if window is not collapsed
        if (!ImGui::IsWindowCollapsed()) {
            ImGui::SliderInt("Grip Force", &gripforce, 0, 10);

            if (ImGui::Button("Send instructions")) {
                ExecuteNodeChain();
            }
            ImGui::Text(textInput.c_str());
        }

        ImGui::End();
    }

    void RenderImGuiNodesEditorWindow(ed::EditorContext *g_Context) {
        ImGui::Begin("Node Editor", &m_ShowNodeEditor, ImGuiWindowFlags_MenuBar);

        // Add minimize/maximize in the menu bar
        if (ImGui::BeginMenuBar()) {
            ImGui::EndMenuBar();
        }

        if (!ImGui::IsWindowCollapsed()) {
            ed::SetCurrentEditor(g_Context);
            ed::Begin("My Editor");

            // Render all stored nodes
            for (auto &node: m_Nodes) {
                RenderNodesInEditor(node);
            }
            // Submit Links
            for (auto &linkInfo: m_Links) {
                ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);
            }

            // Handle creation action, returns true if editor want to create new object (node or link)
            if (ed::BeginCreate()) {
                LinkHandler();
            }
            ed::EndCreate();


            // Handle deletion action
            if (ed::BeginDelete()) {
                // There may be many links marked for deletion, let's loop over them.
                LinkDeleteHandler();
            }
            ed::EndDelete();

            if (m_FirstFrame) {
                ed::NavigateToContent(0.0f);
            }

            ed::End();
            ed::SetCurrentEditor(nullptr);
        }
        m_FirstFrame = false;
        ImGui::End();
    }

    void LinkHandler() {
        ed::PinId inputPinId, outputPinId;
        if (ed::QueryNewLink(&inputPinId, &outputPinId)) {
            if (inputPinId && outputPinId) {
                // Validate connection between pins
                // Can't connect input to input or output to output
                // Can't create self-loops
                bool validateLink = true;

                // Find source and target nodes
                domain::Node *sourceNode = nullptr;
                domain::Node *targetNode = nullptr;
                for (auto &node: m_Nodes) {
                    if (node.GetNodeOutputPinId() == outputPinId ||
                        node.GetNodeInputPinId() == inputPinId) {
                        sourceNode = &node;
                    }
                    if (node.GetNodeInputPinId() == inputPinId ||
                        node.GetNodeOutputPinId() == outputPinId) {
                        targetNode = &node;
                    }
                }

                // Validate connection
                if (sourceNode && targetNode) {
                    // Check for self-loops
                    if (sourceNode == targetNode) {
                        validateLink = false;
                    }

                    // Ensure input connects to output
                    if (outputPinId == sourceNode->GetNodeInputPinId() ||
                        inputPinId == targetNode->GetNodeOutputPinId()) {
                        validateLink = false;
                    }
                }

                if (validateLink && ed::AcceptNewItem()) {
                    // Create new link
                    m_Links.push_back({ed::LinkId(m_NextLinkId++), inputPinId, outputPinId});
                    ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
                }
            }
        }
    }

    void LinkDeleteHandler() {
        ed::LinkId deletedLinkId;
        while (ed::QueryDeletedLink(&deletedLinkId)) {
            // If you agree that link can be deleted, accept deletion.
            if (ed::AcceptDeletedItem()) {
                // Then remove link from your data.
                for (auto &link: m_Links) {
                    if (link.Id == deletedLinkId) {
                        m_Links.erase(&link);
                        break;
                    }
                }
            }
        }
    }


    void RenderNodesInEditor(domain::Node &node) {
        ed::BeginNode(node.GetNodeId());
        ImGui::Text(node.GetTitle().c_str());

        // Add relative movement controls for Relative nodes
        if (node.GetActivation() == RobotActions::NodeActivation::Relative) {
            ImGui::PushItemWidth(100);

            // Get current relative move values
            glm::vec3 relMove = node.GetRelativeMove();
            float x = relMove.x;
            float y = relMove.y;
            float z = relMove.z;

            std::string idTitle = "absolute_move " + node.GetNodeId().Get();
            ImGui::PushID(idTitle.c_str());
            // Create sliders for X, Y, Z
            bool changed = false;
            changed |= ImGui::SliderFloat("X", &x, -200.0f, 200.0f);
            changed |= ImGui::SliderFloat("Y", &y, -200.0f, 200.0f);
            changed |= ImGui::SliderFloat("Z", &z, -200.0f, 200.0f);

            // Update values if changed
            if (changed) {
                node.SetRelativeMove(x, y, z);
            }

            ImGui::PopID();
            ImGui::PopItemWidth();
        }
        else if (node.GetActivation() == RobotActions::NodeActivation::LoopStart) {
            ImGui::PushItemWidth(100);
            std::string idTitle = "loop " + node.GetNodeId().Get();
            ImGui::PushID(idTitle.c_str());

            ImGui::Text("# loops");
            int loopCount = node.GetLoopCount();
            ImGui::InputInt("", &loopCount);
            ImGui::PopID();
            ImGui::PopItemWidth();
            node.SetLoopCount(loopCount);
        }
        else if (node.GetActivation() == RobotActions::NodeActivation::Wait) {
            std::string idTitle = "wait " + node.GetNodeId().Get();
            ImGui::PushID(idTitle.c_str());
            ImGui::PushItemWidth(100);
            ImGui::Text("ms");
            int waitTimer = node.GetWaitTimer();
            ImGui::InputInt("", &waitTimer);

            ImGui::PopID();
            ImGui::PopItemWidth();
            node.SetWaitTimer(waitTimer);

        }
        // Add absolute movement controls for Absolute nodes
        else if(node.GetActivation() == RobotActions::NodeActivation::Absolute){
            ImGui::PushItemWidth(100);

            // Get current relative move values
            glm::vec3 absMove = node.GetAbsolutePosition()->getCoords();
            float x = absMove.x;
            float y = absMove.y;
            float z = absMove.z;

            std::string idTitle = "absolute_move " + node.GetNodeId().Get();
            ImGui::PushID(idTitle.c_str());

            // Create sliders for X, Y, Z
            bool changed = false;
            changed |= ImGui::SliderFloat("X", &x, -400.0f, 800.0f);
            changed |= ImGui::SliderFloat("Y", &y, -400.0f, 800.0f);
            changed |= ImGui::SliderFloat("Z", &z, -400.0f, 800.0f);

            // Update values if changed
            if (changed) {
                auto* position = new domain::Position(vec3(x, y, z), vec3(0, 0, 0));
                node.SetAbsolutePosition(position);
            }
            ImGui::PopID();
            ImGui::PopItemWidth();
        }
        ed::BeginPin(node.GetNodeInputPinId(), ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(node.GetNodeOutputPinId(), ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ed::EndNode();
    }


    void ExecuteNodeChain() {
        const domain::Node *startNode = NodeHelpers::FindStartNode(m_Nodes, m_Links);
        if (!startNode) {
            std::cout << "No starting node found!" << std::endl;
            return;
        }

        const domain::Node *currentNode = startNode;
        while (currentNode) {
            ExecuteNode(*currentNode);
            currentNode = NodeHelpers::FindNextNode(currentNode, m_Nodes, m_Links);
        }
    }

    void ExecuteNode(const domain::Node &node) {
        if (!localSimulationManager) return;

        switch (node.GetActivation()) {
            case RobotActions::NodeActivation::Relative: {
                // Create an instruction with the relative movement values
                auto *instruction = new domain::Instruction();
                instruction->setRelative(true);
                instruction->setRelMove(node.GetRelativeMove());
                // Execute the instruction using SimulationManager
                localSimulationManager->executeInstruction(instruction);

                // Cleanup
                delete instruction;
                break;
            }
            case (RobotActions::NodeActivation::Absolute): {
                auto *instruction = new Instruction();
                instruction->setRelative(false);
                instruction->setPosition(node.GetAbsolutePosition());
                localSimulationManager->executeInstruction(instruction);
                delete instruction;
                break;
            }
            case(RobotActions::NodeActivation::Wait): {
                auto *instruction = new Instruction();
                instruction->setRelative(false);
                instruction->setWait(node.GetWaitTimer());
                localSimulationManager->executeInstruction(instruction);
                delete instruction;
                break;
            }
            case(RobotActions::NodeActivation::Home): {
                auto *instruction = new Instruction();
                instruction->setRelative(false);
                instruction->setGoHome(true);
                localSimulationManager->executeInstruction(instruction);
                delete instruction;
                break;
            }
            default:
                break;
        }
    }

public:

    explicit WindowManager(SimulationManager *simulationManager) {
        localSimulationManager = simulationManager;
    }


    void SetupImGui(GLFWwindow *existingWindow) override {
        window = existingWindow;

        // Set window user pointer for callbacks
        glfwSetWindowUserPointer(window, this);

        // Add input callbacks
        glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
            auto* manager = static_cast<WindowManager*>(glfwGetWindowUserPointer(w));
            if (button == GLFW_MOUSE_BUTTON_LEFT && !ImGui::GetIO().WantCaptureMouse) {
                manager->localSimulationManager->setDragActive(action == GLFW_PRESS);
            }
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* w, double xpos, double ypos) {
            auto* manager = static_cast<WindowManager*>(glfwGetWindowUserPointer(w));
            if (!ImGui::GetIO().WantCaptureMouse) {
                manager->localSimulationManager->handleMouseDrag(xpos, ypos);
            }
        });

        glfwSetScrollCallback(window, [](GLFWwindow* w, double xoffset, double yoffset) {
            auto* manager = static_cast<WindowManager*>(glfwGetWindowUserPointer(w));
            if (!ImGui::GetIO().WantCaptureMouse) {
                manager->localSimulationManager->handleMouseScroll(yoffset);
            }
        });

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

        // Menu to toggle windows
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Windows")) {
                ImGui::MenuItem("Node Selector", nullptr, &m_ShowNodeSelector);
                ImGui::MenuItem("Single Instruction", nullptr, &m_ShowSingleInstruction);
                ImGui::MenuItem("Node Editor", nullptr, &m_ShowNodeEditor);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Render windows based on their visibility flags
        if (m_ShowSingleInstruction) {
            RenderSingleInstructionWindow();
        }
        if (m_ShowNodeSelector) {
            RenderNodeSelectorWindow();
        }
        if (m_ShowNodeEditor) {
            RenderImGuiNodesEditorWindow(g_Context);
        }
        // Render the final ImGui frame
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void CleanupImGui(ed::EditorContext *g_Context) override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        ed::DestroyEditor(g_Context);
    }
};
