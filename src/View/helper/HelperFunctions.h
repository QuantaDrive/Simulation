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
    const domain::Node* FindStartNode(const std::vector<domain::Node>& nodes, const ImVector<LinkInfo>& links);
    const domain::Node* FindNextNode(const domain::Node* currentNode, const std::vector<domain::Node>& nodes, const ImVector<LinkInfo>& links);
    bool IsStartNode(const domain::Node& node, const ImVector<LinkInfo>& links);
    bool HasInputConnection(const domain::Node& node, const ImVector<LinkInfo>& links);
    bool HasOutputConnection(const domain::Node& node, const ImVector<LinkInfo>& links);
    void RenderNodeTooltip(RobotActions::NodeActivation action);
    void CalcRandomPosNextNode(ImVec2* m_NextNodePosition);
    void RenderHelpText();
    void RenderNodeControls(domain::Node& node, SimulationManager* simulationManager, std::function<void(const std::string&)> showInfoCallback);
    void ExecuteNode(const domain::Node& node, SimulationManager* simulationManager);
    bool ShouldRemoveLink(const LinkInfo& link, const domain::Node& node);
    void HandleNodeCopy(std::vector<domain::Node>& nodes, int& nextNodeId, ImVec2& nextNodePosition);
    bool ValidateNewLink(const domain::Node* sourceNode, const domain::Node* targetNode);
    void DeleteNodeAndConnectedLinks(ed::NodeId nodeId, std::vector<domain::Node>& nodes, ImVector<LinkInfo>& links);
    void RenderLinks(const ImVector<LinkInfo>& links);
    void HandleNodeSelection(SimulationManager* simulationManager);
    void HandleDeleteActions(ed::NodeId selectedNodeId,
                                        std::vector<domain::Node>& nodes,
                                        ImVector<LinkInfo>& links);
}


#endif //HELPERFUNCTIONS_H
