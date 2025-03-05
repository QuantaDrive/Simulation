// src/Domain/NodeActivation.h
#ifndef NODEACTIVATION_H
#define NODEACTIVATION_H
#include <string_view>

namespace RobotActions {
    enum class NodeActivation {
        Wait,
        RapidMove,
        LinearMove,
        DirectMove,
        Absolute,
        Relative,
        Home,
        EnableMotors,
        DisableMotors,
        Loop,
        Grip,
        Hold,
        Action, COUNT
    };
    std::string_view toString(NodeActivation action);
}

#endif // NODEACTIVATION_H
