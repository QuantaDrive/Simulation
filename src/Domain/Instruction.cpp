#include "Instruction.h"

Instruction::Instruction(Position* position, const float gripForce, const double wait) :
position_(position),
gripForce_(gripForce),
wait_(wait){}

Instruction::~Instruction() = default;

Position* Instruction::getPosition() const
{
    return position_;
}

void Instruction::setPosition(Position* position)
{
    position_=position;
}


float Instruction::getGripForce() const
{
    return gripForce_;
}

void Instruction::setGripForce(const float gripForce)
{
    gripForce_=gripForce;
}
