#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Instruction {
private:
    vec3 position_;
    vec3 degrees_;
    float gripForce_;
    double wait_;

public:
    Instruction(const vec3& position, const vec3& degrees, float gripForce);

    ~Instruction();

    [[nodiscard]] vec3 getPosition() const;

    void setPosition(const vec3& position);

    [[nodiscard]] vec3 getDegrees() const;

    void setDegrees(const vec3& degrees);

    [[nodiscard]] float getGripForce() const;

    void setGripForce(float gripForce);
};



#endif //INSTRUCTION_H
