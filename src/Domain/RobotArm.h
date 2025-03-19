//
// Created by dante on 2/13/25.
//

#ifndef DOMAIN_ROBOTARM_H
#define DOMAIN_ROBOTARM_H

#include <ctime>
#include <string>
#include <vector>

using namespace std;

namespace domain
{
    class Task;
    class Instruction;
    class Tool;
    class Position;

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
        Task* task_;
        Status status_;
        Position* currPosition_;
        string host_;
        string type_;
        Tool* tool_;
        bool isAbsolute_ = true;

    public:
        RobotArm(const string& name, Task* task, Status status, Position* currPosition, const string& host, const string& type, Tool* tool);

        virtual ~RobotArm();

        [[nodiscard]] Status getStatus() const;

        void setStatus(Status status);

        [[nodiscard]] Task* getTask() const;

        void setTask(Task* task);

        [[nodiscard]] Position* getCurrPosition() const;

        void setCurrPosition(Position* currPosition);

        [[nodiscard]] string getHost() const;

        void setHost(const string& host);

        [[nodiscard]] string getName() const;

        void setName(const string& name);

        [[nodiscard]] string getType() const;

        void setType(const string& type);

        [[nodiscard]] Tool* getTool() const;

        void setTool(Tool* tool);

        [[nodiscard]] bool isAbsolute() const;

        void setAbsolute(bool is_absolute);
    };
};


#endif //DOMAIN_ROBOTARM_H

