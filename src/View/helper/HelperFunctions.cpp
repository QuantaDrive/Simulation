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
            std::cout << "Starting node: " << node.getTitle() << std::endl;
            return &node;
        }
    }
    return nullptr;
}

const domain::Node *NodeHelpers::FindNextNode(const domain::Node *currentNode, const std::vector<domain::Node> &nodes,
                                              const ImVector<LinkInfo> &links) {
    for (const auto &link: links) {
        if (link.InputId == currentNode->getNodeOutputPinId()) {
            for (const auto &node: nodes) {
                if (link.OutputId == node.getNodeInputPinId()) {
                    std::cout << "Next node: " << node.getTitle() << std::endl;
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
        if (link.OutputId == node.getNodeInputPinId()) {
            return true;
        }
    }
    return false;
}

bool NodeHelpers::HasOutputConnection(const domain::Node &node, const ImVector<LinkInfo> &links) {
    for (const auto &link: links) {
        if (link.InputId == node.getNodeOutputPinId()) {
            return true;
        }
    }
    return false;
}

void NodeHelpers::RenderNodeTooltip(RobotActions::NodeActivation action) {
    switch (action) {
        case RobotActions::NodeActivation::Relative:
            ImGui::Text("All following input nodes (Rapid or Linear) will move relative to the arm's current position");
            break;

        case RobotActions::NodeActivation::Absolute:
            ImGui::Text("All following input nodes (Rapid or Linear) will move to position specified by those nodes");
            break;

        case RobotActions::NodeActivation::AngleHead:
            ImGui::Text("Rotate the robot's head to a specific angle");
            ImGui::Text("Specify rotation angles in degrees");
            break;

        case RobotActions::NodeActivation::RapidMove:
            ImGui::Text("Move the robot at maximum speed");
            ImGui::Text("Use for quick positioning where precision isn't critical");
            break;

        case RobotActions::NodeActivation::LinearMove:
            ImGui::Text("Move the robot in a straight line");
            ImGui::Text("Maintains constant speed and precise path");
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
            ImGui::Text("Safe position with defined coordinates");
            break;

        default:
            ImGui::Text("Unknown action type");
            break;
    }
}

void NodeHelpers::CalcRandomPosNextNode(ImVec2* m_NextNodePosition) {
    float randomXNumber = (rand() % 100) - 30;
    float randomYNumber = (rand() % 100) - 30;
    m_NextNodePosition->x += randomXNumber;
    m_NextNodePosition->y += randomYNumber;
}

void NodeHelpers::RenderHelpText() {
    ImGui::Text("Basic Controls:");
    ImGui::BulletText("Left-click and drag: Move nodes");
    ImGui::BulletText("Left-click on pin and drag: Create connections");
    ImGui::BulletText("Left-click on node or link and press Delete: Delete");
    ImGui::BulletText("Ctrl + C while node selected: Copy node");
    ImGui::Spacing();

    ImGui::Text("Creating Nodes:");
    ImGui::BulletText("Click buttons in Node Selector to create nodes");
    ImGui::BulletText("New nodes are placed at random positions");
    ImGui::BulletText("Hover over node buttons for detailed descriptions");
    ImGui::Spacing();

    ImGui::Text("Node Types:");
    ImGui::BulletText("Relative Move: Move robot relative to current position");
    ImGui::BulletText("Absolute Move: Move robot to exact coordinates");
    ImGui::BulletText("Loop Start/End: Create repeating sequences");
    ImGui::BulletText("Wait: Pause execution for specified duration");
    ImGui::BulletText("Home: Return robot to home position");
    ImGui::Spacing();

    ImGui::Text("Connections:");
    ImGui::BulletText("Connect 'Out' to 'In' pins to create sequence");
    ImGui::BulletText("Each node can have one input and one output");
    ImGui::BulletText("Cannot create loops or self-connections");
    ImGui::Spacing();

    ImGui::Text("Execution:");
    ImGui::BulletText("Click 'Send Instructions' to execute node sequence");
    ImGui::BulletText("Sequence starts from node with no input connection");
    ImGui::BulletText("Nodes execute in order of connections");
}
