//
// Created by dante on 2/14/25.
//

#include "SimulationManager.h"

#include <iostream>

#include "../../Domain/RobotArm.h"

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
    for (const auto p : task->getPositions())
    {
        //move(p);
    }
}

bool SimulationManager::move(RobotArm* arm, Instruction* position)
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
