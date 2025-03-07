//
// Created by dante on 2/13/25.
//

#include "User.h"

#include <stdexcept>

domain::User::User(const string& name) :
name_(name){}

string domain::User::getName() const
{
    return name_;
}

void domain::User::setName(const string& name)
{
    name.empty() ? throw invalid_argument("username can't be empty") : name_ = name;
}
