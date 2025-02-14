#include "ArmPosition.h"

ArmPosition::ArmPosition(float xPosition, float yPosition, float zPosition, float xDegrees, float yDegrees, float zDegrees, float gripForce) : xPosition_(xPosition), yPosition_(yPosition), zPosition_(zPosition), xDegrees_(xDegrees), yDegrees_(yDegrees), zDegrees_(zDegrees), gripForce_(gripForce){}

ArmPosition::~ArmPosition() = default;

float ArmPosition::get_x_position() const
{
    return xPosition_;
}

void ArmPosition::set_x_position(const float x_position)
{
    xPosition_=x_position;
}

float ArmPosition::get_y_position() const
{
    return yPosition_;
}

void ArmPosition::set_y_position(const float y_position)
{
    yPosition_=y_position;
}

float ArmPosition::get_z_position() const
{
    return zPosition_;
}

void ArmPosition::set_z_position(const float z_position)
{
    zPosition_=z_position;
}

float ArmPosition::get_x_degrees() const
{
    return xDegrees_;
}

void ArmPosition::set_x_degrees(const float x_degrees)
{
    xDegrees_=x_degrees;
}

float ArmPosition::get_y_degrees() const
{
    return yDegrees_;
}

void ArmPosition::set_y_degrees(const float y_degrees)
{
    yDegrees_=y_degrees;
}

float ArmPosition::get_z_degrees() const
{
    return zDegrees_;
}

void ArmPosition::set_z_degrees(const float z_degrees)
{
    zDegrees_=z_degrees;
}

float ArmPosition::get_grip_force() const
{
    return gripForce_;
}

void ArmPosition::set_grip_force(const float grip_force)
{
    gripForce_=grip_force;
}
