//
// Created by renat on 26-2-2025.
//

#ifndef NODE_H
#define NODE_H
#include "imgui_node_editor.h"
#include <atomic>

namespace ed = ax::NodeEditor;
namespace RobotActions
{
    enum class NodeActivation;
}


class Node {
    int& id_;
    const char * title_;
    RobotActions::NodeActivation activation_;
    ed::NodeId nodeId_;
    ed::PinId nodeInputPinId_;
    ed::PinId nodeOutputPinId_;
public:
    explicit Node(int& nodeId, const char* title, RobotActions::NodeActivation activation);

    [[nodiscard]] int getId() const;

    [[nodiscard]] const char* getTitle() const;

    [[nodiscard]] RobotActions::NodeActivation getActivation() const;

    [[nodiscard]] ed::NodeId getNodeId() const;

    [[nodiscard]] ed::PinId getNodeInputPinId() const;

    [[nodiscard]] ed::PinId getNodeOutputPinId() const;
};

#endif //NODE_H
