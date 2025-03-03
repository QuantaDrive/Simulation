//
// Created by dante on 2/24/25.
//

#include "Tool.h"

domain::Tool::Tool(const string& name, domain::Position* position) :
name_(name),
position_(position){}

string domain::Tool::getName() const
{
    return name_;
}

void domain::Tool::setName(const string& name)
{
    name_=name;
}

domain::Position* domain::Tool::getPosition() const
{
    return position_;
}

void domain::Tool::setPosition(Position* position)
{
    position_=position;
}
