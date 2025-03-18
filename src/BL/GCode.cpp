//
// Created by dante on 3/8/25.
//

#include "GCode.h"

#include <string>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <regex.h>
#include <regex>
#include <glm/gtc/constants.hpp>

#include "../Domain/Task.h"
#include "../Domain/Instruction.h"
#include "../Domain/Position.h"
#include "../Domain/RobotArm.h"
#include "../Physical/BL/PhysicalManager.h"
#include "../Simulation/BL/SimulationManager.h"

GCode::GCode(SimulationManager* simManager, PhysicalManager* physManager) :
simManager_(simManager),
physManager_(physManager)
{}

string GCode::toGCode()
{
    Task* task = simManager_->getRobotArm()->getTask();
    bool alreadyRelative = false;
    bool alreadyAbsolute = false;
    string gCode = "";
    for (const auto instruction : task->getInstructions())
    {
        if (instruction->getWait()>0)
        {
            gCode += "G4 P"+to_string(instruction->getWait())+"\n";
            continue;
        }
        if (instruction->isGoHome())
        {
            gCode += "G28\n";
            continue;
        }
        if (instruction->isRelative() && !alreadyRelative)
        {
            alreadyRelative = true;
            alreadyAbsolute = false;
            gCode += "G91\n";
        }
        if (!instruction->isRelative() && !alreadyAbsolute)
        {
            alreadyRelative = false;
            alreadyAbsolute = true;
            gCode += "G90\n";
        }
        if (instruction->isRapid())
        {
            gCode += "G0 X"+to_string(instruction->getPosition()->getCoords()[0])+" Y"+to_string(instruction->getPosition()->getCoords()[1])+" Z"+to_string(instruction->getPosition()->getCoords()[2])+"\n";
        }
        else gCode += "G1 X"+to_string(instruction->getPosition()->getCoords()[0])+" Y"+to_string(instruction->getPosition()->getCoords()[1])+" Z"+to_string(instruction->getPosition()->getCoords()[2])+" F"+to_string(instruction->getVelocity())+"\n";
    }
    return gCode;
}

std::vector<std::string> GCode::split(std::string s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s = s.substr(pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

domain::Task* GCode::parseGCode(vector<std::string>& gCode)
{
    time_t timestamp = time(&timestamp);
    tm datetime = *localtime(&timestamp);
    domain::Task* task = new domain::Task(datetime,{});
    bool relative = false;
    for (auto line : gCode)
    {
        std::regex re = regex("(G|M).*", regex_constants::extended);
        auto codeParts = split(line, " ");
        if (!regex_search(codeParts[0], re))
        {
            cout << line << endl;
            throw runtime_error("Invalid Gcode");
        }
        re.assign("G0 X-?[0-9]{0,3}(\\.[0-9]{0,6})? Y-?[0-9]{0,3}(\\.[0-9]{0,6})? Z-?[0-9]{0,3}(\\.[0-9]{0,6})?");
        if (regex_search(line, re))
        {
            auto* p = new domain::Position({strtof(codeParts[1].substr(1).c_str(), nullptr),strtof(codeParts[2].substr(1).c_str(), nullptr),strtof(codeParts[3].substr(1).c_str(), nullptr)}, {0,0,0});
            auto* i = new domain::Instruction(p, 0, 0, false, relative, simManager_->getSimulationArm()->getMaxVel());
            i->setRapid(true);
            task->addInstruction(i);

            continue;
        }
        re.assign("G1 X-?[0-9]{0,3}(\\.[0-9]{0,6})? Y-?[0-9]{0,3}(\\.[0-9]{0,6})? Z-?[0-9]{0,3}(\\.[0-9]{0,6})? F[0-9]{0,3}(\\.[0-9]{0,6})?");
        if (regex_search(line, re))
        {
            float velocity = strtof(codeParts[4].substr(1).c_str(), nullptr);
            auto* p = new domain::Position({strtof(codeParts[1].substr(1).c_str(), nullptr),strtof(codeParts[2].substr(1).c_str(), nullptr),strtof(codeParts[3].substr(1).c_str(), nullptr)}, {0,0,0});
            auto* i = new domain::Instruction(p, 0, 0, false, relative, velocity);
            i->setLinear(true);
            task->addInstruction(i);
            continue;
        }
        re.assign("G91");
        if (regex_search(line, re))
        {
            relative = true;
            continue;
        }
        re.assign("G90");
        if (regex_search(line, re))
        {
            relative = false;
            continue;
        }
        re.assign("G4 P[0-9]+");
        if (regex_search(line, re))
        {
            const int wait = stoi(codeParts[1].substr(1));
            task->addInstruction(new domain::Instruction(nullptr,0,wait, false, relative, 0));
        }
        re.assign("G28");
        if (regex_search(line, re))
        {
            task->addInstruction(new domain::Instruction(nullptr,0,0,true,false,simManager_->getSimulationArm()->getMaxVel()));
        }

    }
    return task;
}

void GCode::saveToFile(const std::string& fileName)
{
    ofstream out;
    out.open(fileName);
    out << toGCode();
    out.close();

}

void GCode::loadFromFile(const std::string& fileName)
{
    ifstream in;
    in.open(fileName);
    vector<string> lines;
    std::string line;
    while (getline(in,line))
    {
        lines.emplace_back(line);
    }
    in.close();
    simManager_->getRobotArm()->setTask(parseGCode(lines));
}
