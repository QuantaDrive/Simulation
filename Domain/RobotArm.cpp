//
// Created by dante on 2/13/25.
//

#include "RobotArm.h"

RobotArm::~RobotArm()
{
    for (auto t: tasks_)
    {
        delete t;
    }
    delete currPosition_;
}

Status RobotArm::getStatus() const
{
    return status_;
}

void RobotArm::setStatus(Status status)
{
    this->status_=status;
}

vector<Task*> RobotArm::getTasks() const
{
    return tasks_;
}

void RobotArm::setTasks(const vector<Task*>& tasks)
{
    this->tasks_=tasks;
}
