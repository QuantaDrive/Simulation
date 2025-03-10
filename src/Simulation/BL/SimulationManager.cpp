//
// Created by dante on 2/14/25.
//

#include "SimulationManager.h"

#include <iostream>
#include <unistd.h>

#include "../../Domain/RobotArm.h"
#include "../../Domain/Instruction.h"
#include "../../Domain/Task.h"
#include "../../Domain/Tool.h"
#include "../../DAL/Repo.h"
#include "../../Domain/Instruction.h"
#include "../../Domain/Instruction.h"
#include "../../Domain/Position.h"
#include "../src/RobotArm.h"

SimulationManager::SimulationManager(Repo *repo, simulation::RobotArm *simulationArm): repo_(repo),
    simulationArm_(simulationArm) {
    robotArm_ = repo_->readArm(simulationArm_->getName());
}

SimulationManager::~SimulationManager() {
    delete repo_;
    delete simulationArm_;
}

//
// Movements
//

void SimulationManager::executeInstruction(const domain::Instruction *instruction) {
    // const auto arm = repo_->readArm(simulationArm_->getName());
    const auto currentPosition = robotArm_->getCurrPosition();
    if (instruction->getWait() > 0) sleep(instruction->getWait());
    else if (instruction->getGripForce() > 0) grip(instruction->getGripForce());
    else if (instruction->isGoHome()) move(new domain::Position({0, 0, 0}, {0, 0, 0}));
    else if (instruction->isRelative()) move(new domain::Position({
                                                                      currentPosition->getCoords()[0] + instruction->
                                                                      getRelMove()[0],
                                                                      currentPosition->getCoords()[1] + instruction->
                                                                      getRelMove()[1],
                                                                      currentPosition->getCoords()[2] + instruction->
                                                                      getRelMove()[2]
                                                                  }, currentPosition->getRotation()));
    else move(instruction->getPosition());
}

void SimulationManager::executeTask(const domain::Task *task) {
    for (const auto i: task->getInstructions()) {
        executeInstruction(i);
    }
}

vector<domain::Position*> SimulationManager::interpolate(const domain::Position* currentPosition, domain::Position* newPosition) {
    auto currP = currentPosition->getCoords();
    auto newP = newPosition->getCoords();

    // Calculate max reach based on arm segments (using DH parameters)
    float maxReach = simulationArm_->getDhParameters()[1][3] +  // a2
                     simulationArm_->getDhParameters()[2][3] +  // a3
                     simulationArm_->getDhParameters()[3][2];   // d4

    // Calculate distance to target
    float d = sqrtf(powf(currP[0] - newP[0], 2) + powf(currP[1] - newP[1], 2) + powf(currP[2] - newP[2], 2));

    // If target is beyond reach, scale down the position
    if (d > maxReach) {
        float scale = maxReach / d;
        newP[0] = currP[0] + (newP[0] - currP[0]) * scale;
        newP[1] = currP[1] + (newP[1] - currP[1]) * scale;
        newP[2] = currP[2] + (newP[2] - currP[2]) * scale;
        d = maxReach;
    }

    vector<domain::Position*> interpolpoints = {};
    for (int i = 1; i < static_cast<int>(round(d)); i++) {
        float x = currP[0] + static_cast<float>(i) / d * (newP[0] - currP[0]);
        float y = currP[1] + static_cast<float>(i) / d * (newP[1] - currP[1]);
        float z = currP[2] + static_cast<float>(i) / d * (newP[2] - currP[2]);
        interpolpoints.emplace_back(new domain::Position({x, y, z}, currentPosition->getRotation()));
    }
    return interpolpoints;
}

bool SimulationManager::move(domain::Position *position) {
    // auto arm = repo_->readArm(simulationArm_->getName());
    if (robotArm_->getStatus() == domain::READY) {
        robotArm_->setStatus(domain::BUSY);
        auto interpolPos = interpolate(robotArm_->getCurrPosition(), position);
        for (auto pos: interpolPos) {
            auto angles = inverseKinematics(pos);
            for (int i = 0; i < angles.size(); i++) {
                simulationArm_->moveAngle(i + 1, angles[i], false, true);
            }
            usleep(5000);
            simulation::refresh();
            simulationArm_->render();
        }
        robotArm_->setCurrPosition(position);
        robotArm_->setStatus(domain::READY);
        return true;
    }
    if (robotArm_->getStatus() == domain::BUSY) {
        cout << "Arm is busy" << endl;
    } else if (robotArm_->getStatus() == domain::DEFECT) {
        cout << "Arm is defect" << endl;
    }
    return false;
}

