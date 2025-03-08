//
// Created by dante on 3/8/25.
//

#include "IManager.h"

#include <string>
#include <ctime>
#include <stdexcept>
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
            gCode += "G0 X"+to_string(instruction->getRelMove()[0])+" Y"+to_string(instruction->getRelMove()[1])+" Z"+to_string(instruction->getRelMove()[2])+"\n";

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

std::vector<std::string> split(std::string& s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

domain::Instruction* IManager::parseGCode(std::string gCode)
{
    regex re("G*");
    auto codeParts = split(gCode, " ");
    if (!regex_search(codeParts[0], re))
    {
        throw runtime_error('Invalid Gcode');
    }
    re.assign("(G0|G1) X[0-9]{3} Y[0-9]{3} Z[0-9]{3}");
    if (regex_search(gCode, re))
    {

        //return new domain::Instruction();
    }

}
