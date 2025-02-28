//
// Created by stan on 22/02/25.
//

#ifndef SIMULATION_ROBOTARM_H
#define SIMULATION_ROBOTARM_H
#include <string>

#include "Mesh.h"


namespace simulation{
    class RobotArm {
        std::vector<Mesh*> joints;
        std::vector<float> jointPositions;
        std::vector<glm::vec3> jointOffsets;
        std::vector<glm::vec3> jointDOFs;
        std::vector<glm::vec4> dh_parameters;

    public:
        void moveAngle(int joint, float angle,
            bool relative = false, bool isDegree = false);
        RobotArm(const std::string& definitionFile);
        ~RobotArm();
        std::vector<glm::vec4> getDhParameters() const;
        void render();
    };
} // simulation


#endif //SIMULATION_ROBOTARM_H
