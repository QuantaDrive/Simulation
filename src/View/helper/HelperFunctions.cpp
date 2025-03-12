//
// Created by renat on 10-3-2025.
//

#include "HelperFunctions.h"

#include <iostream>

const domain::Node *NodeHelpers::FindStartNode(const std::vector<domain::Node> &nodes,
                                               const ImVector<LinkInfo> &links) {
    // If there's only one node and no links, consider it the start node
    if (nodes.size() == 1 && links.empty()) {
        return &nodes[0];
    }

    // Otherwise, find a node that only has output connections
    for (const auto &node: nodes) {
        if (IsStartNode(node, links)) {
            std::cout << "Starting node: " << node.GetTitle() << std::endl;
            return &node;
        }
    }
    return nullptr;
}

const domain::Node *NodeHelpers::FindNextNode(const domain::Node *currentNode, const std::vector<domain::Node> &nodes,
                                              const ImVector<LinkInfo> &links) {
    for (const auto &link: links) {
        if (link.InputId == currentNode->GetNodeOutputPinId()) {
            for (const auto &node: nodes) {
                if (link.OutputId == node.GetNodeInputPinId()) {
                    std::cout << "Next node: " << node.GetTitle() << std::endl;
                    return &node;
                }
            }
        }
    }
    return nullptr;

}

bool NodeHelpers::IsStartNode(const domain::Node &node, const ImVector<LinkInfo> &links) {
    // If there are no links at all a single node is considered the start node
    if (links.empty()) {
        return true;
    }
    // Otherwise, node should have no input connections but have output connections
    return !HasInputConnection(node,links) && HasOutputConnection(node, links);
}

bool NodeHelpers::HasInputConnection(const domain::Node &node, const ImVector<LinkInfo> &links) {
    for (const auto &link: links) {
        if (link.OutputId == node.GetNodeInputPinId()) {
            return true;
        }
    }
    return false;
}

bool NodeHelpers::HasOutputConnection(const domain::Node &node, const ImVector<LinkInfo> &links) {
    for (const auto &link: links) {
        if (link.InputId == node.GetNodeOutputPinId()) {
            return true;
        }
    }
    return false;
}

void NodeHelpers::RenderNodeTooltip(RobotActions::NodeActivation action) {
    switch (action) {
        case RobotActions::NodeActivation::Relative:
            ImGui::Text("Move the robot relative to its current position");
        ImGui::Text("X, Y, Z coordinates can be adjusted");
        break;
        case RobotActions::NodeActivation::Absolute:
            ImGui::Text("Move the robot to an absolute position");
        ImGui::Text("Specify exact X, Y, Z coordinates");
        break;
        case RobotActions::NodeActivation::LoopStart:
            ImGui::Text("Start a loop sequence");
        ImGui::Text("Specify number of iterations");
        break;
        case RobotActions::NodeActivation::LoopEnd:
            ImGui::Text("End a loop sequence");
        ImGui::Text("Must be connected to a Loop Start node");
        break;
        case RobotActions::NodeActivation::Wait:
            ImGui::Text("Pause execution for specified time");
        ImGui::Text("Duration in seconds");
        break;
        case RobotActions::NodeActivation::Home:
            ImGui::Text("Return robot to home position");
        break;
        default:
            break;
    }
}
