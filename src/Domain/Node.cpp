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

void Node::setTitle(const std::string &title) {
    title_ = title;
}

RobotActions::NodeActivation Node::getActivation() const
{
    return activation_;
}

void Node::setActivation(RobotActions::NodeActivation activation) {
    activation_ = activation;
}

ed::NodeId Node::getNodeId() const
{
    return nodeId_;
}

void Node::setNodeId(const ed::NodeId &node_id) {
    nodeId_ = node_id;
}

ed::PinId Node::getNodeInputPinId() const
{
    return nodeInputPinId_;
}

void Node::setNodeInputPinId(const ed::PinId &node_input_pin_id) {
    nodeInputPinId_ = node_input_pin_id;
}

void Node::setNodeOutputPinId(const ed::PinId &node_output_pin_id) {
    nodeOutputPinId_ = node_output_pin_id;
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



void Node::setPosition(Position *absolute_position) {
    position_ = absolute_position;
}


Position *Node::getPosition() const {
    return position_;
}

int Node::getWaitTimer() const {
    return waitTimer_;
}

void Node::setWaitTimer(int waitTimer) {
    waitTimer_ = waitTimer;
}

vec3 Node::getRotationHead() const {
    return rotationHead_;
}

void Node::setRotationHead(const vec3 &degrees) {
    rotationHead_ = degrees;
}

float Node::getVelocity() const {
    return velocity_;
}

void Node::setVelocity(float velocity) {
    velocity_ = velocity;
}




