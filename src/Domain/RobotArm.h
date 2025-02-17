//
// Created by dante on 2/13/25.
//

#ifndef ROBOTARM_H
#define ROBOTARM_H

#include <string>

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
    string name_;
    vector<Task*> tasks_;
    Status status_;
    ArmPosition* currPosition_;
    string host_;

public:
    RobotArm(const string& name, const vector<Task*>& tasks, Status status, ArmPosition* currPosition, const string& host);

    virtual ~RobotArm();

    [[nodiscard]] Status getStatus() const;

    void setStatus(Status status);

    [[nodiscard]] vector<Task*> getTasks() const;

    void setTasks(const vector<Task*>& tasks);

    [[nodiscard]] ArmPosition* getCurrPosition() const;

    void setCurrPosition(ArmPosition* currPosition);

    string getHost() const;

    void setHost(const string& host);

    string getName() const;

    void setName(const string& name);
};



#endif //ROBOTARM_H
