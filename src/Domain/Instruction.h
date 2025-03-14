#ifndef DOMAIN_INSTRUCTION_H
#define DOMAIN_INSTRUCTION_H

#include <vector>
#include <glm/glm.hpp>

#include "Node.h"

using namespace std;
using namespace glm;

class Position;

namespace domain
{
    class Instruction {
    private:
        Position* position_;
        float gripForce_;
        int wait_;
        bool goHome_;
        bool relative_;
        Position* relMove_;
        bool rapid_;
        float velocity_;

    public:
        Instruction(Position* position, float gripForce, int wait, bool goHome, bool relative, Position* relMove, float velocity);

        Instruction() = default;

        ~Instruction();

        [[nodiscard]] Position* getPosition() const;

        void setPosition(Position* position);

        [[nodiscard]] float getGripForce() const;

        void setGripForce(float gripForce);

        [[nodiscard]] int getWait() const;

        void setWait(int wait);

        [[nodiscard]] bool isGoHome() const;

        void setGoHome(bool goHome);

        [[nodiscard]] bool isRelative() const;

        void setRelative(bool relative);

        [[nodiscard]] Position* getRelMove() const;

        void setRelMove(Position* relMove);

        [[nodiscard]] bool isRapid() const;

        void setRapid(bool rapid);

        [[nodiscard]] float getVelocity() const;

        void setVelocity(float velocity);

    };
}


#endif //DOMAIN_INSTRUCTION_H
