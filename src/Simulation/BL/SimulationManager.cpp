//
// Created by dante on 2/14/25.
//

#include "SimulationManager.h"

#include <iostream>
#include <unistd.h>
#include <glm/gtc/constants.hpp>

#include "../../Domain/RobotArm.h"
#include "../../Domain/Instruction.h"
#include "../../Domain/Task.h"
#include "../../Domain/Tool.h"
#include "../../DAL/Repo.h"
#include "../../Domain/Position.h"
#include "../src/RobotArm.h"

SimulationManager::SimulationManager(Repo *repo, simulation::RobotArm *simulationArm): repo_(repo),
    simulationArm_(simulationArm) {
    robotArm_ = repo_->readArm(simulationArm_->getName());
}

SimulationManager::~SimulationManager() {
    delete repo_;
    delete simulationArm_;
    delete robotArm_;
    delete lastPreviewPosition;
}

void SimulationManager::setRobotArm(domain::RobotArm *robotArm) {
    robotArm_ = robotArm;
}

domain::RobotArm *SimulationManager::getRobotArm() {
    return robotArm_;
}

//
// Movements
//

void SimulationManager::executeInstruction(const domain::Instruction *instruction) {
    // const auto arm = repo_->readArm(simulationArm_->getName());
    const auto currentPosition = robotArm_->getCurrPosition();
    try {
        if (instruction->getWait() > 0) sleep(instruction->getWait());
        else if (instruction->getGripForce() > 0) grip(instruction->getGripForce());
        else if (instruction->isGoHome())
            move(new domain::Position({0, 0, 754.5}, {0, 0, 0}),
                 instruction->getVelocity());

        if (instruction->isRapid()) {
            if (robotArm_->isAbsolute()) {
                move(instruction->getPosition(), simulationArm_->getMaxVel());
            } else {
                move(new domain::Position({
                                              currentPosition->getCoords()[0] + instruction->
                                              getPosition()->getCoords()[0],
                                              currentPosition->getCoords()[1] + instruction->
                                              getPosition()->getCoords()[1],
                                              currentPosition->getCoords()[2] + instruction->
                                              getPosition()->getCoords()[2]
                                          }, currentPosition->getRotation()), simulationArm_->getMaxVel());
            }
        }
        // if instruction is Linear
        else if (instruction->isLinear()) {
            if (robotArm_->isAbsolute()) {
                move(instruction->getPosition(), instruction->getVelocity());
            } else {
                move(new domain::Position({
                                              currentPosition->getCoords()[0] + instruction->
                                              getPosition()->getCoords()[0],
                                              currentPosition->getCoords()[1] + instruction->
                                              getPosition()->getCoords()[1],
                                              currentPosition->getCoords()[2] + instruction->
                                              getPosition()->getCoords()[2]
                                          }, currentPosition->getRotation()), instruction->getVelocity());
            }
        }
    } catch (logic_error e) {
    }
}

void SimulationManager::executeTask(const domain::Task *task) {
    for (const auto i: task->getInstructions()) {
        executeInstruction(i);
    }
}

vector<domain::Position *> SimulationManager::interpolate(const domain::Position *currentPosition,
                                                          domain::Position *newPosition) {
    auto currP = currentPosition->getCoords();
    auto newP = newPosition->getCoords();

    float d = sqrtf(powf(currP[0] - newP[0], 2) + powf(currP[1] - newP[1], 2) + powf(currP[2] - newP[2], 2));

    vector<domain::Position *> interpolpoints = {};
    for (int i = 1; i < static_cast<int>(round(d)); i++) {
        float x = currP[0] + static_cast<float>(i) / d * (newP[0] - currP[0]);
        float y = currP[1] + static_cast<float>(i) / d * (newP[1] - currP[1]);
        float z = currP[2] + static_cast<float>(i) / d * (newP[2] - currP[2]);
        interpolpoints.emplace_back(new domain::Position({x, y, z}, newPosition->getRotation()));
    }
    return interpolpoints;
}

