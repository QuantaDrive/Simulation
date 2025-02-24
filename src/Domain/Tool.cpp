//
// Created by dante on 2/24/25.
//

#include "Tool.h"

Tool::Tool(const string& name, Position* position) :
name_(name),
position_(position){}

string Tool::getName() const
{
    return name_;
}

void Tool::setName(const string& name)
{
    name_=name;
}

Position* Tool::getPosition() const
{
    return position_;
}

void Tool::setPosition(Position* position)
{
    position_=position;
}
