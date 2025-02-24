//
// Created by dante on 2/13/25.
//

#include "RobotArm.h"

#include "Position.h"
#include "../Domain/Instruction.h"
#include "../Domain/Task.h"

RobotArm::RobotArm(const string& name, const vector<Task*>& tasks, const Status status, Position* currPosition, const string& host, const string& type, Tool* tool) :
name_(name),
tasks_(tasks),
status_(status),
currPosition_(currPosition),
host_(host),
type_(type),
tool_(tool){}

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

Position* RobotArm::getCurrPosition() const
{
    return currPosition_;
}

void RobotArm::setCurrPosition(Position* currPosition)
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

string RobotArm::getType() const
{
    return type_;
}

void RobotArm::setType(const string& type)
{
    type_=type;
}

Tool* RobotArm::getTool() const
{
    return tool_;
}

void RobotArm::setTool(Tool* tool)
{
    tool_=tool;
}
