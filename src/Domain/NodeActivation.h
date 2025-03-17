// src/Domain/NodeActivation.h
#ifndef NODEACTIVATION_H
#define NODEACTIVATION_H
#include <string_view>

namespace RobotActions {
    enum class NodeActivation {
        Wait,
        AngleHead,
        RapidMove,
        LinearMove,
        Absolute,
        Relative,
        Home,
        LoopStart,
        LoopEnd,
        COUNT
    };
    std::string_view toString(NodeActivation action);
}

#endif // NODEACTIVATION_H
