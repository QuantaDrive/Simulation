//
// Created by renat on 26-2-2025.
//

#include "Node.h"
#include "NodeActivation.h"

using namespace domain;

Node::Node(const char* title, RobotActions::NodeActivation activation)
    : title_(title), activation_(activation) {
}

 std::string Node::GetTitle() const
{
    return title_;
}

RobotActions::NodeActivation Node::GetActivation() const
{
    return activation_;
}

ed::NodeId Node::GetNodeId() const
{
    return nodeId_;
}

ed::PinId Node::GetNodeInputPinId() const
{
    return nodeInputPinId_;
}

ed::PinId Node::GetNodeOutputPinId() const
{
    return nodeOutputPinId_;
}

void Node::InitializeNodeIds(int& nodeId)
{
    nodeId_ = nodeId++;
    nodeInputPinId_ = nodeId++;
    nodeOutputPinId_ = nodeId++;

}

void Node::SetLoopCount(int count) {
    if (activation_ == RobotActions::NodeActivation::LoopStart) {
        loopCount_ = count;
    }
}

int Node::GetLoopCount() const {
    if (activation_ == RobotActions::NodeActivation::LoopStart) {
        return loopCount_;
    }
    return 0;
}

void Node::SetRelativeMove(Position* relMove) {
    relativeMove_ = relMove;
}

Position* Node::GetRelativeMove() const {
    return relativeMove_;
}

void Node::SetPosition(Position *absolute_position) {
    position_ = absolute_position;
}


Position *Node::GetPosition() const {
    return position_;
}

int Node::GetWaitTimer() const {
    return waitTimer_;
}

void Node::SetWaitTimer(int wait_timer) {
    waitTimer_ = wait_timer;
}

vec3 Node::GetRotationHead() const {
    return rotationHead_;
}

void Node::SetRotationHead(const vec3 &degrees) {
    rotationHead_ = degrees;
}

