//
// Created by dante on 2/13/25.
//

#ifndef USER_H
#define USER_H

#include <string>

using namespace std;

class User {
private:
    string name;

public:
    explicit User(const string& name) : name(name) {}

    [[nodiscard]] string getName() const;

    void setName(const string &name);
};



#endif //USER_H
