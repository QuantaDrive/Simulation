// src/Domain/NodeActivation.h
#ifndef NODEACTIVATION_H
#define NODEACTIVATION_H

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
        Action
    };
}

#endif // NODEACTIVATION_H