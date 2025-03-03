#include "NodeActivation.h"
#include <array>
#include <string_view>

namespace RobotActions {
    // Move array inside namespace
    constexpr std::array<std::string_view, static_cast<size_t>(NodeActivation::COUNT)> NodeActivationNames = {
        "Wait",
        "RapidMove",
        "LinearMove",
        "DirectMove",
        "Absolute",
        "Relative",
        "Home",
        "EnableMotors",
        "DisableMotors",
        "Loop",
        "Grip",
        "Hold",
        "Action"
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