//
// Created by dante on 2/13/25.
//

#include "User.h"

User::User(const string& name) :
name_(name){}

string User::getName() const
{
    return name_;
}

void User::setName(const string& name)
{
    name_ = name;
}
