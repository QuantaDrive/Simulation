//
// Created by stan on 23/02/25.
//

#ifndef UTILS_H
#define UTILS_H
#include <sstream>
#include <glm/glm.hpp>

inline glm::vec3 stringToVec3(const std::string& str) {
    const size_t pos_delimiter = str.find(',');
    const size_t pos2_delimiter = str.find(',', pos_delimiter + 1);
    if (pos_delimiter == std::string::npos || pos2_delimiter == std::string::npos) {
        return glm::vec3(0);
    }

    const float x = std::stof(str.substr(0, pos_delimiter));
    const float y = std::stof(str.substr(pos_delimiter + 1, pos2_delimiter));
    const float z = std::stof(str.substr(pos2_delimiter + 1));
    return {x, y, z};
}

inline glm::vec4 stringToVec4(const std::string& str) {
    const size_t pos_delimiter = str.find(',');
    const size_t pos2_delimiter = str.find(',', pos_delimiter + 1);
    const size_t pos3_delimiter = str.find(',', pos2_delimiter + 1);
    if (pos_delimiter == std::string::npos ||
        pos2_delimiter == std::string::npos ||
        pos3_delimiter == std::string::npos) {
        return glm::vec4(0);
    }

    const float x = std::stof(str.substr(0, pos_delimiter));
    const float y = std::stof(str.substr(pos_delimiter + 1, pos2_delimiter));
    const float z = std::stof(str.substr(pos2_delimiter + 1, pos3_delimiter));
    const float w = std::stof(str.substr(pos3_delimiter + 1));
    return {x, y, z, w};
}

#endif //UTILS_H
