//
// Created by dante on 2/13/25.
//

#include "User.h"

string User::getName() const
{
    return name_;
}

void User::setName(const string& name)
{
    this->name_ = name;
}
