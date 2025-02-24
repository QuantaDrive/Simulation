//
// Created by dante on 2/12/25.
//

#ifndef TASK_H
#define TASK_H

#include <ctime>
#include <string>
#include <vector>

using namespace std;

class Instruction;
class Task {
private:
    string id_;
    tm timestamp_;
    vector<Instruction*> instructions_;

public:
    Task(const tm& timestamp, const vector<Instruction*>& instructions);

    ~Task();

    [[nodiscard]]tm getTimestamp() const;

    void setTimestamp(const tm& timestamp);

    [[nodiscard]] vector<Instruction*> getInstructions() const;

    void setInstructions(const vector<Instruction*>& instructions);
};


#endif //TASK_H
