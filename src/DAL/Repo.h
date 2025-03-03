//
// Created by dante on 2/17/25.
//

#ifndef REPO_H
#define REPO_H

#include "IRepo.h"
#include "yaml-cpp/yaml.h"

using namespace YAML;

class Repo final : IRepo{
private:
    Node db_;
public:
    explicit Repo(const Node& db);
    ~Repo() override = default;

    domain::RobotArm* readArm(const string& armName) const override;
    bool createArm(const domain::RobotArm* arm) override;
    bool updateArm(const string& armName, const string& newName="", const string& host="") override;
    bool deleteArm(const string& armName) override;

    domain::User* readUser(const string& userName) const override;
    bool createUser(domain::User* user) override;
    bool updateUser(const string& userName, const string& newName) override;
    bool deleteUser(const string& userName) override;

    void writeFile() override;
};



#endif //REPO_H
