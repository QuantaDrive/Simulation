#include "ArmPosition.h"

ArmPosition::ArmPosition(const vector<float>& position, const vector<float>& degrees, float gripForce) : position_(position), degrees_(degrees), gripForce_(gripForce) {}

ArmPosition::~ArmPosition() = default;

vector<float> ArmPosition::getPosition() const
{
    return position_;
}

void ArmPosition::setPosition(const vector<float>& position)
{
    position_=position;
}

vector<float> ArmPosition::getDegrees() const
{
    return degrees_;
}

void ArmPosition::setDegrees(const vector<float>& degrees)
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
