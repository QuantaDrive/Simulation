//
// Created by dante on 2/21/25.
//

#ifndef IREPO_H
#define IREPO_H

#include <string>

using namespace std;

class RobotArm;
class User;
class IRepo {
public:
    virtual ~IRepo() = default;

    IRepo()= default;

    [[nodiscard]] virtual RobotArm* readArm(const std::string& armName) const=0;
    virtual bool createArm(const RobotArm* arm)=0;
    virtual bool updateArm(const string& armName, const string& newName, const string& host)=0;
    virtual bool deleteArm(const string& armName)=0;

    [[nodiscard]] virtual User* readUser(const string& userName) const=0;
    virtual bool createUser(User* user)=0;
    virtual bool updateUser(const string& userName, const string& newName)=0;
    virtual bool deleteUser(const string& userName)=0;

    virtual void writeFile()=0;
};



#endif //IREPO_H
