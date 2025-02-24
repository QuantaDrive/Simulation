//
// Created by dante on 2/24/25.
//

#ifndef POSITION_H
#define POSITION_H
#include <glm/glm.hpp>

using namespace glm;

class Position {
private:
    vec3 coords_;
    vec3 rotation_;
public:
    Position(const vec3& coords, const vec3& rotation);
    ~Position()=default;

    [[nodiscard]] vec3 getCoords() const;
    void setCoords(const vec3& coords);
    [[nodiscard]] vec3 getRotation() const;
    void setRotation(const vec3& rotation);

};



#endif //POSITION_H
