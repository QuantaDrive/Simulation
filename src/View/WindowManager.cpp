#include "IWindowManager.h"
#include <bits/algorithmfwd.h>

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
    bool m_ShowNodeEditor = true;
    bool m_ShowHelpWindow = false;

    //Simulationmanager
    SimulationManager *localSimulationManager;



    void RenderHelpWindow() {
        if (!m_ShowHelpWindow) return;
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, 300), ImVec2(600, 800));
        ImGui::Begin("Node Editor Help", &m_ShowHelpWindow);
        NodeHelpers::RenderHelpText();
        ImGui::End();
    }


    void RenderNodeSelectorWindow() {
        // Set max width to 250
        ImGui::SetNextWindowSizeConstraints(ImVec2(200, 0), ImVec2(250, FLT_MAX));
        ImGui::Begin("Node Selector", &m_ShowNodeSelector, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar()) {
            ImGui::EndMenuBar();
        }

        if (!ImGui::IsWindowCollapsed()) {
            // Make buttons fill the width
            float windowWidth = ImGui::GetContentRegionAvail().x;

            for (int i = 0; i < static_cast<int>(RobotActions::NodeActivation::COUNT); ++i) {
                auto action = static_cast<RobotActions::NodeActivation>(i);
                std::string label = toString(action).data();

                // Set button width to match window width
                ImGui::PushItemWidth(windowWidth);
                if (ImGui::Button(label.c_str(), ImVec2(windowWidth, 0))) {
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

                    NodeHelpers::CalcRandomPosNextNode( &m_NextNodePosition);
                }
                // Add tooltip for each button
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    NodeHelpers::RenderNodeTooltip(action);
                    ImGui::EndTooltip();
                }
                ImGui::PopItemWidth();
            }
            // Add spacing before the Send instructions button
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();


            // Style the Send instructions button differently
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));

            if (ImGui::Button("Send instructions", ImVec2(windowWidth, 0))) {
                ExecuteNodeChain();
            }

            // Add tooltip for Send instructions button
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Execute the current node sequence");
                ImGui::Text("Nodes must be properly connected");
                ImGui::EndTooltip();
            }

            ImGui::PopStyleColor(3);
        }
        ImGui::End();
    }

