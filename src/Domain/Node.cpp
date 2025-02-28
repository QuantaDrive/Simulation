//
// Created by renat on 26-2-2025.
//

#include "Node.h"

Node::Node(RobotActions::NodeActivation activation, int& nodeId)
    : id_(nodeId),
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


