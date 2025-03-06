//
// Created by dante on 2/13/25.
//

#ifndef DOMAIN_USER_H
#define DOMAIN_USER_H

#include <string>

using namespace std;

namespace domain
{
    class User {
    private:
        string name_;

    public:
        explicit User(const string& name);

        [[nodiscard]] string getName() const;

        void setName(const string &name);
    };
}


#endif //USER_H
