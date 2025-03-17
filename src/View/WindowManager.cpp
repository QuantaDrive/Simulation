#include "IWindowManager.h"
#include <bits/algorithmfwd.h>

#include "imgui_internal.h"
#include "../Domain/Node.h"
#include "../Domain/NodeActivation.h"
#include "../Domain/Instruction.h"
#include "helper/HelperFunctions.h"
namespace ed = ax::NodeEditor;


using namespace domain;


class WindowManager : public IWindowManager {
    struct NodeIdMatcher {
        explicit NodeIdMatcher(ed::NodeId id) : nodeId(id) {
        }

        bool operator()(const domain::Node &node) const {
            return node.getNodeId() == nodeId;
        }

        ed::NodeId nodeId;
    };

    struct LinkNodeMatcher {
        explicit LinkNodeMatcher(const domain::Node &n) : node(n) {
        }

        bool operator()(const LinkInfo &link) const {
            return link.InputId == node.getNodeInputPinId() ||
                   link.OutputId == node.getNodeOutputPinId();
        }

        const domain::Node &node;
    };


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
    // Select node
    ed::NodeId m_SelectedNodeId = 0;

    // Error handling
    bool m_ShowInfoWindow = true; // Always visible by default
    std::string m_InfoMessage = "Welcome to the Robot Arm Simulator";
    //Simulationmanager
    SimulationManager *localSimulationManager;


    void RenderInfoWindow() {
        // Set window position aligned with Node Editor window
        ImVec2 nodeEditorPos = ImVec2(0, 0);
        float windowHeight = 50.0f;
        float padding = 5.0f;

        // Find the Node Editor window position
        ImGuiWindow* nodeEditorWindow = ImGui::FindWindowByName("Node Editor");
        if (nodeEditorWindow) {
            nodeEditorPos = nodeEditorWindow->Pos;
        }

        ImVec2 windowPos(
            nodeEditorPos.x,
            nodeEditorPos.y - windowHeight
        );
        ImVec2 windowSize(300.0f, windowHeight);

        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

        ImGui::Begin("Status", &m_ShowInfoWindow,
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_AlwaysAutoResize
        );

        ImGui::TextWrapped("%s", m_InfoMessage.c_str());
        ImGui::End();
    }

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
                    newNode.initializeNodeIds(currentId);
                    m_NextNodeId = currentId;

                    switch (action) {
                        case RobotActions::NodeActivation::LoopStart: {
                            int loopCount = 1;
                            ImGui::InputInt("Loop Count", &loopCount);
                            newNode.setLoopCount(loopCount);
                            break;
                        }
                        case RobotActions::NodeActivation::Wait: {
                            int waitTimer = 1;
                            ImGui::InputInt("Wait Timer", &waitTimer);
                            newNode.setWaitTimer(waitTimer);
                            break;
                        }
                        default:
                            break;
                    }

                    m_Nodes.push_back(newNode);

                    NodeHelpers::CalcRandomPosNextNode(&m_NextNodePosition);
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

    static bool compareNodeById(const domain::Node &node, ed::NodeId nodeId) {
        return node.getNodeId() == nodeId;
    }

