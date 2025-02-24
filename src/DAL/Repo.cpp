//
// Created by dante on 2/17/25.
//
#include "Repo.h"

#include <iostream>
#include <fstream>

#include "../Domain/RobotArm.h"
#include "../Domain/Tool.h"
#include "../Domain/User.h"
#include "../Domain/Position.h"

using namespace YAML;

Repo::Repo(const Node& db) : IRepo()
{
    db_=db;
}

RobotArm* Repo::readArm(const string& armName) const
{
    if (db_["arms"][armName])
    {
        auto arm = db_["arms"][armName];
        auto* newArm = new RobotArm(armName,{},READY,nullptr,arm["host"].as<string>(),arm["type"].as<string>(),new Tool("default",new Position({0,0,0},{0,0,0})));
        return newArm;
    }
    return nullptr;
}

bool Repo::createArm(const RobotArm* arm)
{
    try
    {
        db_["arms"][arm->getName()]["type"] = arm->getType();
        db_["arms"][arm->getName()]["host"] = arm->getHost();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    writeFile();

    return true;
}

bool Repo::updateArm(const string& armName, const string& newName, const string& host)
{
    if (db_["arms"][armName])
    {
        if (!host.empty())
        {
            try
            {
                db_["arms"][armName]["host"] = host;
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                return false;
            }
        }
        if (!newName.empty())
        {
            try
            {
                Node oldArm = db_["arms"][armName];
                db_["arms"].remove(armName);
                db_["arms"][newName] = oldArm;
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                return false;
            }
        }
        writeFile();
        return true;
    }
    return false;
}

bool Repo::deleteArm(const string& armName)
{
    if (db_["arms"][armName])
    {
        db_["arms"].remove(armName);
        writeFile();
        return true;
    }
    return false;
}

User* Repo::readUser(const string& userName) const
{
    for (auto user:db_["users"])
    {
        if (user.as<string>()==userName)
        {
            return new User(userName);
        }
    }
    return nullptr;
}

bool Repo::createUser(User* user)
{
    if (user->getName().empty())
    {
        cerr << "User name is empty" << endl;
        return false;
    }
    for (auto i : db_["users"])
    {
        if (i.as<string>()==user->getName())
        {
            cerr << "User already exists" << endl;
            return false;
        }
    }
    db_["users"].push_back(user->getName());
    writeFile();
    return true;
}

bool Repo::updateUser(const string& userName, const string& newName)
{
    if (userName.empty() || newName.empty())
    {
        cerr << "Invalid use of update method for user. Please pass user name and new name" << endl;
        return false;
    }
    for (auto i : db_["users"])
    {
        if (i.as<string>()==userName)
        {
            deleteUser(userName);
            db_["users"].push_back(newName);
            writeFile();
            return true;
        }
    }
    cerr << "User not found" << endl;
    return false;
}

bool Repo::deleteUser(const string& userName)
{
    if (userName.empty())
    {
        cerr << "User name is empty" << endl;
        return false;
    }
    for (auto i : db_["users"])
    {
        if (i.as<string>()==userName)
        {
            auto users = db_["users"].as<list<string>>();
            users.remove(userName);
            db_["users"] = users;
            writeFile();
            return true;
        }
    }
    cerr << "User not found" << endl;
    return false;    return true;
}

void Repo::writeFile()
{
    YAML::Emitter out;
    out << db_;
    ofstream file("conf/db.yaml");
    file << out.c_str();
    file.close();
    cout << "File written" << endl;

}
