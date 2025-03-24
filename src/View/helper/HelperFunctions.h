//
// Created by renat on 10-3-2025.
//

#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H



#pragma once
#include <functional>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui_node_editor.h"
#include "../../Domain/Node.h"
#include "../../Domain/NodeActivation.h"
#include "../../Domain/Instruction.h"
#include "../../Simulation/BL/SimulationManager.h"

class SimulationManager;

namespace domain {
    class Node;
}

namespace ed = ax::NodeEditor;

namespace NodeHelpers {
    struct LinkInfo {
        ed::LinkId Id;
        ed::PinId InputId;
        ed::PinId OutputId;
    };

    // Function declarations
    const domain::Node* findStartNode(const std::vector<domain::Node>& nodes, const ImVector<LinkInfo>& links);
    const domain::Node* findNextNode(const domain::Node* currentNode, const std::vector<domain::Node>& nodes, const ImVector<LinkInfo>& links);
    bool isStartNode(const domain::Node& node, const ImVector<LinkInfo>& links);
    bool hasInputConnection(const domain::Node& node, const ImVector<LinkInfo>& links);
    bool hasOutputConnection(const domain::Node& node, const ImVector<LinkInfo>& links);
    void renderNodeTooltip(RobotActions::NodeActivation action);
    void calcRandomPosNextNode(ImVec2* m_NextNodePosition);
    void renderHelpText();
    void renderNodeControls(domain::Node& node, SimulationManager* simulationManager, std::function<void(const std::string&)> showInfoCallback);
    void executeNode(const domain::Node& node, SimulationManager* simulationManager);
    bool shouldRemoveLink(const LinkInfo& link, const domain::Node& node);
    void handleNodeCopy(std::vector<domain::Node>& nodes, int& nextNodeId, ImVec2& nextNodePosition);
    bool validateNewLink(const domain::Node* sourceNode, const domain::Node* targetNode);
    void deleteNodeAndConnectedLinks(ed::NodeId nodeId, std::vector<domain::Node>& nodes, ImVector<LinkInfo>& links);
    void renderLinks(const ImVector<LinkInfo>& links);
    void handleNodeSelection(SimulationManager* simulationManager, std::vector<domain::Node>& m_Nodes, ed::NodeId& lastSelectedNode);
    void handleDeleteActions(ed::NodeId selectedNodeId,
                                        std::vector<domain::Node>& nodes,
                                        ImVector<LinkInfo>& links);
    void saveNodeEditor(
      const std::string& filename,
      const std::vector<domain::Node>& nodes,
      const ImVector<LinkInfo>& links
  );

    void loadNodeEditor(
        const std::string& filename,
        std::vector<domain::Node>& nodes,
        ImVector<LinkInfo>& links,
        int& nextNodeId,
        int& nextLinkId
    );
    void renderTooltip(const char* text);
}


#endif //HELPERFUNCTIONS_H
