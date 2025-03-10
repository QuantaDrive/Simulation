//
// Created by renat on 26-2-2025.
//

#include "Node.h"
#include "NodeActivation.h"

using namespace domain;

Node::Node(const char* title, RobotActions::NodeActivation activation)
    : title_(title), activation_(activation) {
}

 std::string Node::getTitle() const
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

void Node::setLoopCount(int count) {
    if (activation_ == RobotActions::NodeActivation::LoopStart) {
        loopCount_ = count;
    }
}

int Node::getLoopCount() const {
    if (activation_ == RobotActions::NodeActivation::LoopStart) {
        return loopCount_;
    }
    return 0;
}

void Node::setRelativeMove(float x, float y, float z) {
    relativeMove_ = {x, y, z};
}

glm::vec3 Node::getRelativeMove() const {
    return relativeMove_;
}

void Node::setAbsolutePosition(Position *absolute_position) {
    absolutePosition_ = absolute_position;
}


Position *Node::getAbsolutePosition() const {
    return absolutePosition_;
}

