//
// Created by renat on 10-3-2025.
//

#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H



#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include "imgui_node_editor.h"
#include "../../Domain/Node.h"
#include "../../Domain/NodeActivation.h"

namespace domain {
    class Node;
}

namespace ed = ax::NodeEditor;

struct LinkInfo {
    ed::LinkId Id;
    ed::PinId InputId;
    ed::PinId OutputId;
};

class NodeHelpers {
public:
    static const domain::Node* FindStartNode(const std::vector<domain::Node>& nodes, const ImVector<LinkInfo>& links);
    static const domain::Node* FindNextNode(const domain::Node* currentNode, const std::vector<domain::Node>& nodes, const ImVector<LinkInfo>& links);
    static bool IsStartNode(const domain::Node& node, const ImVector<LinkInfo>& links);
    static bool HasInputConnection(const domain::Node& node, const ImVector<LinkInfo>& links);
    static bool HasOutputConnection(const domain::Node& node, const ImVector<LinkInfo>& links);
    static void RenderNodeTooltip(RobotActions::NodeActivation action);
    static void CalcRandomPosNextNode(ImVec2* m_NextNodePosition);
    static void RenderHelpText();
};


#endif //HELPERFUNCTIONS_H
