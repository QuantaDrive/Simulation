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
    DEFECT,
    UNKNOWN
};

class RobotArm {
private:
    string name_;
    vector<Task*> tasks_;
    Status status_;
    Instruction* currPosition_;
    string host_;
    string type_;

public:
    RobotArm(const string& name, const vector<Task*>& tasks, Status status, Instruction* currPosition, const string& host, const string& type);

    virtual ~RobotArm();

    [[nodiscard]] Status getStatus() const;

    void setStatus(Status status);

    [[nodiscard]] vector<Task*> getTasks() const;

    void setTasks(const vector<Task*>& tasks);

    [[nodiscard]] Instruction* getCurrPosition() const;

    void setCurrPosition(Instruction* currPosition);

    [[nodiscard]] string getHost() const;

    void setHost(const string& host);

    [[nodiscard]] string getName() const;

    void setName(const string& name);

    [[nodiscard]] string getType() const;

    void setType(const string& type);


};



#endif //ROBOTARM_H
