//
// Created by dante on 2/17/25.
//
#include "Repo.h"

#include <iostream>
#include <fstream>

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
        auto* newArm = new RobotArm(armName,{},READY,nullptr,arm["host"].as<string>(),arm["type"].as<string>());
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
            }
        }
    }
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

void Repo::writeFile()
{
    YAML::Emitter out;
    out << db_;
    ofstream file("conf/db.yaml");
    file << out.c_str();
    file.close();
    cout << "File written" << endl;

}
