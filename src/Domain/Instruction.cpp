#include "Instruction.h"

#include <stdexcept>

domain::Instruction::Instruction(Position* position, const float gripForce, const double wait) :
position_(position),
gripForce_(gripForce),
wait_(wait){}

domain::Instruction::~Instruction() = default;

Position* domain::Instruction::getPosition() const
{
    return position_;
}

void domain::Instruction::setPosition(Position* position)
{
    position_=position;
}


float domain::Instruction::getGripForce() const
{
    return gripForce_;
}

void domain::Instruction::setGripForce(const float gripForce)
{
    gripForce<0 ? throw invalid_argument("grip can't be negative") : gripForce_=gripForce;
}

double domain::Instruction::getWait() const
{
    return wait_;
}

void domain::Instruction::setWait(double wait)
{
    wait<0 ? throw invalid_argument("wait can't be negative") : wait_=wait;
}
