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
    if (joint < 1 || joint >= this->joints.size())
        // You cant move joint 0 because it is the base
        return;
    if (isDegree)
        angle = glm::radians(angle);
    if (relative)
        this->jointPositions[joint] += angle;
    else
        this->jointPositions[joint] = angle;
    const glm::vec3 rotation = this->jointDOFs[joint] * angle;
    this->joints[joint]->rotate(rotation, relative);
}

simulation::RobotArm::RobotArm(const std::string& definitionFile)
{
    // get part directory from definitionFile
    std::string directory;
    if (const size_t pos = definitionFile.find_last_of('/'); pos != std::string::npos)
        directory = definitionFile.substr(0, pos + 1);

    ini::IniFile definition;
    definition.load(definitionFile);

    const unsigned int numJoints = definition["arm"]["degrees_of_freedom"].as<unsigned int>();
    std::string materialFilename = directory + definition["arm"]["material"].as<std::string>();

    this->joints.reserve(numJoints);
    this->jointPositions.reserve(numJoints);
    this->jointOffsets.reserve(numJoints);
    this->jointDOFs.reserve(numJoints);

    for (int i = 0; i <= numJoints; i++) {
        // Load mesh
        std::string meshFilename = directory + definition["joint_" + std::to_string(i)]["mesh"].as<std::string>();
        Mesh* mesh = new Mesh(meshFilename.c_str(), materialFilename.c_str());
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
    }
}

simulation::RobotArm::~RobotArm()
{
    for (const Mesh* mesh : this->joints) {
        delete mesh;
    }
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
