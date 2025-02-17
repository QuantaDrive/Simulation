//
// Created by dante on 2/17/25.
//

#ifndef REPO_H
#define REPO_H

#include "../Domain/RobotArm.h"
#include "../Domain/User.h"
#include "yaml-cpp/yaml.h"



class Repo {
private:
    YAML::Node db_;
public:
    Repo(YAML::Node db);

    RobotArm* readArm(const string& armName) const;
    bool createArm(RobotArm* arm);
    bool updateArm();
    bool deleteArm(const string& armName);

    User* readUser(const string& userName) const;
    bool createUser(User* user);
    bool updateUser();
    bool deleteUser(const string& userName);
};



#endif //REPO_H