bool SimulationManager::move(domain::Position *position, float velocity) {
    // auto arm = repo_->readArm(simulationArm_->getName());
    if (robotArm_->getStatus() == domain::READY) {
        robotArm_->setStatus(domain::BUSY);
        vector<float> anglestest = {};
        try {
            anglestest = inverseKinematics(position);
        } catch (logic_error e) {
            robotArm_->setStatus(domain::READY);
            throw;
        }
        // for (auto angle:anglestest)
        // {
        //     cout << angle << endl;
        // }
        auto interpolPos = interpolate(robotArm_->getCurrPosition(), position);
        vector<vector<float> > allAngles = {};
        for (auto pos: interpolPos) {
            vector<float> angles = {};
            try {
                // cout << "Position: " << pos->getCoords()[0] << " " << pos->getCoords()[1] << " " << pos->getCoords()[2] << endl;
                angles = inverseKinematics(pos);
            } catch (logic_error e) {
                robotArm_->setStatus(domain::READY);
                throw;
            }
            allAngles.emplace_back(angles);
            delete pos;
        }
        for (auto angles: allAngles) {
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

void SimulationManager::setRotationOfHead(vec3 rotation) {
    auto currentPosition = robotArm_->getCurrPosition();
    // update it with the new rotation
    currentPosition->setRotation(rotation);
    robotArm_->setCurrPosition(currentPosition);
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
    auto matrix2 = mat4(cos(Rx) * cos(Ry), sin(Rx) * cos(Ry), -sin(Ry), 0,
                        cos(Rx) * sin(Ry) * sin(Rz) - sin(Rx) * cos(Rz),
                        sin(Rx) * sin(Ry) * sin(Rz) + cos(Rx) * cos(Rz), cos(Ry) * sin(Rz), 0,
                        cos(Rx) * sin(Ry) * cos(Rz) + sin(Rx) * sin(Rz),
                        sin(Rx) * sin(Ry) * cos(Rz) - cos(Rx) * sin(Rz), cos(Ry) * cos(Rz), 0,
                        position[0], position[1], position[2], 1);
    return matrix2;
}

mat4 SimulationManager::getDhTransformationMatrix(float joint, float alpha, float d, const float a) {
    joint = radians(joint);
    alpha = radians(alpha);
    return {
        cos(joint), sin(joint), 0, 0,
        -sin(joint) * cos(alpha), cos(joint) * cos(alpha), sin(alpha), 0,
        sin(joint) * sin(alpha), -cos(joint) * sin(alpha), cos(alpha), 0,
        a * cos(joint), a * sin(joint), d, 1
    };
}

mat4 SimulationManager::toolToArm(const domain::Position *position, const domain::Tool *tool) {
    auto positionMat = getTransformationMatrix(position->getCoords(), position->getRotation());
    vec3 radianRotationTool = {
        radians(tool->getPosition()->getRotation()[0]), radians(tool->getPosition()->getRotation()[1]),
        radians(tool->getPosition()->getRotation()[2])
    };
    auto toolFrame = getTransformationMatrix(tool->getPosition()->getCoords(), radianRotationTool);
    toolFrame = inverse(toolFrame);
    return positionMat * toolFrame;
}

mat4 SimulationManager::armToSphericalWrist(const mat4 &j6) {
    mat4 negate = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -simulationArm_->getDhParameters()[5][2], 1};
    return j6 * negate;
}

vector<vector<float> > SimulationManager::getParamsJ1Zero(mat4 &sphericalWrist) {
    float j1 = 0;
    float x = sphericalWrist[3][0] * cos(radians(-j1)) - sphericalWrist[3][1] * sin(radians(-j1));
    float y = sphericalWrist[3][1] * cos(radians(-j1)) + sphericalWrist[3][0] * sin(radians(-j1));
    float L1 = abs(x - simulationArm_->getDhParameters()[0][3]);
    float L4 = sphericalWrist[3][2] - simulationArm_->getDhParameters()[0][2];
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
    float j3 = -(thetaD + thetaE) + 180;
    // cout << "thetaC = " << thetaC << " thetaD = " << thetaD << " j2 = " << j2 << " j3 = " << j3 << endl;
    if (thetaC != thetaC || thetaD != thetaD || j2 != j2 || j3 != j3)
        throw
                logic_error("coordinates out of arms reach");
    return {{x, y}, {L1, L2, L3, L4}, {thetaB, thetaC, thetaD, thetaE}, {j2, j3}};
}


vector<float> SimulationManager::inverseKinematics(domain::Position *position) {
    mat4 j6Matrix = toolToArm(position, robotArm_->getTool());
    mat4 sphericalWrist = armToSphericalWrist(j6Matrix);
    auto j1 = degrees(atan2(sphericalWrist[3][1], sphericalWrist[3][1]));
    cout << "j1: " << j1 << endl;
    vector<vector<float> > params = {};
    try {
        params = getParamsJ1Zero(sphericalWrist);
    } catch (logic_error e) {
        cerr << e.what() << endl;
        throw;
    }
    auto dhParams = simulationArm_->getDhParameters();
    mat4 j1M = getDhTransformationMatrix(j1 + dhParams[0][0], dhParams[0][1], dhParams[0][2], dhParams[0][3]);
    mat4 j2M = getDhTransformationMatrix(params[3][0] + dhParams[1][0], dhParams[1][1], dhParams[1][2], dhParams[1][3]);
    mat4 j3M = getDhTransformationMatrix(params[3][1] + dhParams[2][0], dhParams[2][1], dhParams[2][2], dhParams[2][3]);
    mat4 R02 = j1M * j2M;
    mat4 R03 = R02 * j3M;
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
    mat3 j3Orientation = transpose(R03Sub) * j6Rot;
    float j5 = degrees(atan2(sqrtf(1 - powf(j3Orientation[2][2], 2)), j3Orientation[2][2]));
    if (j5 <= 0) j5 = degrees(atan2(-sqrtf(1 - powf(j3Orientation[2][2], 2)), j3Orientation[2][2]));
    // cout << "j5: " << j5 << endl;
    float j4, j6;
    if (j5 > 0) {
        j4 = degrees(atan2(j3Orientation[2][1], -j3Orientation[2][0]));
        j6 = degrees(atan2(j3Orientation[1][2], j3Orientation[0][2]));
    } else {
        j4 = degrees(atan2(-j3Orientation[2][1], j3Orientation[2][0]));
        j6 = degrees(atan2(-j3Orientation[1][2], -j3Orientation[0][2]));
    }
    return {j1, params[3][0], params[3][1], j4, j5, j6};
}

// Camera controls
void SimulationManager::initializeCamera() {
    m_CameraPosition = glm::vec3(0, 750 / 2, 1000);
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

void SimulationManager::setAbsolute(bool is_absolute) {
    robotArm_->setAbsolute(is_absolute);
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

vector<float> SimulationManager::calculateFinalAngles(domain::Position *targetPosition) {
    try {
        return inverseKinematics(targetPosition);
    } catch (logic_error &e) {
        throw;
    }
}

bool SimulationManager::hasPositionChanged(const domain::Position *newPosition) const {
    if (!lastPreviewPosition) return true;

    const auto &newCoords = newPosition->getCoords();
    const auto &lastCoords = lastPreviewPosition->getCoords();
    const auto &newRot = newPosition->getRotation();
    const auto &lastRot = lastPreviewPosition->getRotation();

    return newCoords != lastCoords || newRot != lastRot;
}


bool SimulationManager::startPreview(domain::Position* position) {
    if (!isPreviewActive) {
        isPreviewActive = true;
    }

    try {
        // Get interpolated positions
        auto currentPosition = robotArm_->getCurrPosition();
        auto interpolatedPositions = interpolate(currentPosition, position);

        // Add final position to check
        interpolatedPositions.push_back(position);

        // Try to calculate angles for all positions
        vector<vector<float>> allAngles;
        for (const auto& pos : interpolatedPositions) {
            try {
                allAngles.push_back(inverseKinematics(pos));
            } catch (const std::logic_error& e) {
                // Clean up temporary positions
                for (auto p : interpolatedPositions) {
                    if (p != position) delete p;
                }
                endPreview();
                return false;
            }
        }
        // Apply only the final angles for preview
        previewAngles = allAngles.back();
        for (int i = 0; i < previewAngles.size(); ++i) {
            simulationArm_->moveAngle(i + 1, previewAngles[i], false, true);
        }

        // Clean up temporary positions
        for (auto p : interpolatedPositions) {
            if (p != position) delete p;
        }

        return true;

    } catch (const std::exception& e) {
        endPreview();
        throw;
    }
}

void SimulationManager::endPreview() {
    if (isPreviewActive) {
        // Use current position from robot arm
        auto currentPos = robotArm_->getCurrPosition();
        vector<float> angles = calculateFinalAngles(currentPos);

        for (int i = 0; i < angles.size(); ++i) {
            simulationArm_->moveAngle(i + 1, angles[i], false, true);
        }
        simulation::refresh();
        simulationArm_->render();
        isPreviewActive = false;
    }
}
