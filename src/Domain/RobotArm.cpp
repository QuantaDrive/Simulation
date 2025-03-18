//
// Created by dante on 2/13/25.
//

#include "RobotArm.h"

#include <stdexcept>

#include "Tool.h"
#include "../Domain/Instruction.h"
#include "../Domain/Task.h"

domain::RobotArm::RobotArm(const string& name, Task* task, const Status status, Position* currPosition, const string& host, const string& type, Tool* tool) :
name_(name),
task_(task),
status_(status),
currPosition_(currPosition),
host_(host),
type_(type),
tool_(tool){}

domain::RobotArm::~RobotArm()
{
    delete task_;
    delete tool_;
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

domain::Task* domain::RobotArm::getTask() const
{
    return task_;
}

void domain::RobotArm::setTask(Task* task)
{
    task_=task;
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

bool domain::RobotArm::isAbsolute() const {
    return isAbsolute_;
}

void domain::RobotArm::setAbsolute(bool is_absolute) {
    isAbsolute_ = is_absolute;
}
