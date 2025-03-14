#include "Instruction.h"

#include <stdexcept>

domain::Instruction::Instruction(Position* position, const float gripForce, const int wait, const bool goHome, const bool relative, const vec3& relMove) :
position_(position),
gripForce_(gripForce),
wait_(wait),
goHome_(goHome),
relative_(relative),
relMove_(relMove){}

domain::Instruction::~Instruction() = default;

domain::Position *domain::Instruction::getPosition() const
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

int domain::Instruction::getWait() const
{
    return wait_;
}

void domain::Instruction::setWait(int wait)
{
    wait<0 ? throw invalid_argument("wait can't be negative") : wait_=wait;
}

bool domain::Instruction::isGoHome() const
{
    return goHome_;
}

void domain::Instruction::setGoHome(bool goHome)
{
    goHome_=goHome;
}

bool domain::Instruction::isRelative() const
{
    return relative_;
}

void domain::Instruction::setRelative(bool relative)
{
    relative_=relative;
}

vec3 domain::Instruction::getRelMove() const
{
    return relMove_;
}

void domain::Instruction::setRelMove(vec3 relMove)
{
    relMove_=relMove;
}
