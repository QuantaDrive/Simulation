//
// Created by dante on 2/13/25.
//

#include "RobotArm.h"

#include <stdexcept>

#include "Position.h"
#include "../Domain/Instruction.h"
#include "../Domain/Task.h"

domain::RobotArm::RobotArm(const string& name, const vector<Task*>& tasks, const Status status, Position* currPosition, const string& host, const string& type, Tool* tool) :
name_(name),
tasks_(tasks),
status_(status),
currPosition_(currPosition),
host_(host),
type_(type),
tool_(tool){}

domain::RobotArm::~RobotArm()
{
    for (const auto t: tasks_)
    {
        delete t;
    }
    delete currPosition_;
}

domain::Status domain::RobotArm::getStatus() const
{
    return status_;
}

void domain::RobotArm::setStatus(Status status)
{
    status_=status;
}

vector<domain::Task*> domain::RobotArm::getTasks() const
{
    return tasks_;
}

void domain::RobotArm::setTasks(const vector<Task*>& tasks)
{
    tasks_=tasks;
}

domain::Position* domain::RobotArm::getCurrPosition() const
{
    return currPosition_;
}

void domain::RobotArm::setCurrPosition(Position* currPosition)
{
    currPosition_=currPosition;
}

string domain::RobotArm::getHost() const
{
    return host_;
}

void domain::RobotArm::setHost(const string& host)
{
    host_=host;
}

string domain::RobotArm::getName() const
{
    return name_;
}

void domain::RobotArm::setName(const string& name)
{
    name.empty() ? throw invalid_argument("name can't be empty") : name_=name;
}

string domain::RobotArm::getType() const
{
    return type_;
}

void domain::RobotArm::setType(const string& type)
{
    type.empty() ? throw invalid_argument("type can't be empty") : type_=type;
}

domain::Tool* domain::RobotArm::getTool() const
{
    return tool_;
}

void domain::RobotArm::setTool(Tool* tool)
{
    tool_=tool;
}