    bool shouldRemoveLink(const LinkInfo &link, const domain::Node &node) const {
        return link.InputId == node.getNodeInputPinId() ||
               link.OutputId == node.getNodeOutputPinId();
    }


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
                newNode.initializeNodeIds(currentId);
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
                renderNodesInEditor(node);
            }

            // Submit Links
            for (auto &linkInfo: m_Links) {
                ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);
            }

            // Handle creation action
            if (ed::BeginCreate()) {
                LinkHandler();
            }
            ed::EndCreate();

            // Handle deletion action
            if (ed::BeginDelete()) {
                LinkDeleteHandler();

                ed::NodeId selectedNodeId;
                if (ed::GetSelectedNodes(&selectedNodeId, 1)) {
                    deleteNodeAndConnectedLinks(selectedNodeId);
                }
                ed::EndDelete();
            }

            // Handle node selection/deselection
            ed::NodeId selectedNodeId;
            if (!ed::GetSelectedNodes(&selectedNodeId, 1)) {
                localSimulationManager->endPreview();
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
                    if (node.getNodeOutputPinId() == outputPinId ||
                        node.getNodeInputPinId() == inputPinId) {
                        sourceNode = &node;
                    }
                    if (node.getNodeInputPinId() == inputPinId ||
                        node.getNodeOutputPinId() == outputPinId) {
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
                    if (outputPinId == sourceNode->getNodeInputPinId() ||
                        inputPinId == targetNode->getNodeOutputPinId()) {
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


    void renderNodesInEditor(domain::Node &node) {
        ed::BeginNode(node.getNodeId());
        ImGui::Text(node.getTitle().c_str());

        // TODO: Relative and Absolute -> Booleans
        // Add relative movement controls for Relative nodes

        if (node.getActivation() == RobotActions::NodeActivation::LoopStart) {
            ImGui::PushItemWidth(100);
            std::string idTitle = "loop " + node.getNodeId().Get();
            ImGui::PushID(idTitle.c_str());

            ImGui::Text("# loops");
            int loopCount = node.getLoopCount();
            ImGui::InputInt("", &loopCount);
            ImGui::PopID();
            ImGui::PopItemWidth();
            node.setLoopCount(loopCount);
        } else if (node.getActivation() == RobotActions::NodeActivation::Wait) {
            std::string idTitle = "wait " + node.getNodeId().Get();
            ImGui::PushID(idTitle.c_str());
            ImGui::PushItemWidth(100);
            ImGui::Text("s");
            int waitTimer = node.getWaitTimer();
            ImGui::InputInt("", &waitTimer);

            ImGui::PopID();
            ImGui::PopItemWidth();
            node.setWaitTimer(waitTimer);
        }
        // Add absolute movement controls for Absolute nodes
        else if (node.getActivation() == RobotActions::NodeActivation::LinearMove ||
                 node.getActivation() == RobotActions::NodeActivation::RapidMove) {
            std::string idTitle = "linear_or_rapid_move " + node.getNodeId().Get();
            ImGui::PushID(idTitle.c_str());
            ImGui::PushItemWidth(100);

            // Get current coordinates move values
            glm::vec3 posCoords = node.getPosition()->getCoords();
            float xCoord = posCoords.x;
            float yCoord = posCoords.y;
            float zCoord = posCoords.z;
            int linearVelocity = node.getVelocity();

            // Create sliders for X, Y, Z
            bool changed = false;
            changed |= ImGui::InputFloat("X", &xCoord, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Y", &yCoord, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Z", &zCoord, -200.0f, 200.0f);

            if (node.getActivation() == RobotActions::NodeActivation::LinearMove) {
                changed |= ImGui::SliderInt("Velocity", &linearVelocity, 0, 10);
            }

            // Update values and trigger preview if changed
            if (changed) {
                auto *position = new domain::Position(vec3(xCoord, yCoord, zCoord), vec3(0, 0, 0));
                node.setPosition(position);
                if (node.getActivation() == RobotActions::NodeActivation::LinearMove) {
                    node.setVelocity(linearVelocity);
                }
                // Check if this node is currently selected
                ed::NodeId selectedNodeId;
                if (ed::GetSelectedNodes(&selectedNodeId, 1) && node.getNodeId() == selectedNodeId) {
                    if (!localSimulationManager->startPreview(position)) {
                        ShowInfo("current position is unreachable");
                    }
                }
            }

            ImGui::PopID();
            ImGui::PopItemWidth();
        } else if (node.getActivation() == RobotActions::NodeActivation::AngleHead) {
            std::string idTitle = "linear_or_rapid_move " + node.getNodeId().Get();
            ImGui::PushID(idTitle.c_str());
            ImGui::PushItemWidth(100);

            // create input for yaw, pitch,roll
            float yaw = node.getRotationHead().x;
            float pitch = node.getRotationHead().y;
            float roll = node.getRotationHead().z;
            bool changed = false;

            changed |= ImGui::InputFloat("Yaw", &yaw, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Pitch", &pitch, -200.0f, 200.0f);
            changed |= ImGui::InputFloat("Roll", &roll, -200.0f, 200.0f);
            // Update values if changed
            if (changed) {
                node.setRotationHead(vec3(yaw, pitch, roll));
            }
            ImGui::PopID();
            ImGui::PopItemWidth();
        }
        ed::BeginPin(node.getNodeInputPinId(), ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(node.getNodeOutputPinId(), ed::PinKind::Output);
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

        switch (node.getActivation()) {
            case RobotActions::NodeActivation::Absolute: {
                localSimulationManager->setAbsolute(true);
                break;
            }
            case RobotActions::NodeActivation::Relative: {
                localSimulationManager->setAbsolute(false);
                break;
            }
            case RobotActions::NodeActivation::LinearMove: {
                auto *instruction = new domain::Instruction();
                instruction->setPosition(node.getPosition());
                instruction->setVelocity(node.getVelocity());
                instruction->setLinear(true);
                localSimulationManager->executeInstruction(instruction);
                // Cleanup
                delete instruction;
                break;
            }
            case RobotActions::NodeActivation::RapidMove: {
                // Create an instruction with the relative movement values
                auto *instruction = new domain::Instruction();
                instruction->setPosition(node.getPosition());
                instruction->setRapid(true);
                // Execute the instruction using SimulationManager
                localSimulationManager->executeInstruction(instruction);

                // Cleanup
                delete instruction;
                break;
            }
            case(RobotActions::NodeActivation::Wait): {
                auto *instruction = new Instruction();
                instruction->setRelative(false);
                instruction->setWait(node.getWaitTimer());
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
            case(RobotActions::NodeActivation::AngleHead): {
                localSimulationManager->setRotationOfHead(node.getRotationHead());
            }
            default:
                break;
        }
    }

public:
    void ShowInfo(const std::string &message) {
        m_InfoMessage = message;
    }

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
        if (m_ShowInfoWindow) {
            RenderInfoWindow();
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
