//
// Created by renat on 26-2-2025.
//

#include "Node.h"

using namespace domain;

Node::Node(const char* title, RobotActions::NodeActivation activation)
        : title_(title), activation_(activation)

{
}

const char* Node::getTitle() const
{
    return title_;
}

RobotActions::NodeActivation Node::getActivation() const
{
    return activation_;
}

ed::NodeId Node::getNodeId() const
{
    return nodeId_;
}

ed::PinId Node::getNodeInputPinId() const
{
    return nodeInputPinId_;
}

ed::PinId Node::getNodeOutputPinId() const
{
    return nodeOutputPinId_;
}

void Node::initializeNodeIds(int& nodeId)
{
    nodeId_ = nodeId++;
    nodeInputPinId_ = nodeId++;
    nodeOutputPinId_ = nodeId++;

}
