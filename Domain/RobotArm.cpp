//
// Created by dante on 2/13/25.
//

#include "RobotArm.h"

RobotArm::~RobotArm()
{
    for (auto t: tasks)
    {
        delete t;
    }
}

Status RobotArm::getStatus() const
{
    return status;
}

void RobotArm::setStatus(Status status)
{
    this->status=status;
}

vector<Task*> RobotArm::getTasks() const
{
    return tasks;
}

void RobotArm::setTasks(const vector<Task*>& tasks)
{
    this->tasks=tasks;
}
