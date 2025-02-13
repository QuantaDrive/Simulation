//
// Created by dante on 2/12/25.
//

#ifndef TASK_H
#define TASK_H

#include <ctime>
#include <vector>

#include "ArmPosition.h"

using namespace std;

class Task {
private:
    tm timestamp{};
    vector<ArmPosition*> positions;

public:
    Task(const tm& timestamp, const vector<ArmPosition*>& positions);

    ~Task();

    [[nodiscard]]tm getTimestamp() const;

    void setTimestamp(const tm& timestamp);

    [[nodiscard]] vector<ArmPosition*> getPositions() const;

    void setPositions(const vector<ArmPosition*>& positions);
};


#endif //TASK_H
