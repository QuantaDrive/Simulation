//
// Created by dante on 2/12/25.
//

#include "Task.h"
#include "Instruction.h"

domain::Task::Task(const tm& timestamp, const vector<Instruction*>& instructions) :
timestamp_(timestamp),
instructions_(instructions)
{}

domain::Task::~Task()
{
    for (const auto i : instructions_)
    {
        delete i;
    }
}

tm domain::Task::getTimestamp() const
{
    return timestamp_;
}

void domain::Task::setTimestamp(const tm& timestamp)
{
    timestamp_=timestamp;
}

vector<domain::Instruction*> domain::Task::getInstructions() const
{
    return instructions_;
}

void domain::Task::setInstructions(const vector<Instruction*>& instructions)
{
    instructions_=instructions;
}
