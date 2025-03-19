#include "WindowManager.h"

#include <stack>
#include <bits/algorithmfwd.h>

#include "imgui_internal.h"
#include "../Domain/Node.h"
#include "../Domain/NodeActivation.h"
#include "../Domain/Instruction.h"
#include "../Domain/RobotArm.h"
#include "../Domain/Task.h"
#include "helper/HelperFunctions.h"
namespace ed = ax::NodeEditor;
using namespace domain;

WindowManager::NodeIdMatcher::NodeIdMatcher(ed::NodeId id) : nodeId(id) {
}

bool WindowManager::NodeIdMatcher::operator()(const domain::Node &node) const {
    return node.getNodeId() == nodeId;
}

WindowManager::LinkNodeMatcher::LinkNodeMatcher(const domain::Node &n) : node(n) {
}

bool WindowManager::LinkNodeMatcher::operator()(const NodeHelpers::LinkInfo &link) const {
    return link.InputId == node.getNodeInputPinId() ||
           link.OutputId == node.getNodeOutputPinId();
}


void WindowManager::renderInfoWindow() {
    // Set window position aligned with Node Editor window
    ImVec2 nodeEditorPos = ImVec2(0, 0);
    float windowHeight = 100.0f;
    float padding = 5.0f;

    // Find the Node Editor window position
    ImGuiWindow *nodeEditorWindow = ImGui::FindWindowByName("Node Editor");
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

    ImGui::Begin("Info Window", &m_ShowInfoWindow,
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoScrollWithMouse |
                 ImGuiWindowFlags_AlwaysAutoResize
    );

    // Check if message starts with Error
    bool isError = m_InfoMessage.find("Error") == 0;

    float oldFontSize = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale = 1.5f;
    ImGui::PushFont(ImGui::GetFont());

    if (isError) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    ImGui::TextWrapped("%s", m_InfoMessage.c_str());

    if (isError) {
        ImGui::PopStyleColor();
    }

    ImGui::PopFont();
    ImGui::GetFont()->Scale = oldFontSize;

    ImGui::End();
}

void WindowManager::renderHelpWindow() {
    if (!m_ShowHelpWindow) return;
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 300), ImVec2(600, 800));
    ImGui::Begin("Node Editor Help", &m_ShowHelpWindow);
    NodeHelpers::RenderHelpText();
    ImGui::End();
}


void WindowManager::renderNodeSelectorWindow() {
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
            executeNodeChain();
        }

        // Add tooltip for Send instructions button
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Execute the current node sequence");
            ImGui::Text("Nodes must be properly connected");
            ImGui::EndTooltip();
        }

        ImGui::PopStyleColor(3);

        if (ImGui::Button("Send to physical arm", ImVec2(windowWidth, 0))) {
            sendNodeChainToPhysicalArm();
        }
    }
    ImGui::End();
}

bool WindowManager::shouldRemoveLink(const NodeHelpers::LinkInfo &link, const domain::Node &node) {
    return link.InputId == node.getNodeInputPinId() ||
           link.OutputId == node.getNodeOutputPinId();
}


void WindowManager::deleteNodeAndConnectedLinks(ed::NodeId nodeId) {
    NodeHelpers::DeleteNodeAndConnectedLinks(nodeId, m_Nodes, m_Links);
}

void WindowManager::handleNodeCopy() {
    NodeHelpers::HandleNodeCopy(m_Nodes, m_NextNodeId, m_NextNodePosition);
}

void WindowManager::renderImGuiNodesEditorWindow(ed::EditorContext *g_Context) {
    ImGui::Begin("Node Editor", &m_ShowNodeEditor, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar()) {
        ImGui::Dummy(ImVec2(5.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 3.0f));
        if (ImGui::Button("Focus")) {
            ed::SetCurrentEditor(g_Context);
            ed::NavigateToContent();
        }
        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(5.0f, 0.0f));
        ImGui::EndMenuBar();
    }

    if (!ImGui::IsWindowCollapsed()) {
        ed::SetCurrentEditor(g_Context);
        ed::Begin("My Editor");

        handleNodeCopy();

        for (auto &node: m_Nodes) {
            renderNodesInEditor(node);
        }

        NodeHelpers::RenderLinks(m_Links);

        if (ed::BeginCreate()) {
            linkHandler();
        }
        ed::EndCreate();

        if (ed::BeginDelete()) {
            linkDeleteHandler();
            ed::NodeId selectedNodeId;
            NodeHelpers::HandleDeleteActions(selectedNodeId, m_Nodes, m_Links);
            ed::EndDelete();
        }

        NodeHelpers::HandleNodeSelection(localSimulationManager,m_Nodes,  m_LastSelectedNode);

        if (m_FirstFrame) {
            ed::NavigateToContent(0.0f);
        }

        ed::End();
        ed::SetCurrentEditor(nullptr);
    }
    m_FirstFrame = false;
    ImGui::End();
}

