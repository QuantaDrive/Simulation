#include "NodeActivation.h"
#include <array>
#include <string_view>

namespace RobotActions {
    // Move array inside namespace
    constexpr std::array<std::string_view, static_cast<size_t>(NodeActivation::COUNT)> NodeActivationNames = {
        "Wait",
        "Angle Head",
        "Rapid Move",
        "Linear Move",
        "Absolute",
        "Relative",
        "Home",
        "Loop Start",
        "Loop End"
    };
    // Add namespace to function implementation
    std::string_view toString(NodeActivation action) {
        auto index = static_cast<size_t>(action);
        if (index < NodeActivationNames.size()) {
            return NodeActivationNames[index];
        }
        return "Unknown";
    }
}