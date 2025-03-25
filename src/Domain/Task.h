//
// Created by dante on 2/12/25.
//

#ifndef DOMAIN_TASK_H
#define DOMAIN_TASK_H

#include <ctime>
#include <string>
#include <vector>

using namespace std;

namespace domain
{
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

        void clearInstructions();

        void setInstructions(const vector<Instruction*>& instructions);

        void addInstruction(Instruction* instruction);
    };
}

#endif //TASK_H
