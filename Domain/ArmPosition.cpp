#include "ArmPosition.h"

ArmPosition::ArmPosition(float xPosition, float yPosition, float zPosition, float xDegrees, float yDegrees, float zDegrees, float gripForce) {
    this->xPosition = xPosition;
    this->yPosition = yPosition;
    this->zPosition = zPosition;
    this->xDegrees = xDegrees;
    this->yDegrees = yDegrees;
    this->zDegrees = zDegrees;
    this->gripForce = gripForce;
}

ArmPosition::~ArmPosition() {}

float ArmPosition::get_x_position() const
{
    return xPosition;
}

void ArmPosition::set_x_position(float x_position)
{
    xPosition=x_position;
}

float ArmPosition::get_y_position() const
{
    return yPosition;
}

void ArmPosition::set_y_position(float y_position)
{
    yPosition=y_position;
}

float ArmPosition::get_z_position() const
{
    return zPosition;
}

void ArmPosition::set_z_position(float z_position)
{
    zPosition=z_position;
}

float ArmPosition::get_x_degrees() const
{
    return xDegrees;
}

void ArmPosition::set_x_degrees(float x_degrees)
{
    xDegrees=x_degrees;
}

float ArmPosition::get_y_degrees() const
{
    return yDegrees;
}

void ArmPosition::set_y_degrees(float y_degrees)
{
    yDegrees=y_degrees;
}

float ArmPosition::get_z_degrees() const
{
    return zDegrees;
}

void ArmPosition::set_z_degrees(float z_degrees)
{
    zDegrees=z_degrees;
}

float ArmPosition::get_grip_force() const
{
    return gripForce;
}

void ArmPosition::set_grip_force(float grip_force)
{
    gripForce=grip_force;
}
