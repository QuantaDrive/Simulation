#include "ArmPosition.h"

ArmPosition::ArmPosition(const vec3& position, const vec3& degrees, float gripForce) : position_(position), degrees_(degrees), gripForce_(gripForce) {}

ArmPosition::~ArmPosition() = default;

vec3 ArmPosition::getPosition() const
{
    return position_;
}

void ArmPosition::setPosition(const vec3& position)
{
    position_=position;
}

vec3 ArmPosition::getDegrees() const
{
    return degrees_;
}

void ArmPosition::setDegrees(const vec3& degrees)
{
    degrees_=degrees;
}

float ArmPosition::getGripForce() const
{
    return gripForce_;
}

void ArmPosition::setGripForce(float gripForce)
{
    gripForce_=gripForce;
}