void WindowManager::linkHandler() {
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

void WindowManager::linkDeleteHandler() {
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


void WindowManager::renderNodesInEditor(domain::Node &node) {
    ed::BeginNode(node.getNodeId());
    ImGui::Text(node.getTitle().c_str());

    NodeHelpers::RenderNodeControls(node, localSimulationManager,
                                    [this](const std::string &msg) { showInfo(msg); });

    ed::BeginPin(node.getNodeInputPinId(), ed::PinKind::Input);
    ImGui::Text("-> In");
    ed::EndPin();
    ImGui::SameLine();
    ed::BeginPin(node.getNodeOutputPinId(), ed::PinKind::Output);
    ImGui::Text("Out ->");
    ed::EndPin();
    ed::EndNode();
}


void WindowManager::executeNodeChain() {
    const domain::Node* startNode = NodeHelpers::FindStartNode(m_Nodes, m_Links);
    if (!startNode) {
        showInfo("Error: No starting node found");
        return;
    }

    stack<std::pair<const domain::Node*, int>> loopStack;
    const domain::Node* currentNode = startNode;

    while (currentNode) {
        if (currentNode->getActivation() == RobotActions::NodeActivation::LoopStart) {
            // Push the loop start node and remaining iterations to stack
            loopStack.push({currentNode, currentNode->getLoopCount()});
        }
        else if (currentNode->getActivation() == RobotActions::NodeActivation::LoopEnd) {
            if (loopStack.empty()) {
                showInfo("Error: Loop End without matching Loop Start");
                return;
            }

            auto& [loopStartNode, remainingIterations] = loopStack.top();
            remainingIterations--;

            if (remainingIterations > 0) {
                // Go back to node after loop start
                currentNode = NodeHelpers::FindNextNode(loopStartNode, m_Nodes, m_Links);
                continue;
            }
            loopStack.pop();
        }

        executeNode(*currentNode);
        currentNode = NodeHelpers::FindNextNode(currentNode, m_Nodes, m_Links);
    }

    if (!loopStack.empty()) {
        showInfo("Error: Loop Start without matching Loop End");
    }
}

void WindowManager::executeNode(const domain::Node &node) {
    if (!localSimulationManager) return;
    NodeHelpers::ExecuteNode(node, localSimulationManager);
}

void WindowManager::sendNodeChainToPhysicalArm()
{
    const domain::Node* startNode = NodeHelpers::FindStartNode(m_Nodes, m_Links);
    if (!startNode) {
        showInfo("Error: No starting node found");
        return;
    }

    stack<std::pair<const domain::Node*, int>> loopStack;
    const domain::Node* currentNode = startNode;

    localSimulationManager->getRobotArm()->getTask()->clearInstructions();

    while (currentNode) {
        if (currentNode->getActivation() == RobotActions::NodeActivation::LoopStart) {
            // Push the loop start node and remaining iterations to stack
            loopStack.push({currentNode, currentNode->getLoopCount()});
        }
        else if (currentNode->getActivation() == RobotActions::NodeActivation::LoopEnd) {
            if (loopStack.empty()) {
                showInfo("Error: Loop End without matching Loop Start");
                return;
            }

            auto& [loopStartNode, remainingIterations] = loopStack.top();
            remainingIterations--;

            if (remainingIterations > 0) {
                // Go back to node after loop start
                currentNode = NodeHelpers::FindNextNode(loopStartNode, m_Nodes, m_Links);
                continue;
            }
            loopStack.pop();
        }

        addNodeToTask(*currentNode);
        currentNode = NodeHelpers::FindNextNode(currentNode, m_Nodes, m_Links);
    }

    if (!loopStack.empty()) {
        showInfo("Error: Loop Start without matching Loop End");
    }
    localPhysicalManager->executeTask(localSimulationManager->getRobotArm()->getTask());
}

void WindowManager::addNodeToTask(const domain::Node& node)
{
    Task* task = localSimulationManager->getRobotArm()->getTask();
    switch (node.getActivation()) {
    case RobotActions::NodeActivation::Absolute:
        localSimulationManager->setAbsolute(true);
        break;

    case RobotActions::NodeActivation::Relative:
        localSimulationManager->setAbsolute(false);
        break;

    case RobotActions::NodeActivation::LinearMove: {
            auto* instruction = new domain::Instruction();
            instruction->setPosition(node.getPosition());
            instruction->setVelocity(node.getVelocity());
            instruction->setLinear(true);
            if (localSimulationManager->isAbsolute()) instruction->setRelative(false);
            else instruction->setRelative(true);
            task->addInstruction(instruction);
            // delete instruction;
            break;
    }

    case RobotActions::NodeActivation::RapidMove: {
            auto* instruction = new domain::Instruction();
            instruction->setPosition(node.getPosition());
            instruction->setRapid(true);
            if (localSimulationManager->isAbsolute()) instruction->setRelative(false);
            else instruction->setRelative(true);
            task->addInstruction(instruction);
            // delete instruction;
            break;
    }

    case RobotActions::NodeActivation::Wait: {
            auto* instruction = new domain::Instruction();
            instruction->setRelative(false);
            instruction->setWait(node.getWaitTimer());
            task->addInstruction(instruction);
            // delete instruction;
            break;
    }

    case RobotActions::NodeActivation::Home: {
            auto* instruction = new domain::Instruction();
            instruction->setRelative(false);
            instruction->setGoHome(true);
            task->addInstruction(instruction);
            // delete instruction;
            break;
    }

    case RobotActions::NodeActivation::AngleHead:
        localSimulationManager->setRotationOfHead(node.getRotationHead());
        break;

    default:
        break;
    }

}

void WindowManager::showInfo(const std::string &message) {
    m_InfoMessage = message;
}

WindowManager::WindowManager(SimulationManager *simulationManager, PhysicalManager *physicalManager) {
    localSimulationManager = simulationManager;
    localPhysicalManager = physicalManager;
}


void WindowManager::setupImGui(GLFWwindow *existingWindow) {
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


void WindowManager::renderUI(ed::EditorContext *g_Context) {
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
        renderNodeSelectorWindow();
    }
    if (m_ShowNodeEditor) {
        renderImGuiNodesEditorWindow(g_Context);
    }
    if (m_ShowHelpWindow) {
        renderHelpWindow();
    }
    if (m_ShowInfoWindow) {
        renderInfoWindow();
    }
    // Render the final ImGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void WindowManager::rleanupImGui(ed::EditorContext *g_Context) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    ed::DestroyEditor(g_Context);
}
