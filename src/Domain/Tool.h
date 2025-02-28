//
// Created by dante on 2/24/25.
//

#ifndef TOOL_H
#define TOOL_H
#include <string>
#include <glm/glm.hpp>


class Position;
using namespace std;
using namespace glm;

namespace domain
{
    class Tool {
    private:
        string name_;
        Position* position_;
    public:
        Tool(const string& name, Position* position);
        ~Tool() = default;

        [[nodiscard]] string getName() const;
        void setName(const string& name);
        [[nodiscard]] Position* getPosition() const;
        void setPosition(Position* position);
    };
}


#endif //TOOL_H
