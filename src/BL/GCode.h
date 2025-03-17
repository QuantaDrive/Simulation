//
// Created by dante on 3/8/25.
//

#ifndef GCODE_H
#define GCODE_H
#include <string>
#include <vector>


namespace domain
{
    class Instruction;
    class Task;
    class RobotArm;
}

namespace simulation
{
    class RobotArm;
}

class GCode {
private:
    domain::RobotArm* robotArm_;
    float maxVel_;
    float maxAcc_;
public:
    GCode(simulation::RobotArm* simulationArm, domain::RobotArm* robotArm);
    virtual ~GCode() = default;
    std::string toGCode(domain::Task* task);
    domain::Task* parseGCode(std::string& gCode);
    void saveToFile(const std::string& fileName);
    void loadFromFile(const std::string& fileName);
protected:
    std::vector<std::string> split(std::string s, const std::string& delimiter);
};



#endif //GCODE_H
