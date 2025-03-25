//
// Created by dante on 3/8/25.
//

#ifndef GCODE_H
#define GCODE_H
#include <string>
#include <vector>

class PhysicalManager;
class SimulationManager;

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
    SimulationManager* simManager_;
    PhysicalManager* physManager_;
public:
    GCode(SimulationManager* simManager, PhysicalManager* physManager = nullptr);
    virtual ~GCode() = default;
    static std::string toGCode(domain::Task* task);
    domain::Task* parseGCode(std::vector<std::string>& gCode);
    void saveToFile(const std::string& fileName);
    void loadFromFile(const std::string& fileName);
protected:
    std::vector<std::string> split(std::string s, const std::string& delimiter);
};



#endif //GCODE_H
