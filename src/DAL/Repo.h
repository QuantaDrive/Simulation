//
// Created by dante on 2/17/25.
//

#ifndef REPO_H
#define REPO_H
#include "IRepo.h"
#include "../Domain/RobotArm.h"
#include "../Domain/User.h"
#include "yaml-cpp/yaml.h"

using namespace YAML;

class Repo : IRepo{
private:
    Node db_;
public:
    explicit Repo(const Node& db);
    ~Repo() override = default;

    RobotArm* readArm(const string& armName) const override;
    bool createArm(const RobotArm* arm) override;
    bool updateArm(const string& armName, const string& newName="", const string& host="") override;
    bool deleteArm(const string& armName) override;

    User* readUser(const string& userName) const override;
    bool createUser(User* user) override;
    bool updateUser(const string& userName, const string& newName) override;
    bool deleteUser(const string& userName) override;

    void writeFile() override;
};



#endif //REPO_H
