#include "Instruction.h"

Instruction::Instruction(const vec3& position, const vec3& degrees, float gripForce) :
position_(position),
degrees_(degrees),
gripForce_(gripForce) {}

Instruction::~Instruction() = default;

vec3 Instruction::getPosition() const
{
    return position_;
}

void Instruction::setPosition(const vec3& position)
{
    position_=position;
}

vec3 Instruction::getDegrees() const
{
    return degrees_;
}

void Instruction::setDegrees(const vec3& degrees)
{
    degrees_=degrees;
}

float Instruction::getGripForce() const
{
    return gripForce_;
}

void Instruction::setGripForce(float gripForce)
{
    gripForce_=gripForce;
}
