//
// Created by renat on 26-2-2025.
//

#include "Node.h"

Node::Node(int& nodeId, const char* title, RobotActions::NodeActivation activation)
    : id_(nodeId),
      title_(title),
      activation_(activation),
      nodeId_(nodeId++),
      nodeInputPinId_(nodeId++),
      nodeOutputPinId_(nodeId++)
{
}

int Node::getId() const
{
    return id_;
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
