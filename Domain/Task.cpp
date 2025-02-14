//
// Created by dante on 2/12/25.
//

#include "Task.h"

Task::Task(const tm& timestamp, const vector<ArmPosition*>& positions) : timestamp_(timestamp), positions_(positions){}

Task::~Task()
{
    for (auto p : positions_)
    {
        delete p;
    }
}

tm Task::getTimestamp() const
{
    return timestamp_;
}

void Task::setTimestamp(const tm& timestamp)
{
    this->timestamp_=timestamp;
}

vector<ArmPosition*> Task::getPositions() const
{
    return positions_;
}

void Task::setPositions(const vector<ArmPosition*>& positions)
{
    this->positions_=positions;
}
