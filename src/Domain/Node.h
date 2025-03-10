//
// Created by renat on 26-2-2025.
//

#ifndef NODE_H
#define NODE_H
#include <array>

#include "imgui_node_editor.h"
#include <atomic>
#include <string>
#include <vector>
#include <glm/detail/type_vec.hpp>
#include <glm/detail/type_vec3.hpp>

#include "Position.h"

namespace ed = ax::NodeEditor;
namespace RobotActions
{
    enum class NodeActivation;
}

namespace domain
{
    class Position;

    class Node {
        std::string title_;
        RobotActions::NodeActivation activation_;
        ed::NodeId nodeId_;
        ed::PinId nodeInputPinId_;
        ed::PinId nodeOutputPinId_;
        int loopCount_ = 0;
        int waitTimer_ = 0;
        glm::vec3 relativeMove_;
        Position* absolutePosition_ = new Position({0, 0, 0}, {0, 0, 0});
    public:
        explicit Node(const char* title, RobotActions::NodeActivation activation);

        [[nodiscard]] std::string GetTitle() const;

        [[nodiscard]] RobotActions::NodeActivation GetActivation() const;

        [[nodiscard]] ed::NodeId GetNodeId() const;

        [[nodiscard]] ed::PinId GetNodeInputPinId() const;

        [[nodiscard]] ed::PinId GetNodeOutputPinId() const;

        void InitializeNodeIds(int& nodeId);

        void SetLoopCount(int count);

        [[nodiscard]] int GetLoopCount() const;

        void SetRelativeMove(float x, float y, float z);

        [[nodiscard]] glm::vec3 GetRelativeMove() const;

        void SetAbsolutePosition(Position *absolute_position);

        [[nodiscard]] Position *GetAbsolutePosition() const;

        [[nodiscard]] int GetWaitTimer() const;

        void SetWaitTimer(int wait_timer);
    };
}

#endif //NODE_H
