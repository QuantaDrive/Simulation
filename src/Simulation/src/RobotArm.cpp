//
// Created by stan on 22/02/25.
//

#include "RobotArm.h"

#include <utility>
#include <inicpp.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Init.h"
#include "Utils.h"
#include "Visualization.h"

void simulation::RobotArm::moveAngle(const int joint, float angle, const bool relative, const bool isDegree)
{
    if (joint < 1 || joint >= this->jointDOFs.size())
        // You cant move joint 0 because it is the base
        return;
    if (joint == 6)
        angle = -angle;
    if (isDegree)
        angle = glm::radians(angle);
    if (relative)
        this->jointPositions[joint] += angle;
    else
        this->jointPositions[joint] = angle;
    this->jointPositions[joint] = std::fmod(this->jointPositions[joint], glm::two_pi<float>());
}

const std::vector<float> & simulation::RobotArm::getJointPositions() const {
    return jointPositions;
}

simulation::RobotArm::RobotArm(const std::string& name, const std::string& definitionFile)
{
    name_=name;
    // get part directory from definitionFile
    std::string directory;
    if (const size_t pos = definitionFile.find_last_of('/'); pos != std::string::npos)
        directory = definitionFile.substr(0, pos + 1);

    ini::IniFile definition;
    definition.load(definitionFile);

    const unsigned int numJoints = definition["arm"]["degrees_of_freedom"].as<unsigned int>();
    std::string materialFilename = directory + definition["arm"]["material"].as<std::string>();
    maxVel_ = definition["arm"]["max_velocity"].as<float>();
    maxAcc_ = definition["arm"]["max_accel"].as<float>();

    this->jointPositions.resize(numJoints + 1);

    for (int i = 0; i <= numJoints; i++) {
        // Load mesh
        std::string meshFilename = directory + definition["joint_" + std::to_string(i)]["mesh"].as<std::string>();
        Mesh* mesh = new Mesh(meshFilename);
        this->joints.push_back(mesh);
        // Load joint position and axis for visualization
        this->jointOffsets.push_back(stringToVec3(definition["joint_" + std::to_string(i)]["offset"].as<std::string>()));
        if (i == 0)
            definition["joint_" + std::to_string(i)]["axis"] = 'X';
        switch (definition["joint_" + std::to_string(i)]["axis"].as<char>()) {
            case 'X':
                this->jointDOFs.emplace_back(1.0f, 0.0f, 0.0f);
                break;
            case 'Y':
                this->jointDOFs.emplace_back(0.0f, 1.0f, 0.0f);
                break;
            case 'Z':
                this->jointDOFs.emplace_back(0.0f, 0.0f, 1.0f);
                break;
            default: ;
        }
        if (i != 0) {
            const float omega = definition["joint_" + std::to_string(i)]["dh_omega"].as<float>();
            const float alpha = definition["joint_" + std::to_string(i)]["dh_alpha"].as<float>();
            const float a = definition["joint_" + std::to_string(i)]["dh_a"].as<float>();
            const float d = definition["joint_" + std::to_string(i)]["dh_d"].as<float>();
            this->dh_parameters.emplace_back(omega, alpha, d, a);
        }
    }
}

simulation::RobotArm::~RobotArm()
{
    for (const Mesh* mesh : this->joints) {
        delete mesh;
    }
}

std::vector<glm::vec4> simulation::RobotArm::getDhParameters() const
{
    return dh_parameters;
}

void simulation::RobotArm::render()
{
    glm::mat4 transformationMatrix = glm::mat4(1.0f);
    for (int i = 0; i < this->joints.size(); i++) {
        transformationMatrix = glm::translate(transformationMatrix, this->jointOffsets[i]);
        transformationMatrix = glm::rotate(transformationMatrix, this->jointPositions[i],
            this->jointDOFs[i]);
        Mesh* mesh = this->joints[i];
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &transformationMatrix[0][0]);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * transformationMatrix;
        glUniformMatrix4fv(MvpMatrixID, 1, GL_FALSE, &MVP[0][0]);
        mesh->renderBare();
    }
}

std::string simulation::RobotArm::getName() const
{
    return name_;
}

float simulation::RobotArm::getMaxVel() const
{
    return maxVel_;
}

float simulation::RobotArm::getMaxAcc() const
{
    return maxAcc_;
}
