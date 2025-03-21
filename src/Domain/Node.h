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
        Position* position_ = new Position({0, 0, 0}, {0, 0, 0});
        vec3 rotationHead_ = {0, 0, 0};
        float velocity_ = 0;
    public:
        explicit Node(const char* title, RobotActions::NodeActivation activation);

        Node() = default;
        [[nodiscard]] std::string getTitle() const;
        void setTitle(const std::string &title);

        [[nodiscard]] RobotActions::NodeActivation getActivation() const;
        void setActivation(RobotActions::NodeActivation activation);

        [[nodiscard]] ed::NodeId getNodeId() const;
        void setNodeId(const ed::NodeId &node_id);

        [[nodiscard]] ed::PinId getNodeInputPinId() const;
        void setNodeInputPinId(const ed::PinId &node_input_pin_id);

        [[nodiscard]] ed::PinId getNodeOutputPinId() const;
        void setNodeOutputPinId(const ed::PinId &node_output_pin_id);

        void initializeNodeIds(int& nodeId);

        [[nodiscard]] int getLoopCount() const;
        void setLoopCount(int count);

        [[nodiscard]] Position *getPosition() const;
        void setPosition(Position *absolute_position);


        [[nodiscard]] int getWaitTimer() const;
        void setWaitTimer(int waitTimer);

        [[nodiscard]] vec3 getRotationHead() const;
        void setRotationHead(const vec3 &degrees);

        [[nodiscard]] float getVelocity() const;
        void setVelocity(float velocity);
    };
}

#endif //NODE_H
