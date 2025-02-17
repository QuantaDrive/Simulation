//
// Created by dante on 2/17/25.
//
#include "Repo.h"

#include <iostream>

using namespace YAML;

Repo::Repo(Node db)
{
    db_=db;
}

RobotArm* Repo::readArm(const string& armName) const
{
    if (db_["arms"][armName])
    {
        Node arm = db_["arms"][armName];
        Status status=UNKNOWN;



    }
    return nullptr;
}

bool Repo::createArm(RobotArm* arm)
{
    return true;
}

bool Repo::updateArm()
{
    return true;
}

bool Repo::deleteArm(const string& armName)
{
    return true;
}

User* Repo::readUser(const string& userName) const
{
    if (db_["users"][userName])
    {
        Node user=db_["users"][userName];
        auto* newUser = new User(user["name"].as<string>());
        return newUser;
    }
    return nullptr;
}

bool Repo::createUser(User* user)
{
    return true;
}

bool Repo::updateUser()
{
    return true;
}

bool Repo::deleteUser(const string& userName)
{
    return true;
}