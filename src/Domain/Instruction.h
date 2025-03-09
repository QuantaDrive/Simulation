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
        int wait_;
        bool goHome_;
        bool relative_;
        vec3 relMove_;

    public:
        Instruction(Position* position, float gripForce, int wait, bool goHome, bool relative, const vec3& relMove);

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

        [[nodiscard]] vec3 getRelMove() const;

        void setRelMove(vec3 relMove);
    };
}


#endif //DOMAIN_INSTRUCTION_H
