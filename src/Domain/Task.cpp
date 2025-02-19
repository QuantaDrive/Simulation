//
// Created by dante on 2/12/25.
//

#include "Task.h"

Task::Task(const tm& timestamp, const vector<Instruction*>& instructions) :
timestamp_(timestamp),
instructions_(instructions)
{}

Task::~Task()
{
    for (const auto i : instructions_)
    {
        delete i;
    }
}

tm Task::getTimestamp() const
{
    return timestamp_;
}

void Task::setTimestamp(const tm& timestamp)
{
    timestamp_=timestamp;
}

vector<Instruction*> Task::getPositions() const
{
    return instructions_;
}

void Task::setPositions(const vector<Instruction*>& instructions)
{
    instructions_=instructions;
}
