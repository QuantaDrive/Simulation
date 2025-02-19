//
// Created by dante on 2/13/25.
//

#include "RobotArm.h"

RobotArm::RobotArm(const string& name, const vector<Task*>& tasks, Status status, Instruction* currPosition, const string& host) :
name_(name),
tasks_(tasks),
status_(status),
currPosition_(currPosition),
host_(host){}

RobotArm::~RobotArm()
{
    for (const auto t: tasks_)
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
    status_=status;
}

vector<Task*> RobotArm::getTasks() const
{
    return tasks_;
}

void RobotArm::setTasks(const vector<Task*>& tasks)
{
    tasks_=tasks;
}

Instruction* RobotArm::getCurrPosition() const
{
    return currPosition_;
}

void RobotArm::setCurrPosition(Instruction* currPosition)
{
    currPosition_=currPosition;
}

string RobotArm::getHost() const
{
    return host_;
}

void RobotArm::setHost(const string& host)
{
    host_=host;
}

string RobotArm::getName() const
{
    return name_;
}

void RobotArm::setName(const string& name)
{
    name_=name;
}