void SimulationManager::grip(float gripForce) {
    cout << "gripped with force: " << gripForce << endl;
}

//
// Inverse Kinematics
//

mat4 SimulationManager::getTransformationMatrix(vec3 position, vec3 rotation) {
    const float Rx = radians(rotation[0]);
    const float Ry = radians(rotation[1]);
    const float Rz = radians(rotation[2]);
    auto matrix = mat4(cos(Rx) * cos(Ry), cos(Rx) * sin(Ry) * sin(Rz) - sin(Rx) * cos(Rz),
                       cos(Rx) * sin(Ry) * cos(Rz) + sin(Rx) * sin(Rz), position[0],
                       sin(Rx) * cos(Ry), sin(Rx) * sin(Ry) * sin(Rz) + cos(Rx) * cos(Rz),
                       sin(Rx) * sin(Ry) * cos(Rz) - cos(Rx) * sin(Rz), position[1],
                       -sin(Ry), cos(Ry) * sin(Rz), cos(Ry) * cos(Rz), position[2],
                       0, 0, 0, 1);
    return matrix;
}

mat4 SimulationManager::getDhTransformationMatrix(float joint, float alpha, float d, const float a) {
    joint = radians(joint);
    alpha = radians(alpha);
    return {
        cos(joint), -sin(joint) * cos(alpha), sin(joint) * sin(alpha), a * cos(joint),
        sin(joint), cos(joint) * cos(alpha), -cos(joint) * sin(alpha), a * sin(joint),
        0, sin(alpha), cos(alpha), d,
        0, 0, 0, 1
    };
}

mat4 SimulationManager::toolToArm(const domain::Position *position, const domain::Tool *tool) {
    vec3 radianRotationTool = {
        radians(tool->getPosition()->getRotation()[0]), radians(tool->getPosition()->getRotation()[1]),
        radians(tool->getPosition()->getRotation()[2])
    };
    auto toolFrame = getTransformationMatrix(tool->getPosition()->getCoords(), radianRotationTool);
    toolFrame = inverse(toolFrame);
    return getTransformationMatrix(position->getCoords(), position->getRotation()) * toolFrame;
}

mat4 SimulationManager::armToSphericalWrist(const mat4 &j6) {
    mat4 negate = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, -simulationArm_->getDhParameters()[5][2], 0, 0, 0, 1};
    return negate * j6;
}

vector<vector<float> > SimulationManager::getParamsJ1Zero(mat4 &sphericalWrist) {
    float j1 = 0;
    float x = sphericalWrist[0][3] * cos(radians(-j1)) - sphericalWrist[1][3] * sin(radians(-j1));
    float y = sphericalWrist[1][3] * cos(radians(-j1)) + sphericalWrist[0][3] * sin(radians(-j1));
    float L1 = abs(x - simulationArm_->getDhParameters()[0][3]);
    float L4 = sphericalWrist[2][3] - simulationArm_->getDhParameters()[0][2];
    float L2 = sqrtf(powf(L1, 2) + powf(L4, 2));
    float L3 = sqrtf(
        powf(simulationArm_->getDhParameters()[3][2], 2) + powf(simulationArm_->getDhParameters()[2][3], 2));
    float thetaB = degrees(atan2(L1, L4));
    float thetaC = degrees(acosf(
        (powf(simulationArm_->getDhParameters()[1][3], 2) + powf(L2, 2) - powf(L3, 2)) / (
            2 * L2 * simulationArm_->getDhParameters()[1][3])));
    float thetaD = degrees(acosf(
        (powf(L3, 2) + powf(simulationArm_->getDhParameters()[1][3], 2) - powf(L2, 2)) / (
            2 * L3 * simulationArm_->getDhParameters()[1][3])));
    float thetaE = degrees(atan2(simulationArm_->getDhParameters()[2][3], simulationArm_->getDhParameters()[3][2]));
    float j2 = thetaB - thetaC;
    float j3 = -(thetaD + thetaE) + 90;
    return {{x, y}, {L1, L2, L3, L4}, {thetaB, thetaC, thetaD, thetaE}, {j2, j3}};
}


