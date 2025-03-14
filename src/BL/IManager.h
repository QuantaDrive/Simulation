//
// Created by dante on 3/8/25.
//

#ifndef IMANAGER_H
#define IMANAGER_H
#include <string>
#include <vector>


namespace domain
{
    class Instruction;
    class Task;
}

class IManager {
private:
    float maxVel_;
    float maxAcc_;
public:
    IManager() = default;
    virtual ~IManager() = default;
protected:
    std::string toGCode(domain::Task* task);
    domain::Task* parseGCode(std::string& gCode);
    std::vector<std::string> split(std::string s, const std::string& delimiter);
};



#endif //IMANAGER_H
