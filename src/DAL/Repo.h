//
// Created by dante on 2/17/25.
//

#ifndef REPO_H
#define REPO_H
#include "../Domain/RobotArm.h"
#include "../Domain/User.h"


class Repo {
public:
    Repo() = default;

    RobotArm* readArm() const;
    bool createArm(RobotArm* arm);
    bool updateArm();
    bool deleteArm(const string& armName);

    User* readUser() const;
    bool createUser(User* user);
    bool updateUser();
    bool deleteUser(const string& userName);
};



#endif //REPO_H
