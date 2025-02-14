#ifndef ARMPOSITION_H
#define ARMPOSITION_H

#include <vector>

using namespace std;

class ArmPosition {
private:
    vector<float> position_;
    vector<float> degrees_;
    float gripForce_;

public:
    ArmPosition(const vector<float>& position, const vector<float>& degrees, float gripForce);

    ~ArmPosition();

    [[nodiscard]] vector<float> getPosition() const;

    void setPosition(const vector<float>& position);

    [[nodiscard]] vector<float> getDegrees() const;

    void setDegrees(const vector<float>& degrees);

    [[nodiscard]] float getGripForce() const;

    void setGripForce(float gripForce);
};



#endif //ARMPOSITION_H
