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
#include "../../Domain/Position.h"


SimulationManager::SimulationManager(Repo* repo)
{
    repo_=repo;
}

SimulationManager::~SimulationManager()
{
    delete repo_;
}

void SimulationManager::executeTask(const Task* task)
{
    for (const auto p : task->getInstructions())
    {
        //move(p);
    }
}

bool SimulationManager::move(RobotArm* arm, Position* position)
{
    if (arm->getStatus() == READY)
    {
        arm->setStatus(BUSY);
        arm->setCurrPosition(position);
        return true;
    }
    else if (arm->getStatus() == BUSY)
    {
        cout << "Arm is busy" << endl;
    }
    else if (arm->getStatus() == DEFECT)
    {
        cout << "Arm is defect" << endl;
    }
    return false;
}

mat4 SimulationManager::getTransformationMatrix(vec3 position, vec3 rotation)
{
    float Rx=rotation[0];
    float Ry=rotation[1];
    float Rz=rotation[2];
    auto matrix = mat4(cos(Rx)*cos(Ry), cos(Rx)*sin(Ry)*sin(Rz)-sin(Rx)*cos(Rz), cos(Rx)*sin(Ry)*cos(Rz)+sin(Rx)*sin(Rz), position[0],
                                sin(Rx)*cos(Ry), sin(Rx)*sin(Ry)*sin(Rz)+cos(Rx)*cos(Rz), sin(Rx)*sin(Ry)*cos(Rz)-cos(Rx)*sin(Rz), position[1],
                                -sin(Ry), cos(Ry)*sin(Rz), cos(Ry)*cos(Rz), position[2],
                                0, 0, 0, 1);
    return matrix;
}

void SimulationManager::inverseToolFrame(mat4& toolFrame)
{
    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {

        }
    }
}

mat4 SimulationManager::toolToArm(const Position* position, const Tool* tool)
{
    auto toolFrame=getTransformationMatrix(tool->getPosition()->getCoords(),tool->getPosition()->getRotation());
    inverseToolFrame(toolFrame);
    return getTransformationMatrix(position->getCoords(),position->getRotation())*toolFrame;
}

void SimulationManager::armToSphericalWrist(mat4& endOfArm)
{

}

void SimulationManager::inverseKinematics(RobotArm* arm, Position* position)
{
    mat4 endofarm = toolToArm(position,arm->getTool());
    armToSphericalWrist(endofarm);
    cout << to_string(endofarm[0][3]) << endl;
}
