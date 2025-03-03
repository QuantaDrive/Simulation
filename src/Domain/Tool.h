//
// Created by dante on 2/24/25.
//

#ifndef DOMAIN_TOOL_H
#define DOMAIN_TOOL_H
#include <string>
#include <glm/glm.hpp>


using namespace std;
using namespace glm;

namespace domain
{
    class Position;
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


#endif //DOMAIN_TOOL_H
