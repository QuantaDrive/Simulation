//
// Created by dante on 2/24/25.
//

#include "Position.h"

#include "Instruction.h"
#include "Instruction.h"

Position::Position(const vec3& coords, const vec3& rotation):
coords_(coords),
rotation_(rotation){}

vec3 Position::getCoords() const
{
    return coords_;
}

void Position::setCoords(const vec3& coords)
{
    coords_=coords;
}

vec3 Position::getRotation() const
{
    return rotation_;
}

void Position::setRotation(const vec3& rotation)
{
    rotation_=rotation;
}
