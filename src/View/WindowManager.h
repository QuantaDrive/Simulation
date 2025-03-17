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

#include "helper/HelperFunctions.h"

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
    bool m_ShowHelpWindow = false;
    ed::NodeId m_SelectedNodeId = 0;

    // Info window members
    bool m_ShowInfoWindow = true;
    std::string m_InfoMessage = "Welcome to the Robot Arm Simulator";
    SimulationManager* localSimulationManager;

    // Private methods
    void RenderInfoWindow();
    void RenderHelpWindow();
    void RenderNodeSelectorWindow();
    void RenderImGuiNodesEditorWindow(ed::EditorContext* g_Context);
    void HandleNodeCopy();
    void LinkHandler();
    void LinkDeleteHandler();
    void renderNodesInEditor(domain::Node& node);
    void ExecuteNodeChain();
    void ExecuteNode(const domain::Node& node);

    static bool compareNodeById(const domain::Node& node, ed::NodeId nodeId);

    bool shouldRemoveLink(const NodeHelpers::LinkInfo &link, const domain::Node &node);
    void deleteNodeAndConnectedLinks(ed::NodeId nodeId);

public:
    // Constructor
    explicit WindowManager(SimulationManager* simulationManager);

    // Interface implementations
    void SetupImGui(GLFWwindow* existingWindow) override;
    void RenderUI(ed::EditorContext* g_Context) override;
    void CleanupImGui(ed::EditorContext* g_Context) override;

    // Public methods
    void ShowInfo(const std::string& message);
};