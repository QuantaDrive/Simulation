#pragma once
#include <algorithm>
#include <vector>
#include "imgui.h"
#include "imgui_node_editor.h"
#include "../Domain/Node.h"
#include "IWindowManager.h"
#include "../Domain/Position.h"
#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include "../Simulation/BL/SimulationManager.h"
#include "../Physical/BL/PhysicalManager.h"

#include "helper/HelperFunctions.h"
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


namespace ed = ax::NodeEditor;

class WindowManager : public IWindowManager {
private:

    // Node matcher structures
    struct NodeIdMatcher {
        explicit NodeIdMatcher(ed::NodeId id);
        bool operator()(const domain::Node& node) const;
        ed::NodeId nodeId;
    };

    struct LinkNodeMatcher {
        explicit LinkNodeMatcher(const domain::Node& n);
        bool operator()(const NodeHelpers::LinkInfo& link) const;
        const domain::Node& node;
    };

    // Window and rendering members
    GLFWwindow* window = nullptr;
    int gripforce = 0;
    std::string textInput;

    // Node editor members
    ImVector<NodeHelpers::LinkInfo> m_Links;
    std::vector<domain::Node> m_Nodes;
    bool m_FirstFrame = true;
    int m_NextLinkId = 100;
    ImVec2 m_NextNodePosition = ImVec2(0, 0);
    int m_NextNodeId = 1;

    // Window state flags
    bool m_ShowNodeSelector = true;
    bool m_ShowNodeEditor = true;
    bool m_ShowHelpWindow = true;
    ed::NodeId m_SelectedNodeId = 0;
    ed::NodeId m_LastSelectedNode;

    // Info window members
    bool m_ShowInfoWindow = true;
    std::string m_InfoMessage = "Welcome to the Robot Arm Simulator";
    SimulationManager* localSimulationManager;
    PhysicalManager* localPhysicalManager;
    // Save and load window members
    bool m_ShowSavedNodesWindow = true;
    std::vector<std::string> m_SavedNodeFiles;
    static const std::string SAVES_DIRECTORY;

    // Private methods
    void renderInfoWindow();
    void renderHelpWindow();
    void renderNodeSelectorWindow();
    void renderImGuiNodesEditorWindow(ed::EditorContext* g_Context);
    void handleNodeCopy();
    void linkHandler();
    void linkDeleteHandler();
    void renderNodesInEditor(domain::Node& node);
    void executeNodeChain();
    void executeNode(const domain::Node& node);
    void sendNodeChainToPhysicalArm();
    void addNodeToTask(const domain::Node& node);

    bool shouldRemoveLink(const NodeHelpers::LinkInfo &link, const domain::Node &node);
    void deleteNodeAndConnectedLinks(ed::NodeId nodeId);
    void saveNodeEditor(const std::string& filename);
    void loadNodeEditor(const std::string& filename);
    void renderSavedNodesWindow();
    void refreshSavedNodesList();
public:
    // Constructor
    explicit WindowManager(SimulationManager* simulationManager,PhysicalManager* physicalManager);

    // Interface implementations
    void setupImGui(GLFWwindow* existingWindow) override;
    void renderUI(ed::EditorContext* g_Context) override;
    void cleanupImGui(ed::EditorContext* g_Context) override;

    // Public methods
    void showInfo(const std::string& message);
};