vector<float> SimulationManager::inverseKinematics(domain::Position *position) {
    // auto arm = repo_->readArm(simulationArm_->getName());
    mat4 j6Matrix = toolToArm(position, robotArm_->getTool());
    mat4 sphericalWrist = armToSphericalWrist(j6Matrix);
    auto j1 = degrees(atan2(sphericalWrist[1][3], sphericalWrist[0][3]));
    vector<vector<float> > params = getParamsJ1Zero(sphericalWrist);
    auto dhParams = simulationArm_->getDhParameters();
    mat4 j1M = getDhTransformationMatrix(j1, dhParams[0][1], dhParams[0][2], dhParams[0][3]);
    mat4 j2M = getDhTransformationMatrix(params[3][0] - 90.0f, dhParams[1][1], dhParams[1][2], dhParams[1][3]);
    mat4 j3M = getDhTransformationMatrix(params[3][1] + 180.0f, dhParams[2][1], dhParams[2][2], dhParams[2][3]);
    mat4 R02 = j2M * j1M;
    mat4 R03 = j3M * R02;
    mat3 R03Sub = {
        R03[0][0], R03[0][1], R03[0][2],
        R03[1][0], R03[1][1], R03[1][2],
        R03[2][0], R03[2][1], R03[2][2]
    };
    mat3 j6Rot = {
        j6Matrix[0][0], j6Matrix[0][1], j6Matrix[0][2],
        j6Matrix[1][0], j6Matrix[1][1], j6Matrix[1][2],
        j6Matrix[2][0], j6Matrix[2][1], j6Matrix[2][2]
    };
    mat3 j3Orientation = j6Rot * transpose(R03Sub);
    auto result = degrees(atan2(sqrtf(1 - powf(j3Orientation[2][2], 2)), j3Orientation[2][2]));
    float j5 = 0;
    float mode = degrees(atan2(sqrtf(1 - powf(j3Orientation[2][2], 2)), j3Orientation[2][2]));
    mode > 0 ? j5 = mode : j5 = degrees(atan2(-sqrtf(1 - powf(j3Orientation[2][2], 2)), j3Orientation[2][2]));
    float j4, j6;
    if (mode > 0) {
        j4 = degrees(atan2(j3Orientation[1][2], j3Orientation[0][2]));
        j6 = degrees(atan2(j3Orientation[2][1], -j3Orientation[2][0]));
    } else {
        j4 = degrees(atan2(-j3Orientation[1][2], -j3Orientation[0][2]));
        j6 = degrees(atan2(j3Orientation[2][1], -j3Orientation[2][0]));
    }
    return {j1, params[3][0], params[3][1], j4, j5, j6};
}

// Camera controls
void SimulationManager::initializeCamera() {
    m_CameraPosition = glm::vec3(0, 750/2, 1000);
    updateCameraPosition();
}

void SimulationManager::handleMouseDrag(double xpos, double ypos) {
    if (!m_DragActive) return;

    if (m_FirstMouse) {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
        return;
    }

    float xoffset = xpos - m_LastX;
    float yoffset = m_LastY - ypos;
    m_LastX = xpos;
    m_LastY = ypos;

    const float sensitivity = 0.3f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // Constrain pitch to avoid flipping
    m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

    updateCameraPosition();
}

void SimulationManager::handleMouseScroll(double yoffset) {
    float zoomSpeed = 50.0f;
    m_CameraDistance -= yoffset * zoomSpeed;
    m_CameraDistance = glm::clamp(m_CameraDistance, 200.0f, 2000.0f);

    updateCameraPosition();
}

void SimulationManager::setDragActive(bool active) {
    m_DragActive = active;
    m_FirstMouse = true;
}

void SimulationManager::updateCameraPosition() {
    // Calculate new camera position using spherical coordinates
    float x = m_CameraTarget.x + m_CameraDistance * cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw));
    float y = m_CameraTarget.y + m_CameraDistance * sin(glm::radians(m_Pitch));
    float z = m_CameraTarget.z + m_CameraDistance * cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw));

    m_CameraPosition = glm::vec3(x, y, z);
    simulation::lookAt(m_CameraPosition, m_CameraTarget);
    simulation::lightPosition(m_CameraPosition);
}
