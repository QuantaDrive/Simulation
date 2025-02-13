//
// Created by dante on 2/13/25.
//

#ifndef ROBOTARM_H
#define ROBOTARM_H

#include "Task.h"

#include <vector>

using namespace std;

enum Status
{
    READY,
    BUSY,
    DEFECT
};

class RobotArm {
private:
    vector<Task*> tasks;
    Status status;

public:
    RobotArm(const vector<Task*>& tasks, Status status)
        : tasks(tasks),
          status(status)
    {
    }

    ~RobotArm();

    [[nodiscard]] Status getStatus() const;

    void setStatus(Status status);

    [[nodiscard]] vector<Task*> getTasks() const;

    void setTasks(const vector<Task*>& tasks);
};



#endif //ROBOTARM_H
