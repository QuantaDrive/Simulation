#include "Instruction.h"

#include <stdexcept>

#include "Position.h"

domain::Instruction::Instruction(Position* position, const float gripForce, const int wait, const bool goHome, const bool relative, const float velocity) :
position_(position),
gripForce_(gripForce),
wait_(wait),
goHome_(goHome),
relative_(relative),
velocity_(velocity){}

domain::Instruction::~Instruction() = default;

domain::Position *domain::Instruction::getPosition() const
{
    return position_;
}

void domain::Instruction::setPosition(domain::Position* position)
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


bool domain::Instruction::isRapid() const
{
    return rapid_;
}

void domain::Instruction::setRapid(bool rapid)
{
    rapid_=rapid;
}

bool domain::Instruction::isLinear() const {
    return linear_;
}

void domain::Instruction::setLinear(bool linear) {
    linear_ = linear;
}

float domain::Instruction::getVelocity() const
{
    return velocity_;
}

void domain::Instruction::setVelocity(const float velocity)
{
    velocity_=velocity;
}
