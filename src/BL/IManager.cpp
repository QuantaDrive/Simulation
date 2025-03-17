//
// Created by dante on 3/8/25.
//

#include "IManager.h"

#include <string>
#include <ctime>
#include <stdexcept>
#include <regex.h>
#include <bits/regex.h>
#include <glm/gtc/constants.hpp>

#include "../Domain/Task.h"
#include "../Domain/Instruction.h"
#include "../Domain/Position.h"

string IManager::toGCode(domain::Task* task)
{
    bool setRelative = false;
    string gCode = "";
    for (const auto instruction : task->getInstructions())
    {
        if (instruction->getWait()>0) gCode += "G4 P"+to_string(instruction->getWait()*1000)+"\n";
        else if (instruction->isRelative())
        {
            setRelative = true;
            gCode += "G91\n";
            gCode += "G0 X"+to_string(instruction->getPosition()->getCoords()[0])+" Y"+to_string(instruction->getPosition()->getCoords()[1])+" Z"+to_string(instruction->getPosition()->getCoords()[2])+"\n";

        }
        else if (instruction->isGoHome()) gCode += "G28\n";
        else
        {
            if (setRelative)
            {
                setRelative = false;
                gCode += "G90\n";
            }
            gCode += "G0 X"+to_string(instruction->getPosition()->getCoords()[0])+" Y"+to_string(instruction->getPosition()->getCoords()[1])+" Z"+to_string(instruction->getPosition()->getCoords()[2])+"\n";
        }
    }
    return gCode;
}

std::vector<std::string> IManager::split(std::string s, const std::string& delimiter) {
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

domain::Task* IManager::parseGCode(std::string& gCode)
{
    time_t timestamp = time(&timestamp);
    tm datetime = *localtime(&timestamp);
    domain::Task* task = new domain::Task(datetime,{});
    bool nextRelative = false;
    std::regex re("(G|M)*");
    auto lines = split(gCode, "\n");
    for (auto line : lines)
    {
        auto codeParts = split(line, " ");
        if (!regex_search(codeParts[0], re))
        {
            throw runtime_error("Invalid Gcode");
        }
        re.assign("G0 X[0-9]{3} Y[0-9]{3} Z[0-9]{3}");
        if (regex_search(gCode, re))
        {
            if (nextRelative)
            {
                vec3 relMove = vec3({codeParts[1].substr(1),codeParts[2].substr(1),codeParts[3].substr(1)});
                nextRelative = false;
                task->getInstructions().emplace_back(new domain::Instruction());
            }
            auto* p = new domain::Position({codeParts[1].substr(1),codeParts[2].substr(1),codeParts[3].substr(1)}, {0,0,0});
            task->getInstructions().emplace_back(new domain::Instruction(p, 0, 0, false, false, maxVel_));
        }
        re.assign("G1 X[0-9]{3} Y[0-9]{3} Z[0-9]{3} F[0-9]{3}");
        if (regex_search(gCode, re))
        {
            float velocity = strtof(codeParts[4].substr(1).c_str(), nullptr);
            auto* p = new domain::Position({codeParts[1].substr(1),codeParts[2].substr(1),codeParts[3].substr(1)}, {0,0,0});
            task->getInstructions().emplace_back(new domain::Instruction(p, 0, 0, false, nextRelative, velocity));
            nextRelative = false;
        }
        re.assign("G91");
        if (regex_search(gCode, re))
        {
            nextRelative = true;
            break;
        }

    }

}


