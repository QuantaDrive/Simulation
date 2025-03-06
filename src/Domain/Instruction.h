#ifndef DOMAIN_INSTRUCTION_H
#define DOMAIN_INSTRUCTION_H

#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Position;

namespace domain
{
    class Instruction {
    private:
        Position* position_;
        float gripForce_;
        double wait_;

    public:
        Instruction(Position* position, float gripForce, double wait);

        ~Instruction();

        [[nodiscard]] Position* getPosition() const;

        void setPosition(Position* position);

        [[nodiscard]] float getGripForce() const;

        void setGripForce(float gripForce);
    };
}


#endif //DOMAIN_INSTRUCTION_H