private:
    // Add these as private member functions in the WindowManager class
    static bool compareNodeById(const domain::Node &node, ed::NodeId nodeId) {
        return node.GetNodeId() == nodeId;
    }

    bool shouldRemoveLink(const LinkInfo &link, const domain::Node &node) const {
        return link.InputId == node.GetNodeInputPinId() ||
               link.OutputId == node.GetNodeOutputPinId();
    }

    struct NodeIdMatcher {
        explicit NodeIdMatcher(ed::NodeId id) : nodeId(id) {
        }

        bool operator()(const domain::Node &node) const {
            return node.GetNodeId() == nodeId;
        }

        ed::NodeId nodeId;
    };

    struct LinkNodeMatcher {
        explicit LinkNodeMatcher(const domain::Node &n) : node(n) {
        }

        bool operator()(const LinkInfo &link) const {
            return link.InputId == node.GetNodeInputPinId() ||
                   link.OutputId == node.GetNodeOutputPinId();
        }

        const domain::Node &node;
    };

    void deleteNodeAndConnectedLinks(ed::NodeId nodeId) {
        // First find the node
        auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), NodeIdMatcher(nodeId));

        if (nodeIt != m_Nodes.end()) {
            // Create a temporary vector for valid links
            ImVector<LinkInfo> newLinks;
            newLinks.reserve(m_Links.size());

            // Copy only the links that are not connected to the node
            for (const auto &link: m_Links) {
                if (!LinkNodeMatcher(*nodeIt)(link)) {
                    newLinks.push_back(link);
                }
            }

            // Replace the old links with the filtered ones
            m_Links = newLinks;

            // Remove the node
            m_Nodes.erase(nodeIt);
        }
    }

    void HandleNodeCopy() {
        ed::NodeId selectedNodeId;
        if (ed::GetSelectedNodes(&selectedNodeId, 1) &&
            ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C)) {
            // Find the selected node
            auto nodeIt = std::find_if(m_Nodes.begin(), m_Nodes.end(), NodeIdMatcher(selectedNodeId));
            if (nodeIt != m_Nodes.end()) {
                // Create a copy of the node
                domain::Node newNode = *nodeIt; // Copy constructor

                // Generate new IDs for the copy
                int currentId = m_NextNodeId;
                newNode.InitializeNodeIds(currentId);
                m_NextNodeId = currentId;

                // Offset the position slightly for visual clarity
                float offsetX = 50.0f;
                float offsetY = 50.0f;
                m_NextNodePosition.x += offsetX;
                m_NextNodePosition.y += offsetY;

                // Add the copy to nodes vector
                m_Nodes.push_back(newNode);
            }
        }
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


            // Handle copying
            HandleNodeCopy();

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
                // Handle link deletion
                LinkDeleteHandler();

                // Handle node deletion
                ed::NodeId selectedNodeId;
                if (ed::GetSelectedNodes(&selectedNodeId, 1)) {
                    deleteNodeAndConnectedLinks(selectedNodeId);
                }
                ed::EndDelete();
            }

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
            changed |= ImGui::InputFloat("X", &x, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Y", &y, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Z", &z, -200.0f, 200.0f);

            // Update values if changed
            if (changed) {
                node.SetRelativeMove(x, y, z);
            }

            ImGui::PopID();
            ImGui::PopItemWidth();
        } else if (node.GetActivation() == RobotActions::NodeActivation::LoopStart) {
            ImGui::PushItemWidth(100);
            std::string idTitle = "loop " + node.GetNodeId().Get();
            ImGui::PushID(idTitle.c_str());

            ImGui::Text("# loops");
            int loopCount = node.GetLoopCount();
            ImGui::InputInt("", &loopCount);
            ImGui::PopID();
            ImGui::PopItemWidth();
            node.SetLoopCount(loopCount);
        } else if (node.GetActivation() == RobotActions::NodeActivation::Wait) {
            std::string idTitle = "wait " + node.GetNodeId().Get();
            ImGui::PushID(idTitle.c_str());
            ImGui::PushItemWidth(100);
            ImGui::Text("s");
            int waitTimer = node.GetWaitTimer();
            ImGui::InputInt("", &waitTimer);

            ImGui::PopID();
            ImGui::PopItemWidth();
            node.SetWaitTimer(waitTimer);
        }
        // Add absolute movement controls for Absolute nodes
        else if (node.GetActivation() == RobotActions::NodeActivation::Absolute) {
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
            changed |= ImGui::InputFloat("X", &x, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Y", &y, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Z", &z, -200.0f, 200.0f);

            // Update values if changed
            if (changed) {
                auto *position = new domain::Position(vec3(x, y, z), vec3(0, 0, 0));
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
        glfwSetMouseButtonCallback(window, [](GLFWwindow *w, int button, int action, int mods) {
            auto *manager = static_cast<WindowManager *>(glfwGetWindowUserPointer(w));
            if (button == GLFW_MOUSE_BUTTON_LEFT && !ImGui::GetIO().WantCaptureMouse) {
                manager->localSimulationManager->setDragActive(action == GLFW_PRESS);
            }
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow *w, double xpos, double ypos) {
            auto *manager = static_cast<WindowManager *>(glfwGetWindowUserPointer(w));
            if (!ImGui::GetIO().WantCaptureMouse) {
                manager->localSimulationManager->handleMouseDrag(xpos, ypos);
            }
        });

        glfwSetScrollCallback(window, [](GLFWwindow *w, double xoffset, double yoffset) {
            auto *manager = static_cast<WindowManager *>(glfwGetWindowUserPointer(w));
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
                ImGui::MenuItem("Node Editor", nullptr, &m_ShowNodeEditor);
                ImGui::MenuItem("Help", nullptr, &m_ShowHelpWindow);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Render windows based on their visibility flags
        if (m_ShowNodeSelector) {
            RenderNodeSelectorWindow();
        }
        if (m_ShowNodeEditor) {
            RenderImGuiNodesEditorWindow(g_Context);
        }
        if (m_ShowHelpWindow) {
            RenderHelpWindow();
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
