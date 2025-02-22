//
// Created by stan on 22/02/25.
//

#include "RobotArm.h"

#include <utility>
#include <glm/gtc/matrix_transform.hpp>

#include "Init.h"
#include "Visualization.h"

void simulation::RobotArm::moveAngle(const int joint, float angle, const bool relative, const bool isDegree)
{
    if (isDegree)
        angle = glm::radians(angle);
    if (relative)
        this->jointPositions[joint] += angle;
    else
        this->jointPositions[joint] = angle;
    const glm::vec3 rotation = static_cast<glm::vec3>(this->jointDOFs[joint]) * angle;
    this->joints[joint]->rotate(rotation, relative);
}

simulation::RobotArm::RobotArm(std::vector<const char*> meshFilenames, const char* materialFilename,
    std::vector<glm::vec3> jointOffsets, std::vector<glm::vec3> jointDOFs):
    jointOffsets(std::move(jointOffsets)), jointDOFs(std::move(jointDOFs))
{
    this->jointPositions = std::vector(meshFilenames.size(), 0.0f);
    for (const char* & meshFilename : meshFilenames) {
        Mesh* mesh = new Mesh(meshFilename, materialFilename);
        this->joints.push_back(mesh);
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
