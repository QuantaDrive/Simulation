//
// Created by dante on 2/12/25.
//

#include "Task.h"

Task::Task(const tm& timestamp, const vector<ArmPosition*>& positions)
{
    this->timestamp=timestamp;
    this->positions=positions;
}

Task::~Task()
{
    for (auto p : positions)
    {
        delete p;
    }
}

tm Task::getTimestamp() const
{
    return timestamp;
}

void Task::setTimestamp(const tm& timestamp)
{
    this->timestamp=timestamp;
}

vector<ArmPosition*> Task::getPositions() const
{
    return positions;
}

void Task::setPositions(const vector<ArmPosition*>& positions)
{
    this->positions=positions;
}
