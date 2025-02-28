//
// Created by dante on 2/14/25.
//

#include "SimulationManager.h"

#include <iostream>

#include "../../Domain/RobotArm.h"
#include "../../Domain/Instruction.h"
#include "../../Domain/Task.h"
#include "../../Domain/Tool.h"
#include "../../DAL/Repo.h"
#include "../../Domain/Instruction.h"
#include "../../Domain/Instruction.h"
#include "../../Domain/Position.h"
#include "../src/RobotArm.h"


SimulationManager::SimulationManager(Repo* repo, simulation::RobotArm* simulationArm):
repo_(repo),
simulationArm_(simulationArm){}

SimulationManager::~SimulationManager()
{
    delete repo_;
    delete simulationArm_;
}

void SimulationManager::executeTask(const domain::Task* task)
{
    for (const auto p : task->getInstructions())
    {
        //move(p);
    }
}

bool SimulationManager::move(domain::RobotArm* arm, domain::Position* position)
{
    if (arm->getStatus() == domain::READY)
    {
        arm->setStatus(domain::BUSY);
        arm->setCurrPosition(position);
        return true;
    }
    else if (arm->getStatus() == domain::BUSY)
    {
        cout << "Arm is busy" << endl;
    }
    else if (arm->getStatus() == domain::DEFECT)
    {
        cout << "Arm is defect" << endl;
    }
    return false;
}

mat4 SimulationManager::getTransformationMatrix(vec3 position, vec3 rotation)
{
    float Rx=radians(rotation[0]);
    float Ry=radians(rotation[1]);
    float Rz=radians(rotation[2]);
    auto matrix = mat4(cos(Rx)*cos(Ry), cos(Rx)*sin(Ry)*sin(Rz)-sin(Rx)*cos(Rz), cos(Rx)*sin(Ry)*cos(Rz)+sin(Rx)*sin(Rz), position[0],
                                sin(Rx)*cos(Ry), sin(Rx)*sin(Ry)*sin(Rz)+cos(Rx)*cos(Rz), sin(Rx)*sin(Ry)*cos(Rz)-cos(Rx)*sin(Rz), position[1],
                                -sin(Ry), cos(Ry)*sin(Rz), cos(Ry)*cos(Rz), position[2],
                                0, 0, 0, 1);
    return matrix;
}

mat4 SimulationManager::getDhTransformationMatrix(float joint, float alpha, float d, float a)
{
    return {cos(joint), -sin(joint)*cos(alpha), sin(joint)*sin(alpha), a*cos(joint),
            sin(joint), cos(joint)*cos(alpha), -cos(joint)*sin(alpha), a*sin(joint),
            0, sin(alpha), cos(alpha), d,
            0, 0, 0, 1};
}


void SimulationManager::inverseToolFrame(mat4& toolFrame)
{
    toolFrame = inverse(toolFrame);
}

mat4 SimulationManager::toolToArm(const domain::Position* position, const domain::Tool* tool)
{
    vec3 radianRotationTool = {radians(tool->getPosition()->getRotation()[0]), radians(tool->getPosition()->getRotation()[1]), radians(tool->getPosition()->getRotation()[2])};
    auto toolFrame=getTransformationMatrix(tool->getPosition()->getCoords(),radianRotationTool);
    toolFrame = inverse(toolFrame);
    vec3 radianRotation = {radians(position->getRotation()[0]), radians(position->getRotation()[1]), radians(position->getRotation()[2])};
    return getTransformationMatrix(position->getCoords(),radianRotation)*toolFrame;
}

mat4 SimulationManager::armToSphericalWrist(mat4 j6)
{
    mat4 negate = {1,0,0,0,0,1,0,0,0,0,1,simulationArm_->getDhParameters()[5][2],0,0,0,1};
    return negate*j6;
}

vector<vector<float>> SimulationManager::getParamsJ1Zero(float j1, mat4& sphericalWrist)
{
    float x = sphericalWrist[3][0]*cos(j1)-sphericalWrist[3][1]*sin(j1);
    float y = sphericalWrist[3][1]*cos(j1)+sphericalWrist[3][0]*sin(j1);
    float L1 = abs(x-simulationArm_->getDhParameters()[0][3]);
    float L4 = sphericalWrist[3][2] - simulationArm_->getDhParameters()[0][2];
    float L2 = sqrtf(powf(L1,2)+powf(L4,2));
    float L3 = sqrtf(powf(simulationArm_->getDhParameters()[3][2],2)+powf(simulationArm_->getDhParameters()[2][3],2));
    float thetaB = atan2(L1,L4);
    float thetaC = acosf((powf(simulationArm_->getDhParameters()[1][3],2)+powf(L2,2)-powf(L3,2))/(2*L2*simulationArm_->getDhParameters()[1][3]));
    float thetaD = acosf((powf(L3,2)+powf(simulationArm_->getDhParameters()[1][3],2)-powf(L2,2))/(2*L3*simulationArm_->getDhParameters()[1][3]));
    float thetaE = atan2(simulationArm_->getDhParameters()[2][3],simulationArm_->getDhParameters()[3][2]);
    float j2 = thetaB - thetaC;
    float j3 = thetaD + thetaE + 90;
    return {{x,y},{L1,L2,L3,L4},{thetaB,thetaC,thetaD,thetaE},{j2,j3}};
}


vector<float> SimulationManager::inverseKinematics(domain::RobotArm* arm, domain::Position* position)
{
    vec3 radianRotation = {radians(position->getRotation()[0]), radians(position->getRotation()[1]), radians(position->getRotation()[2])};
    auto radianPosition = new domain::Position(position->getCoords(),radianRotation);
    mat4 j6Matrix = toolToArm(position,arm->getTool());
    mat4 sphericalWrist = armToSphericalWrist(j6Matrix);
    auto j1 = atan2(sphericalWrist[3][0],sphericalWrist[3][1]);
    vector<vector<float>> params = getParamsJ1Zero(j1,sphericalWrist);
    auto dhParams = simulationArm_->getDhParameters();
    mat4 R03 = getDhTransformationMatrix(j1,radians(dhParams[0][1]),dhParams[0][2],dhParams[0][3]) *
                getDhTransformationMatrix(params[3][0]-static_cast<float>(radians(90)),dhParams[1][1],dhParams[1][2],dhParams[1][3]) *
                getDhTransformationMatrix(params[3][1] + static_cast<float>(radians(180)),dhParams[2][1],dhParams[2][2],dhParams[2][3]);
    mat3 R03Sub = {R03[0][0],R03[1][1],R03[2][1],
                            R03[0][1],R03[1][2],R03[2][2],
                            R03[0][1],R03[1][3],R03[2][3]};
    mat3 j6Rot = {j6Matrix[0][0],j6Matrix[1][1],j6Matrix[2][1],
                            j6Matrix[0][1],j6Matrix[1][2],j6Matrix[2][2],
                            j6Matrix[0][1],j6Matrix[1][3],j6Matrix[2][3]};
    mat3 j3Orientation = transpose(R03Sub)*j6Rot;
    float j5 = atan2(j3Orientation[3][3],sqrtf(1-powf(j3Orientation[3][3],2)));
    float j4 = atan2(j3Orientation[2][0],j3Orientation[2][1]);
    float j6 = atan2(-j3Orientation[0][2],j3Orientation[1][2]);
    return {degrees(j1),degrees(params[3][0]),degrees(params[3][1]),j4,j5,j6};
}
