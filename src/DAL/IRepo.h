//
// Created by dante on 2/21/25.
//

#ifndef IREPO_H
#define IREPO_H

#include <memory>
#include <string>

using namespace std;

namespace domain {
    class RobotArm;
    class User;
}
class IRepo {
public:
    virtual ~IRepo() = default;

    IRepo()= default;

    [[nodiscard]] virtual domain::RobotArm* readArm(const std::string& armName) const=0;
    virtual bool createArm(const domain::RobotArm* arm)=0;
    virtual bool updateArm(const string& armName, const string& newName, const string& host)=0;
    virtual bool deleteArm(const string& armName)=0;

    [[nodiscard]] virtual domain::User* readUser(const string& userName) const=0;
    virtual bool createUser(domain::User* user)=0;
    virtual bool updateUser(const string& userName, const string& newName)=0;
    virtual bool deleteUser(const string& userName)=0;

    virtual void writeFile()=0;
};



#endif //IREPO_H
