#ifndef ARMPOSITION_H
#define ARMPOSITION_H

#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class ArmPosition {
private:
    vec3 position_;
    vec3 degrees_;
    float gripForce_;

public:
    ArmPosition(const vec3& position, const vec3& degrees, float gripForce);

    ~ArmPosition();

    [[nodiscard]] vec3 getPosition() const;

    void setPosition(const vec3& position);

    [[nodiscard]] vec3 getDegrees() const;

    void setDegrees(const vec3& degrees);

    [[nodiscard]] float getGripForce() const;

    void setGripForce(float gripForce);
};



#endif //ARMPOSITION_H
